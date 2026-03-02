#include "pch.h"
#include "CamSwitchController.h"

#include "CamDirector.h"
#include "CharacterController.h"

namespace
{
    Quaternion YawPitchQuatDeg(_float yawDeg, _float pitchDeg)
    {
        return Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(yawDeg), XMConvertToRadians(pitchDeg), 0.f);
    }

    Vector3 OrbitPos(const Vector3& pivotWorld, const Quaternion& q, _float dist)
    {
        const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
        return pivotWorld + backDir * dist;
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

    void RotDegFromLookDir(const Vector3& lookDir, _float& outYawDeg, _float& outPitchDeg)
    {
        const _float yawRad = atan2f(lookDir.x, lookDir.z);
        const _float pitchRad = asinf(clamp(-lookDir.y, -1.f, 1.f));

        outYawDeg = XMConvertToDegrees(yawRad);
        outPitchDeg = XMConvertToDegrees(pitchRad);
    }

    void PivotStab_ApplyTuning(CamSwitchController::PivotStab& s, const CamSwitchController::SwitchTuning::PivotFilter& t)
    {
        s.velTau = t.velTau;
        s.rawTau = t.rawTau;
        s.outlierDist = t.outlierDist;
        s.outlierVel = t.outlierVel;
    }

    void PivotStab_Reset(CamSwitchController::PivotStab& s, const Vector3& pivot)
    {
        s.hasLast = true;
        s.lastRawPivot = pivot;
        s.filteredPivot = pivot;
        s.filteredVel = Vector3::Zero;
    }

    Vector3 PivotStab_Eval(CamSwitchController::PivotStab& s, _float dt, const Vector3& rawPivot)
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

    Vector3 SafeFlatDirOrDefault(const Vector3& dir, const Vector3& fallback)
    {
        Vector3 out = dir;
        out.y = 0.f;

        if (out.Length() > 0.0001f)
        {
            out.Normalize();
            return out;
        }

        Vector3 fb = fallback;
        fb.y = 0.f;

        if (fb.Length() > 0.0001f)
        {
            fb.Normalize();
            return fb;
        }

        return Vector3(0.f, 0.f, 1.f);
    }

    Vector3 PoseToCamPos(const CamSwitchController::Pose& p)
    {
        const Quaternion q = YawPitchQuatDeg(p.yawWorldDeg, p.pitchDeg);
        return OrbitPos(p.pivotWorld, q, p.dist);
    }

    struct SwitchGoalPresetOffsets
    {
        _float lookOffset = 0.f;
        _float rightOffset = 0.f;
        _float upOffset = 0.f;
    };

    struct CharacterSwitchPresetRow
    {
        CHARACTER character = CHARACTER::END;
        SwitchGoalPresetOffsets offsets{};
    };

    SwitchGoalPresetOffsets MakeDefaultSwitchGoalPreset(const CamSwitchController::SwitchTuning::SwitchGoal& goal)
    {
        SwitchGoalPresetOffsets out{};
        out.lookOffset = goal.lookOffset;
        out.rightOffset = goal.rightOffset;
        out.upOffset = goal.upOffset;
        return out;
    }

    SwitchGoalPresetOffsets ResolveSwitchGoalPresetByCharacter(CHARACTER character, const CamSwitchController::SwitchTuning::SwitchGoal& goal)
    {
        // look / Right / Up
        const CharacterSwitchPresetRow presetTable[] =
        {
            {CHARACTER::Corin,   {-2.60f, 0.35f, -0.45f}},
            {CHARACTER::Miyabi,  {-3.50f, 0.65f, -0.55f}},
            {CHARACTER::JaneDoe, {-2.60f, 0.20f, -0.40f}},
        };

        SwitchGoalPresetOffsets out = MakeDefaultSwitchGoalPreset(goal);

        for (const auto& row : presetTable)
            if (row.character == character)
                return row.offsets;

        return out;
    }

    SwitchGoalPresetOffsets ResolveSwitchGoalPresetByHandle(OBJECT_HANDLE handle, const CamSwitchController::SwitchTuning::SwitchGoal& goal)
    {
        auto character = static_cast<CCharacter*>(handle.Get());
        return ResolveSwitchGoalPresetByCharacter(character->Get_CharacterName(), goal);
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
    lens.holdDesiredFov = tune.common.holdFov;
    lens.fovCommanded = lens.fovSaved;
    lens.hasFovCommanded = true;

    hold = {};
    hold.target = CamDirector()->GetCurHandle();

    beginOrbit = {};
    CaptureBeginOrbitBaseline();

    pair = {};
    pair.attacker = hold.target;
    pair.victim = CamDirector()->GetCurTarget();

    sw = {};
    PivotStab_ApplyTuning(sw.pivotStab, tune.filter);

    hold.pivotStab = {};
    PivotStab_ApplyTuning(hold.pivotStab, tune.filter);

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

    const _float curFov = lens.hasFovCommanded ? lens.fovCommanded : CameraManager()->GetFov();
    const _float delta = lens.fovSaved - curFov;
    if (delta != 0.f) CameraManager()->SetFov(delta, 0.f);

    lens.fovCommanded = lens.fovSaved;
    lens.hasFovCommanded = true;

    auto orbit = CamDirector()->GetOrbitCam();
    orbit->SwitchMode_End();
    orbit->ResumeSync();
    orbit->Lock_ReenterBlend(1.f, EaseType::InOutSine);
    orbit->FreezeFor(0.016f);

    core.active = false;
    core.state = State::None;
    core.elapsed = 0.f;

    hold = {};
    pair = {};
    sw = {};
    cancel = {};
    lens = {};
    beginOrbit = {};
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
    if (cur == hold.target)
    {
        BeginCancelRecover();
        return;
    }

    BeginSwitchTo(cur);
}

void CamSwitchController::BeginCancelRecover()
{
    cancel.fovFrom = lens.hasFovCommanded ? lens.fovCommanded : CameraManager()->GetFov();
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

    sw.fromPose = CaptureCurPose();
    sw.fromCamPos = PoseToCamPos(sw.fromPose);
    sw.fromPivot = sw.fromPose.pivotWorld;

    sw.fovFrom = lens.hasFovCommanded ? lens.fovCommanded : CameraManager()->GetFov();

    const _float holdFov = lens.holdDesiredFov;
    const _float savedFov = lens.fovSaved;
    const _float switchRecoverCap = tune.sw.fovSwitchRecoverTarget;

    if (savedFov >= holdFov)
        sw.fovTo = min(savedFov, max(switchRecoverCap, holdFov));
    else
        sw.fovTo = max(savedFov, min(switchRecoverCap, holdFov));

    pair = {};
    pair.attacker = newTarget;
    pair.victim = CamDirector()->GetCurTarget();

    UpdatePairPivots(0.f);

    sw.sideSign = ChooseSwitchSideSign();
    sw.switchCamPosGoal = BuildSwitchCamPosGoal_PlayerPreset(sw.sideSign);
    sw.switchPivotGoal = BuildSwitchPivotGoal_EnemyCenter();

    PivotStab_Reset(sw.pivotStab, sw.switchPivotGoal);

    sw.recoverCamPosFrom = Vector3::Zero;
    sw.recoverPivotFrom = Vector3::Zero;
    sw.recoverTo = {};
    sw.recoverLookBasis = {};
    sw.hasRecoverLookBasis = false;

    sw.recoverFovFrom = 0.f;
    sw.recoverFovTo = 0.f;

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
        const _float dur = max(tune.common.enterSec, 0.0001f);
        const _float u = clamp(core.elapsed / dur, 0.f, 1.f);
        const _float t = Math::ApplyEase(tune.common.enterEase, u);

        FollowHoldPivot(dt);
        ApplyPose(hold.pose);

        const _float desiredFov = Math::Lerp(lens.fovFrom, lens.holdDesiredFov, t);
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

        const _float camDur = max(tune.sw.camPosBlendSec, 0.0001f);
        const _float pivotDur = max(tune.sw.pivotBlendSec, 0.0001f);

        const _float uCam = clamp(core.elapsed / camDur, 0.f, 1.f);
        const _float uPivot = clamp(core.elapsed / pivotDur, 0.f, 1.f);

        const _float tCam = Math::ApplyEase(tune.sw.camPosBlendEase, uCam);
        const _float tPivot = Math::ApplyEase(tune.sw.pivotBlendEase, uPivot);

        Vector3 rawPivotGoal = BuildSwitchPivotGoal_EnemyCenter();
        Vector3 filteredPivotGoal = PivotStab_Eval(sw.pivotStab, dt, rawPivotGoal);

        sw.switchPivotGoal = filteredPivotGoal;

        const Vector3 curCamPos = Vector3::Lerp(sw.fromCamPos, sw.switchCamPosGoal, tCam);
        const Vector3 curPivot = Vector3::Lerp(sw.fromPivot, sw.switchPivotGoal, tPivot);

        const Pose p = BuildPoseFromPivotAndCamPos(curPivot, curCamPos, sw.fromPose);
        ApplyPose(p);

        const _float desiredFov = EvalSwitchFov(core.elapsed);
        ApplyFovTarget(desiredFov);

        const _float fovDur = max(tune.sw.fovBlendSec, 0.0001f);
        const _float uFov = clamp(core.elapsed / fovDur, 0.f, 1.f);

        if (uCam >= 1.f && uPivot >= 1.f && uFov >= 1.f)
        {
            sw.recoverCamPosFrom = curCamPos;
            sw.recoverPivotFrom = curPivot;

            sw.recoverLookBasis = p;
            sw.hasRecoverLookBasis = true;

            sw.recoverTo = BuildRecoverPose_PlayerCenter();

            PivotStab_Reset(sw.pivotStab, sw.recoverTo.pivotWorld);

            sw.recoverFovFrom = lens.hasFovCommanded ? lens.fovCommanded : CameraManager()->GetFov();
            sw.recoverFovTo = lens.fovSaved;

            core.state = State::Recover;
            core.elapsed = 0.f;
            return;
        }
        return;
    }

    if (core.state == State::Recover)
    {
        Pose recoverGoal = BuildRecoverPose_PlayerCenter();

        const Vector3 rawPivot = recoverGoal.pivotWorld;
        const Vector3 filteredPivot = PivotStab_Eval(sw.pivotStab, dt, rawPivot);
        recoverGoal.pivotWorld = filteredPivot;

        sw.recoverTo = recoverGoal;

        const _float poseDur = max(tune.sw.recoverPoseSec, 0.0001f);
        const _float uPose = clamp(core.elapsed / poseDur, 0.f, 1.f);
        const _float tPose = Math::ApplyEase(tune.sw.recoverPoseEase, uPose);

        Vector3 recoverCamPosGoal = PoseToCamPos(sw.recoverTo);
        recoverCamPosGoal.y += tune.sw.recoverCamPosAddY;

        const Vector3 curPivot = Vector3::Lerp(sw.recoverPivotFrom, sw.recoverTo.pivotWorld, tPose);
        const Vector3 curCamPos = Vector3::Lerp(sw.recoverCamPosFrom, recoverCamPosGoal, tPose);

        const Pose p = BuildPoseFromPivotAndCamPos(curPivot, curCamPos, sw.recoverTo);
        ApplyPose(p);

        const _float desiredFov = EvalRecoverFov(core.elapsed);
        ApplyFovTarget(desiredFov);

        const _float fovDur = max(tune.sw.recoverFovSec, 0.0001f);
        const _float uFov = clamp(core.elapsed / fovDur, 0.f, 1.f);

        if (uPose >= 1.f && uFov >= 1.f)
        {
            Vector3 finalCamPos = PoseToCamPos(sw.recoverTo);
            finalCamPos.y += tune.sw.recoverCamPosAddY;

            const Pose finalPose = BuildPoseFromPivotAndCamPos(sw.recoverTo.pivotWorld, finalCamPos, sw.recoverTo);

            ApplyPose(finalPose);
            ApplyFovTarget(lens.fovSaved);
            End();
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
    hold.pose.dist = s.pose.distCur;
    hold.pose.pivotWorld = CalcCenterPivot(hold.target);
}

void CamSwitchController::FollowHoldPivot(_float dt)
{
    const Vector3 raw = CalcCenterPivot(hold.target);
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

Vector3 CamSwitchController::CalcCenterPivot(OBJECT_HANDLE h) const
{
    auto obj = ObjectManager()->Request_Object(h);
    if (!obj) return Vector3{};
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector3 foot = cc->Get_FootPosition();
    const _float halfSize = cc->Get_HalfSize();

    return foot + Vector3(0.f, halfSize * 1.5f, 0.f);
}

void CamSwitchController::UpdatePairPivots(_float dt)
{
    if (!pair.attacker.isValid())
    {
        pair = {};
        return;
    }

    const Vector3 attackerCenter = CalcCenterPivot(pair.attacker);

    if (!pair.aValid || dt <= 0.f)
    {
        pair.aCenter = attackerCenter;
        pair.aValid = true;
    }
    else
    {
        const _float t = clamp(dt * tune.common.pivotFollowLerpSpeed, 0.f, 1.f);
        pair.aCenter = Vector3::Lerp(pair.aCenter, attackerCenter, t);
        pair.aValid = true;
    }

    if (!pair.victim.isValid())
    {
        pair.vCenter = Vector3::Zero;
        pair.vValid = false;
        return;
    }

    const Vector3 victimCenter = CalcCenterPivot(pair.victim);

    Vector3 delta = victimCenter - pair.aCenter;
    delta.y = 0.f;
    const _float dist = delta.Length();

    if (dist > tune.common.maxVictimDist)
    {
        pair.vCenter = Vector3::Zero;
        pair.vValid = false;
        return;
    }

    if (!pair.vValid || dt <= 0.f)
    {
        pair.vCenter = victimCenter;
        pair.vValid = true;
        return;
    }

    const _float t = clamp(dt * tune.common.pivotFollowLerpSpeed, 0.f, 1.f);
    pair.vCenter = Vector3::Lerp(pair.vCenter, victimCenter, t);
    pair.vValid = true;
}

Vector3 CamSwitchController::BuildSwitchPivotGoal_EnemyCenter() const
{
    if (pair.vValid) return pair.vCenter;
    if (pair.aValid) return pair.aCenter;
    return hold.pose.pivotWorld;
}

Vector3 CamSwitchController::BuildSwitchCamPosGoal_PlayerPreset(_int sideSign) const
{
    const Pose fallbackPose = sw.active ? sw.fromPose : hold.pose;
    Vector3 fallbackCamPos = PoseToCamPos(fallbackPose);

    if (!pair.aValid) return fallbackCamPos;

    auto obj = ObjectManager()->Request_Object(pair.attacker);
    auto tf = obj->Get_Component<CTransform>();

    const SwitchGoalPresetOffsets preset = ResolveSwitchGoalPresetByHandle(pair.attacker, tune.goal);

    Vector3 forward = SafeFlatDirOrDefault(tf->Dir(STATE::LOOK), Vector3(0.f, 0.f, 1.f));
    Vector3 right(forward.z, 0.f, -forward.x);
    if (right.Length() > 0.f) right.Normalize();

    const _float sign = sideSign < 0 ? -1.f : 1.f;

    Vector3 camPos = pair.aCenter;
    camPos += forward * preset.lookOffset;
    camPos += right * (preset.rightOffset * sign);
    camPos += Vector3(0.f, preset.upOffset, 0.f);

    if (pair.vValid)
    {
        Vector3 delta = pair.vCenter - pair.aCenter;
        delta.y = 0.f;
        const _float pairDist = delta.Length();

        _float distAdd = tune.goal.distBaseAdd;
        distAdd += clamp(pairDist * max(tune.goal.distRatio, 0.f), 0.f, max(tune.goal.distMaxAdd, 0.f));

        if (distAdd != 0.f)
        {
            Vector3 toCam = camPos - pair.aCenter;
            if (toCam.Length() > 0.0001f)
            {
                toCam.Normalize();
                camPos += toCam * distAdd;
            }
        }
    }

    return camPos;
}

_int CamSwitchController::ChooseSwitchSideSign() const
{
    if (!tune.goal.chooseNearerSidePreset) return 1;
    if (tune.goal.rightOffset <= 0.f) return 1;
    if (!pair.aValid) return 1;

    const Vector3 refCamPos = PoseToCamPos(sw.active ? sw.fromPose : hold.pose);

    Vector3 leftPos = BuildSwitchCamPosGoal_PlayerPreset(-1);
    Vector3 rightPos = BuildSwitchCamPosGoal_PlayerPreset(+1);

    leftPos.y = 0.f;
    rightPos.y = 0.f;

    Vector3 refXZ = refCamPos;
    refXZ.y = 0.f;

    const _float dL = (leftPos - refXZ).LengthSquared();
    const _float dR = (rightPos - refXZ).LengthSquared();

    return dL <= dR ? -1 : +1;
}

CamSwitchController::Pose CamSwitchController::BuildRecoverPose_PlayerCenter() const
{
    Pose out = sw.hasRecoverLookBasis ? sw.recoverLookBasis : (beginOrbit.valid ? beginOrbit.pose : hold.pose);

    const OBJECT_HANDLE target = sw.target.isValid() ? sw.target : hold.target;
    if (!target.isValid()) return out;

    out.pivotWorld = CalcCenterPivot(target);

    if (beginOrbit.valid) out.dist = beginOrbit.pose.dist;

    return out;
}

CamSwitchController::Pose CamSwitchController::BuildPoseFromPivotAndCamPos(const Vector3& pivotWorld, const Vector3& camPos, const Pose& fallback) const
{
    Pose out = fallback;
    out.pivotWorld = pivotWorld;

    Vector3 toPivot = pivotWorld - camPos;
    const _float dist = toPivot.Length();

    if (dist <= 0.0001f)
    {
        out.dist = fallback.dist;
        return out;
    }

    const Vector3 lookDir = toPivot / dist;

    _float yawDeg = fallback.yawWorldDeg;
    _float pitchDeg = fallback.pitchDeg;
    RotDegFromLookDir(lookDir, yawDeg, pitchDeg);

    out.yawWorldDeg = yawDeg;
    out.pitchDeg = pitchDeg;
    out.dist = dist;

    return out;
}

void CamSwitchController::ApplyPose(const Pose& p) const
{
    auto orbit = CamDirector()->GetOrbitCam();

    const Quaternion q = YawPitchQuatDeg(p.yawWorldDeg, p.pitchDeg);
    const Vector3 camPos = OrbitPos(p.pivotWorld, q, p.dist);

    orbit->SnapFromExternalPose(p.pivotWorld, camPos, q, p.dist);
}

void CamSwitchController::ApplyFovTarget(_float desiredFov)
{
    const _float curFov = lens.hasFovCommanded ? lens.fovCommanded : CameraManager()->GetFov();
    const _float delta = desiredFov - curFov;

    if (delta != 0.f) CameraManager()->SetFov(delta, 0.f);

    lens.fovCommanded = desiredFov;
    lens.hasFovCommanded = true;
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

_float CamSwitchController::EvalRecoverFov(_float tSec) const
{
    const _float dur = max(tune.sw.recoverFovSec, 0.0001f);
    const _float u = clamp(tSec / dur, 0.f, 1.f);
    const _float t = Math::ApplyEase(tune.sw.recoverFovEase, u);

    return Math::Lerp(sw.recoverFovFrom, sw.recoverFovTo, t);
}

void CamSwitchController::CaptureBeginOrbitBaseline()
{
    beginOrbit = {};
    beginOrbit.pose = CaptureCurPose();
    beginOrbit.valid = true;
}