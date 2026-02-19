#include "pch.h"
#include "CamSwitchController.h"
#include "CamDirector.h"
// Engine
#include "GameInstance.h"
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
    CamSwitchController::Pose LerpPose(const CamSwitchController::Pose& a, const CamSwitchController::Pose& b, _float t)
    {
        CamSwitchController::Pose out{};

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
        auto anim = obj->Get_Component<CAnimator3D>();
        _float4x4 m{};
        anim->Get_BipWorld(&m);
        return Vector3(m._41, m._42, m._43) + Vector3(0.f, offsetY, 0.f);
    }
    _float YawFromDirDeg(const Vector3& dir)
    {
        Vector3 d = dir;
        d.y = 0.f;
        d.Normalize();
        return XMConvertToDegrees(atan2f(d.x, d.z));
    }
    _float RollPulse(_float u01)
    {
        return sinf(u01 * XM_PI);
    }
}

void CamSwitchController::Begin()
{
    if (core.active) return;

    core.active = true;
    core.state = State::Enter;
    core.elapsed = 0.f;

    sw.target.Reset();
    sw.active = false;

    lens.fovSaved = CameraManager()->GetFov();
    lens.fovAppliedOffset = 0.f;
    lens.recoverFromOffset = 0.f;

    hold.target = CamDirector()->GetCurHandle();

    auto orbit = CamDirector()->GetOrbitCam();
    orbit->SwitchMode_Begin();

    CaptureHoldPose();
    ApplyPose(hold.pose);
}

void CamSwitchController::End()
{
    if (!core.active) return;

    if (lens.fovAppliedOffset != 0.f) CameraManager()->SetFov(-lens.fovAppliedOffset, 0.f);
    lens.fovAppliedOffset = 0.f;

    auto orbit = CamDirector()->GetOrbitCam();
    orbit->SwitchMode_ResumeSync();
    orbit->SwitchMode_End();

    core.active = false;
    core.state = State::None;
    core.elapsed = 0.f;

    hold.target.Reset();
    hold.pose = {};
    hold.valid = false;

    sw.target.Reset();
    sw.active = false;
    sw.from = {};
    sw.switchTo = {};
    sw.goal = {};
    sw.recoverFrom = {};
    sw.recoverTo = {};

    lens.fovSaved = 0.f;
    lens.recoverFromOffset = 0.f;
}


void CamSwitchController::Switch()
{
    if (!core.active) return;

    OBJECT_HANDLE target = CamDirector()->GetCurHandle();

    sw.target = target;
    sw.active = true;

    sw.from = CaptureCurPose();
    sw.goal = BuildGoalPose(target);

    sw.switchTo = sw.goal;
    sw.switchTo.yawWorldDeg = sw.from.yawWorldDeg;
    sw.switchTo.pitchDeg = sw.from.pitchDeg;
    sw.switchTo.rollDeg = 0.f;

    core.state = State::Switching;
    core.elapsed = 0.f;
}

