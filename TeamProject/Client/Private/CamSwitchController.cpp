#include "pch.h"
#include "CamSwitchController.h"

#include "CamDirector.h"
#include "CharacterController.h"
#include "Animator3D.h"

namespace
{
    Quaternion YawPitchRollQuatDeg(_float yawDeg, _float pitchDeg, _float rollDeg)
    {
        return Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(yawDeg), XMConvertToRadians(pitchDeg), XMConvertToRadians(rollDeg));
    }

    Vector3 OrbitPos(const Vector3& pivotWorld, const Quaternion& q, _float dist)
    {
        const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
        return pivotWorld + backDir * dist;
    }

    Client::CamSwitchController::Pose LerpPose(const Client::CamSwitchController::Pose& a, const Client::CamSwitchController::Pose& b, _float t)
    {
        Client::CamSwitchController::Pose out{};
        out.pivotWorld = Vector3::Lerp(a.pivotWorld, b.pivotWorld, t);
        out.yawWorldDeg = a.yawWorldDeg + Math::WrapDeg(b.yawWorldDeg - a.yawWorldDeg) * t;
        out.pitchDeg = Math::Lerp(a.pitchDeg, b.pitchDeg, t);
        out.rollDeg = a.rollDeg + Math::WrapDeg(b.rollDeg - a.rollDeg) * t;
        out.dist = Math::Lerp(a.dist, b.dist, t);
        return out;
    }

    Vector3 LockPivotPos(OBJECT_HANDLE h, _float offsetY)
    {
        auto obj = ObjectManager()->Request_Object(h);
        if (!obj)
            return { 0,0,0 };
        auto anim = obj->Get_Component<CAnimator3D>();
        _float4x4 m{};
        anim->Get_BipWorld(&m);
        return Vector3(m._41, m._42, m._43) + Vector3(0.f, offsetY, 0.f);
    }

    _float YawFromDirDeg(const Vector3& dir)
    {
        Vector3 d = dir;
        d.y = 0.f;

        const _float len = d.Length();
        if (len <= 0.f) return 0.f;

        d /= len;
        return XMConvertToDegrees(atan2f(d.x, d.z));
    }

    void PivotStab_ApplyTuning(Client::CamSwitchController::PivotStab& s, const Client::CamSwitchController::SwitchTuning::PivotFilter& t)
    {
        s.velTau = t.velTau;
        s.rawTau = t.rawTau;
        s.outlierDist = t.outlierDist;
        s.outlierVel = t.outlierVel;
    }

    void PivotStab_Reset(Client::CamSwitchController::PivotStab& s, const Vector3& pivot)
    {
        s.hasLast = true;
        s.lastRawPivot = pivot;
        s.filteredPivot = pivot;
        s.filteredVel = Vector3::Zero;
    }

    Vector3 PivotStab_Eval(Client::CamSwitchController::PivotStab& s, _float dt, const Vector3& rawPivot)
    {
        if (!s.hasLast)
        {
            PivotStab_Reset(s, rawPivot);
            return rawPivot;
        }

        Vector3 rawDelta = rawPivot - s.lastRawPivot;
        Vector3 rawVel = rawDelta / max(dt, 0.0001f);

        const _float rawJump = rawDelta.Length();
        const _float rawSpeed = rawVel.Length();

        if (rawJump > s.outlierDist || rawSpeed > s.outlierVel)
        {
            PivotStab_Reset(s, rawPivot);
            return rawPivot;
        }

        s.lastRawPivot = rawPivot;

        const _float aVel = Math::ExpAlpha(s.velTau, dt);
        s.filteredVel = Vector3::Lerp(s.filteredVel, rawVel, aVel);

        const _float speed = s.filteredVel.Length();
        const _float dynamicTau = s.rawTau / (1.f + speed * 0.15f);

        const _float aPos = Math::ExpAlpha(dynamicTau, dt);
        s.filteredPivot = Vector3::Lerp(s.filteredPivot, rawPivot, aPos);

        return s.filteredPivot;
    }
}

