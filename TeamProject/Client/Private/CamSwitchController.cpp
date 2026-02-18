#include "pch.h"
#include "CamSwitchController.h"

#include "CamDirector.h"
#include "GameInstance.h"
#include "CharacterController.h"

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

    auto orbit = CamDirector()->GetOrbitCam();
    orbit->ParryMode_Begin();

    CaptureHoldPose();
    ApplyPose(hold.pose);
}

void CamSwitchController::End()
{
    if (!core.active) return;

    if (lens.fovAppliedOffset != 0.f) CameraManager()->SetFov(-lens.fovAppliedOffset, 0.f);
    lens.fovAppliedOffset = 0.f;

    auto orbit = CamDirector()->GetOrbitCam();
    orbit->ParryMode_ResumeSync();
    orbit->ParryMode_End();

    core.active = false;
    core.state = State::None;
    core.elapsed = 0.f;

    hold.pose = {};
    hold.valid = false;

    sw.target.Reset();
    sw.active = false;
    sw.from = {};
    sw.to = {};

    lens.fovSaved = 0.f;
}

void CamSwitchController::Switch(OBJECT_HANDLE target)
{
    if (!core.active) return;

    sw.target = target;
    sw.active = true;

    sw.from = CaptureCurPose();
    sw.to = BuildGoalPose(target);

    hold.pose = sw.from;
    hold.valid = true;

    core.state = State::Switching;
    core.elapsed = 0.f;
}

void CamSwitchController::Update(_float dt)
{
    if (!core.active) return;

    dt *= tune.common.dtMul;
    core.elapsed += dt;

    if (core.state == State::Enter)
    {
        const _float dur = max(tune.common.zoomInSec, 0.0001f);
        const _float u = clamp(core.elapsed / dur, 0.f, 1.f);
        const _float t = Math::ApplyEase(tune.common.zoomInEase, u);

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
        ApplyPose(hold.pose);

        const _float desiredOffset = -tune.common.zoomInDeg + EvalBreathingOffset(core.elapsed);
        ApplyFovOffset(desiredOffset);

        return;
    }

    if (core.state == State::Switching)
    {
        const _float poseDur = max(tune.sw.blendSec, 0.0001f);
        const _float uPose = clamp(core.elapsed / poseDur, 0.f, 1.f);
        const _float tPose = Math::ApplyEase(tune.sw.blendEase, uPose);

        const Pose p = LerpPose(sw.from, sw.to, tPose);
        ApplyPose(p);

        const _float desiredOffset = EvalSwitchFovOffset(core.elapsed);
        ApplyFovOffset(desiredOffset);

        if (uPose >= 1.f && IsSwitchFovDone(core.elapsed))
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

    hold.pose.pivotWorld = s.pose.pivotCurWorld;
    hold.pose.yawWorldDeg = s.pose.rotCurDeg.x;
    hold.pose.pitchDeg = s.pose.rotCurDeg.y;
    hold.pose.rollDeg = 0.f;
    hold.pose.dist = s.pose.distCur;

    hold.valid = true;
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

CamSwitchController::Pose CamSwitchController::BuildGoalPose(OBJECT_HANDLE target) const
{
    auto orbit = CamDirector()->GetOrbitCam();
    const _float offsetY = orbit->GetOffsetY();

    auto obj = ObjectManager()->Request_Object(target);
    const Vector3 pos = obj->Get_WorldPos();

    Vector3 pivot = pos + Vector3(0.f, offsetY, 0.f);

    auto cc = obj->Get_Component<CCharacterController>();
    if (cc)
    {
        const Vector3 foot = cc->Get_FootPosition();
        const _float half = cc->Get_HalfSize();
        pivot = foot + Vector3(0.f, half * tune.goal.pivotHalfMul + offsetY + tune.goal.pivotAddY, 0.f);
    }
    else
    {
        pivot.y += tune.goal.pivotAddY;
    }

    Pose g = hold.pose;
    g.pivotWorld = pivot;
    g.dist = hold.pose.dist + tune.goal.distDelta;

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

_float CamSwitchController::EvalBreathingOffset(_float tSec) const
{
    if (tune.hold.breathingAmpDeg == 0.f) return 0.f;

    const _float sec = max(tune.hold.breathingSec, 0.0001f);
    const _float phase = 2.f * XM_PI * (tSec / sec);
    return sinf(phase) * tune.hold.breathingAmpDeg;
}

_float CamSwitchController::EvalSwitchFovOffset(_float tSec) const
{
    const _float punchSec = max(tune.sw.punchSec, 0.f);
    const _float holdSec = max(tune.sw.fovHoldSec, 0.f);
    const _float recoverSec = max(tune.sw.fovRecoverSec, 0.0001f);

    const _float base = -tune.common.zoomInDeg;

    if (punchSec > 0.f && tSec < punchSec)
    {
        const _float u = clamp(tSec / punchSec, 0.f, 1.f);
        const _float t = Math::ApplyEase(tune.sw.punchEase, u);
        return Math::Lerp(base, base - tune.sw.punchDeg, t);
    }

    const _float tAfterPunch = max(0.f, tSec - punchSec);

    if (tAfterPunch < holdSec)
    {
        return base - tune.sw.punchDeg;
    }

    const _float tAfterHold = max(0.f, tAfterPunch - holdSec);
    const _float u = clamp(tAfterHold / recoverSec, 0.f, 1.f);
    const _float t = Math::ApplyEase(tune.sw.fovRecoverEase, u);

    return Math::Lerp(base - tune.sw.punchDeg, 0.f, t);
}


_bool CamSwitchController::IsSwitchFovDone(_float tSec) const
{
    const _float total = max(0.f, tune.sw.punchSec) + max(0.f, tune.sw.fovHoldSec) + max(tune.sw.fovRecoverSec, 0.f);
    return tSec >= total;
}