void CamSwitchController::Update(_float dt)
{
    if (!core.active) return;
    core.elapsed += dt;

    if (core.state == State::Enter)
    {
        const _float dur = max(tune.common.zoomInSec, 0.0001f);
        const _float u = clamp(core.elapsed / dur, 0.f, 1.f);
        const _float t = Math::ApplyEase(tune.common.zoomInEase, u);

        FollowHoldPivot();
        ApplyPose(hold.pose);

        const _float desiredOffset = Math::Lerp(0.f, -tune.common.zoomInDeg, t);
        ApplyFovOffset(desiredOffset);

        if (u >= 1.f)
        {
            core.state = State::Hold;
            core.elapsed = 0.f;
        }
        return;
    }

    if (core.state == State::Hold)
    {
        FollowHoldPivot();
        ApplyPose(hold.pose);

        ApplyFovOffset(-tune.common.zoomInDeg);
        return;
    }

    if (core.state == State::Switching)
    {
        const _float poseDur = max(tune.sw.blendSec, 0.0001f);
        const _float uPose = clamp(core.elapsed / poseDur, 0.f, 1.f);
        const _float tPose = Math::ApplyEase(tune.sw.blendEase, uPose);

        auto orbit = CamDirector()->GetOrbitCam();
        const _float offsetY = orbit->GetOffsetY() + tune.goal.pivotAddY;

        sw.goal.pivotWorld = LockPivotPos(sw.target, offsetY);
        sw.switchTo.pivotWorld = sw.goal.pivotWorld;
        sw.switchTo.dist = sw.goal.dist;

        const Pose p = LerpPose(sw.from, sw.switchTo, tPose);
        ApplyPose(p);

        ApplyFovOffset(-tune.common.zoomInDeg);

        if (uPose >= 1.f)
        {
            lens.recoverFromOffset = lens.fovAppliedOffset;

            sw.recoverFrom = p;
            sw.recoverTo = sw.goal;

            core.state = State::Recover;
            core.elapsed = 0.f;
            return;
        }
        return;
    }

    if (core.state == State::Recover)
    {
        auto orbit = CamDirector()->GetOrbitCam();
        const _float offsetY = orbit->GetOffsetY() + tune.goal.pivotAddY;

        sw.recoverTo = BuildGoalPose(sw.target);
        sw.recoverTo.pivotWorld = LockPivotPos(sw.target, offsetY);

        const _float poseDur = max(tune.sw.recoverPoseSec, 0.0001f);
        const _float uPose = clamp(core.elapsed / poseDur, 0.f, 1.f);
        const _float tPose = Math::ApplyEase(tune.sw.recoverPoseEase, uPose);

        Pose p = LerpPose(sw.recoverFrom, sw.recoverTo, tPose);
        p.pivotWorld = sw.recoverTo.pivotWorld;
        p.rollDeg += RollPulse(uPose) * tune.sw.rollPeakDeg;

        ApplyPose(p);

        const _float desiredOffset = EvalRecoverFovOffset(core.elapsed);
        ApplyFovOffset(desiredOffset);

        const _float fovDur = max(tune.sw.fovRecoverSec, 0.0001f);
        if (core.elapsed >= poseDur && core.elapsed >= fovDur)
        {
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
    hold.pose.rollDeg = 0.f;
    hold.pose.dist = s.pose.distCur;

    const _float offsetY = orbit->GetOffsetY() + tune.goal.pivotAddY;
    hold.pose.pivotWorld = LockPivotPos(hold.target, offsetY);

    hold.valid = true;
}

void CamSwitchController::FollowHoldPivot()
{
    auto orbit = CamDirector()->GetOrbitCam();
    const _float offsetY = orbit->GetOffsetY() + tune.goal.pivotAddY;
    hold.pose.pivotWorld = LockPivotPos(hold.target, offsetY);
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

CamSwitchController::Pose CamSwitchController::BuildGoalPose(OBJECT_HANDLE target) const
{
    auto orbit = CamDirector()->GetOrbitCam();
    const _float offsetY = orbit->GetOffsetY() + tune.goal.pivotAddY;

    Pose g = hold.pose;
    g.pivotWorld = LockPivotPos(target, offsetY);
    g.dist = hold.pose.dist + tune.goal.distDelta;
    g.yawWorldDeg = CalcBehindYawDeg(target);
    return g;
}

void CamSwitchController::ApplyPose(const Pose& p) const
{
    auto orbit = CamDirector()->GetOrbitCam();

    const Quaternion qPos = YawPitchRollQuatDeg(p.yawWorldDeg, p.pitchDeg, 0.f);
    const Vector3 camPos = OrbitPos(p.pivotWorld, qPos, p.dist);

    const Quaternion qRot = YawPitchRollQuatDeg(p.yawWorldDeg, p.pitchDeg, p.rollDeg);
    orbit->SnapFromOrbitPose(p.pivotWorld, camPos, qRot, p.dist);
}

void CamSwitchController::ApplyFovOffset(_float desiredOffset)
{
    const _float delta = desiredOffset - lens.fovAppliedOffset;
    if (delta != 0.f) CameraManager()->SetFov(delta, 0.f);
    lens.fovAppliedOffset = desiredOffset;
}

_float CamSwitchController::EvalRecoverFovOffset(_float tSec) const
{
    const _float dur = max(tune.sw.fovRecoverSec, 0.0001f);
    const _float u = clamp(tSec / dur, 0.f, 1.f);
    const _float t = Math::ApplyEase(tune.sw.fovRecoverEase, u);

    return Math::Lerp(lens.recoverFromOffset, 0.f, t);
}