void CamSwitchController::Begin()
{
    if (core.active) return;

    core.active = true;
    core.state = State::Enter;
    core.elapsed = 0.f;

    lens.fovSaved = CameraManager()->GetFov();
    lens.fovFrom = lens.fovSaved;
    lens.holdDesiredFov = tune.common.zoomInDeg;

    hold.target = CamDirector()->GetCurHandle();

    pair.attacker = hold.target;
    pair.victim = CamDirector()->GetCurTarget();

    pair.aBase = Vector3::Zero;
    pair.aFace = Vector3::Zero;
    pair.aValid = false;

    pair.vBase = Vector3::Zero;
    pair.vFace = Vector3::Zero;
    pair.vValid = false;

    sw = {};
    sw.target.Reset();
    sw.active = false;
    PivotStab_ApplyTuning(sw.pivotStab, tune.pivot);

    hold.pivotStab = {};
    PivotStab_ApplyTuning(hold.pivotStab, tune.pivot);

    cancel = {};
    cancel.dur = max(tune.common.cancelFovSec, 0.f);
    cancel.ease = tune.common.cancelFovEase;

    auto orbit = CamDirector()->GetOrbitCam();
    orbit->SwitchMode_Begin();

    CaptureHoldPose();
    PivotStab_Reset(hold.pivotStab, hold.pose.pivotWorld);

    ApplyPose(hold.pose);
    ApplyFovTarget(lens.fovFrom);
}

void CamSwitchController::End()
{
    if (!core.active) return;

    const _float curFov = CameraManager()->GetFov();
    const _float delta = lens.fovSaved - curFov;
    if (delta != 0.f) CameraManager()->SetFov(delta, 0.f);

    auto orbit = CamDirector()->GetOrbitCam();
    orbit->ResumeSync();
    orbit->SwitchMode_End();

    core.active = false;
    core.state = State::None;
    core.elapsed = 0.f;

    hold = {};
    pair = {};
    sw = {};
    cancel = {};
    lens.fovSaved = 0.f;
    lens.fovFrom = 0.f;
    lens.holdDesiredFov = 0.f;
}

void CamSwitchController::Switch()
{
    if (!core.active) return;

    if (core.state == State::Switching || core.state == State::Recover)
    {
        BeginCancelRecover();
        return;
    }

    if (core.state == State::CancelRecover) return;

    const OBJECT_HANDLE cur = CamDirector()->GetCurHandle();
    if (!cur.isValid()) { BeginCancelRecover(); return; }
    if (!hold.target.isValid()) { BeginCancelRecover(); return; }

    if (cur == hold.target) { BeginCancelRecover(); return; }

    BeginSwitchTo(cur);
}

void CamSwitchController::BeginCancelRecover()
{
    cancel.fovFrom = CameraManager()->GetFov();
    cancel.fovTo = lens.fovSaved;
    cancel.dur = max(tune.common.cancelFovSec, 0.0001f);
    cancel.ease = tune.common.cancelFovEase;

    core.state = State::CancelRecover;
    core.elapsed = 0.f;
}

void CamSwitchController::EnsureAutoSwitch()
{
    const OBJECT_HANDLE cur = CamDirector()->GetCurHandle();
    if (!cur.isValid()) return;

    if (core.state == State::Switching || core.state == State::Recover) return;
    if (core.state == State::CancelRecover) return;

    if (!hold.target.isValid())
    {
        hold.target = cur;
        CaptureHoldPose();
        PivotStab_Reset(hold.pivotStab, hold.pose.pivotWorld);
        return;
    }

    if (cur == hold.target) return;

    BeginSwitchTo(cur);
}

void CamSwitchController::BeginSwitchTo(OBJECT_HANDLE newTarget)
{
    sw.active = true;
    sw.target = newTarget;

    sw.from = CaptureCurPose();

    sw.fovFrom = CameraManager()->GetFov();
    sw.fovTo = lens.fovSaved;

    pair.attacker = newTarget;
    pair.victim = CamDirector()->GetCurTarget();

    UpdatePairPivots(0.f);

    sw.goal = BuildGoalPose_SimplePair();
    PivotStab_Reset(sw.pivotStab, sw.goal.pivotWorld);

    sw.switchTo = sw.goal;

    core.state = State::Switching;
    core.elapsed = 0.f;
}

void CamSwitchController::Update(_float dt)
{
    if (!core.active) return;

    EnsureAutoSwitch();

    core.elapsed += dt;

    if (core.state == State::Enter)
    {
        const _float dur = max(tune.common.zoomInSec, 0.0001f);
        const _float u = clamp(core.elapsed / dur, 0.f, 1.f);
        const _float t = Math::ApplyEase(tune.common.zoomInEase, u);

        FollowHoldPivot(dt);
        ApplyPose(hold.pose);

        const _float desiredFov = Math::Lerp(lens.fovFrom, tune.common.zoomInDeg, t);
        ApplyFovTarget(desiredFov);

        if (u >= 1.f)
        {
            core.state = State::Hold;
            core.elapsed = 0.f;
        }
        return;
    }

    if (core.state == State::Hold)
    {
        FollowHoldPivot(dt);
        ApplyPose(hold.pose);

        ApplyFovTarget(lens.holdDesiredFov);
        return;
    }

    if (core.state == State::CancelRecover)
    {
        FollowHoldPivot(dt);
        ApplyPose(hold.pose);

        const _float desiredFov = EvalCancelFov(core.elapsed);
        ApplyFovTarget(desiredFov);

        if (core.elapsed >= cancel.dur)
        {
            End();
            return;
        }
        return;
    }

    if (core.state == State::Switching)
    {
        UpdatePairPivots(dt);

        const _float poseDur = max(tune.sw.blendSec, 0.0001f);
        const _float uPose = clamp(core.elapsed / poseDur, 0.f, 1.f);
        const _float tPose = Math::ApplyEase(tune.sw.blendEase, uPose);

        sw.goal = BuildGoalPose_SimplePair();

        const Vector3 rawPivot = sw.goal.pivotWorld;
        const Vector3 filteredPivot = PivotStab_Eval(sw.pivotStab, dt, rawPivot);

        sw.goal.pivotWorld = filteredPivot;
        sw.switchTo = sw.goal;

        Pose p = LerpPose(sw.from, sw.switchTo, tPose);
        p.rollDeg = Math::Lerp(sw.from.rollDeg, tune.sw.arriveRollDeg, tPose);

        ApplyPose(p);

        const _float desiredFov = EvalSwitchFov(core.elapsed);
        ApplyFovTarget(desiredFov);

        const _float fovDur = max(tune.sw.fovBlendSec, 0.0001f);
        const _float uFov = clamp(core.elapsed / fovDur, 0.f, 1.f);

        if (uPose >= 1.f && uFov >= 1.f)
        {
            sw.recoverFrom = p;
            sw.recoverTo = sw.goal;

            core.state = State::Recover;
            core.elapsed = 0.f;

            lens.holdDesiredFov = lens.fovSaved;
            return;
        }
        return;
    }

    if (core.state == State::Recover)
    {
        UpdatePairPivots(dt);

        sw.recoverTo = BuildGoalPose_SimplePair();

        const Vector3 rawPivot = sw.recoverTo.pivotWorld;
        const Vector3 filteredPivot = PivotStab_Eval(sw.pivotStab, dt, rawPivot);
        sw.recoverTo.pivotWorld = filteredPivot;

        const _float poseDur = max(tune.sw.recoverPoseSec, 0.0001f);
        const _float uPose = clamp(core.elapsed / poseDur, 0.f, 1.f);
        const _float tPose = Math::ApplyEase(tune.sw.recoverPoseEase, uPose);

        Pose p = LerpPose(sw.recoverFrom, sw.recoverTo, tPose);
        p.pivotWorld = sw.recoverTo.pivotWorld;

        const _float tRoll = EvalRollSettle(core.elapsed);
        p.rollDeg = Math::Lerp(sw.recoverFrom.rollDeg, 0.f, tRoll);

        ApplyPose(p);
        ApplyFovTarget(lens.fovSaved);

        const _float rollDur = max(tune.sw.rollSettleSec, 0.0001f);
        if (core.elapsed >= poseDur && core.elapsed >= rollDur)
        {
            hold.target = sw.target;
            hold.pose = sw.recoverTo;
            PivotStab_Reset(hold.pivotStab, hold.pose.pivotWorld);

            sw = {};
            PivotStab_ApplyTuning(sw.pivotStab, tune.pivot);

            core.state = State::Hold;
            core.elapsed = 0.f;
            return;
        }
        return;
    }
}

void CamSwitchController::CaptureHoldPose()
{
    auto orbit = CamDirector()->GetOrbitCam();

    OrbitSnapshot s{};
    orbit->CaptureSnapshot(s);

    hold.pose.yawWorldDeg = s.pose.rotCurDeg.x;
    hold.pose.pitchDeg = s.pose.rotCurDeg.y;
    hold.pose.rollDeg = 0.f;
    hold.pose.dist = s.pose.distCur;

    const _float offsetY = orbit->GetOffsetY() + tune.goal.pivotAddY;
    hold.pose.pivotWorld = LockPivotPos(hold.target, offsetY);

    hold.valid = true;
}

void CamSwitchController::FollowHoldPivot(_float dt)
{
    auto orbit = CamDirector()->GetOrbitCam();
    const _float offsetY = orbit->GetOffsetY() + tune.goal.pivotAddY;

    const Vector3 raw = LockPivotPos(hold.target, offsetY);
    hold.pose.pivotWorld = PivotStab_Eval(hold.pivotStab, dt, raw);
}

CamSwitchController::Pose CamSwitchController::CaptureCurPose() const
{
    auto orbit = CamDirector()->GetOrbitCam();

    OrbitSnapshot s{};
    orbit->CaptureSnapshot(s);

    Pose p{};
    p.pivotWorld = s.pose.pivotCurWorld;
    p.yawWorldDeg = s.pose.rotCurDeg.x;
    p.pitchDeg = s.pose.rotCurDeg.y;
    p.rollDeg = 0.f;
    p.dist = s.pose.distCur;

    return p;
}

_float CamSwitchController::CalcBehindYawDeg(OBJECT_HANDLE target) const
{
    auto obj = ObjectManager()->Request_Object(target);
    auto tf = obj->Get_Component<CTransform>();

    const Vector3 fwd = tf->Dir(STATE::LOOK);
    const _float targetYaw = YawFromDirDeg(fwd);

    return Math::WrapDeg(targetYaw + tune.goal.behindYawAddDeg);
}

void CamSwitchController::ApplyPose(const Pose& p) const
{
    auto orbit = CamDirector()->GetOrbitCam();

    const Quaternion qPos = YawPitchRollQuatDeg(p.yawWorldDeg, p.pitchDeg, 0.f);
    const Vector3 camPos = OrbitPos(p.pivotWorld, qPos, p.dist);

    const Quaternion qRot = YawPitchRollQuatDeg(p.yawWorldDeg, p.pitchDeg, p.rollDeg);
    orbit->SnapFromExternalPose(p.pivotWorld, camPos, qRot, p.dist);
}

void CamSwitchController::ApplyFovTarget(_float desiredFov)
{
    const _float curFov = CameraManager()->GetFov();
    const _float delta = desiredFov - curFov;
    if (delta != 0.f) CameraManager()->SetFov(delta, 0.f);
}

_float CamSwitchController::EvalCancelFov(_float tSec) const
{
    const _float dur = max(cancel.dur, 0.0001f);
    const _float u = clamp(tSec / dur, 0.f, 1.f);
    const _float t = Math::ApplyEase(cancel.ease, u);

    return Math::Lerp(cancel.fovFrom, cancel.fovTo, t);
}

_float CamSwitchController::EvalSwitchFov(_float tSec) const
{
    const _float dur = max(tune.sw.fovBlendSec, 0.0001f);
    const _float u = clamp(tSec / dur, 0.f, 1.f);
    const _float t = Math::ApplyEase(tune.sw.fovBlendEase, u);

    return Math::Lerp(sw.fovFrom, sw.fovTo, t);
}

_float CamSwitchController::EvalRollSettle(_float tSec) const
{
    const _float dur = max(tune.sw.rollSettleSec, 0.0001f);
    const _float u = clamp(tSec / dur, 0.f, 1.f);
    return Math::ApplyEase(tune.sw.rollSettleEase, u);
}

CamSwitchController::PivotSample CamSwitchController::SamplePivots(OBJECT_HANDLE h, _float offsetY, _float faceYOffsetMul) const
{
    PivotSample s{};
    if (!h.isValid()) return s;

    auto obj = ObjectManager()->Request_Object(h);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector3 foot = cc->Get_FootPosition();
    const _float half = cc->Get_HalfSize();

    const _float baseMul = 1.1f;
    const _float topMul = 1.3f;

    _float t = clamp(faceYOffsetMul, 0.f, 1.f);
    const _float faceMul = baseMul + (topMul - baseMul) * t;

    s.basePivot = foot + Vector3(0.f, half * baseMul + offsetY, 0.f);
    s.facePivot = foot + Vector3(0.f, half * faceMul + offsetY, 0.f);
    s.valid = true;
    return s;
}

void CamSwitchController::UpdatePairPivots(_float dt)
{
    auto orbit = CamDirector()->GetOrbitCam();
    const _float offsetY = orbit->GetOffsetY() + tune.goal.pivotAddY;

    const PivotSample aS = SamplePivots(pair.attacker, offsetY, tune.common.faceYOffsetMul);
    if (!aS.valid)
    {
        pair = {};
        return;
    }

    if (!pair.aValid || dt <= 0.f)
    {
        pair.aBase = aS.basePivot;
        pair.aFace = aS.facePivot;
        pair.aValid = true;
    }
    else
    {
        const _float t = clamp(dt * tune.common.pivotFollowLerpSpeed, 0.f, 1.f);
        pair.aBase = Vector3::Lerp(pair.aBase, aS.basePivot, t);
        pair.aFace = Vector3::Lerp(pair.aFace, aS.facePivot, t);
        pair.aValid = true;
    }

    const PivotSample vS = SamplePivots(pair.victim, offsetY, tune.common.faceYOffsetMul);
    if (!vS.valid)
    {
        pair.vBase = Vector3::Zero;
        pair.vFace = Vector3::Zero;
        pair.vValid = false;
        return;
    }

    Vector3 delta = vS.facePivot - pair.aFace;
    delta.y = 0.f;
    const _float dist = delta.Length();

    if (dist > tune.common.maxVictimDist)
    {
        pair.vBase = Vector3::Zero;
        pair.vFace = Vector3::Zero;
        pair.vValid = false;
        return;
    }

    if (!pair.vValid || dt <= 0.f)
    {
        pair.vBase = vS.basePivot;
        pair.vFace = vS.facePivot;
        pair.vValid = true;
        return;
    }

    const _float t = clamp(dt * tune.common.pivotFollowLerpSpeed, 0.f, 1.f);
    pair.vBase = Vector3::Lerp(pair.vBase, vS.basePivot, t);
    pair.vFace = Vector3::Lerp(pair.vFace, vS.facePivot, t);
    pair.vValid = true;
}

CamSwitchController::Pose CamSwitchController::BuildGoalPose_SimplePair() const
{
    Pose g = hold.pose;

    if (!pair.aValid) return g;

    Vector3 pivot = pair.aBase;
    _float yawDeg = CalcBehindYawDeg(pair.attacker);

    if (pair.vValid)
    {
        Vector3 dir = pair.vFace - pair.aFace;
        dir.y = 0.f;

        const _float len = dir.Length();
        if (len > 0.0001f)
        {
            dir /= len;

            yawDeg = Math::WrapDeg(YawFromDirDeg(dir) + tune.goal.pairYawAddDeg);

            pivot += dir * tune.goal.pivotForward;
            pivot.y = pair.aBase.y;
        }
    }

    g.pivotWorld = pivot;
    g.yawWorldDeg = yawDeg;

    _float add = tune.goal.distBaseAdd;

    if (pair.vValid)
    {
        Vector3 d = pair.vFace - pair.aFace;
        d.y = 0.f;
        const _float dist = d.Length();
        add += clamp(dist * max(tune.goal.distRatio, 0.f), 0.f, max(tune.goal.distMaxAdd, 0.f));
    }

    g.dist = hold.pose.dist + add;
    return g;
}