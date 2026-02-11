#include "pch.h"
#include "CamWipeOutController.h"
// Client
#include "CamDirector.h"
// Engine
#include "Helper_Func.h"
#include "GameInstance.h"
#include "CharacterController.h"

CamWipeOutController::PivotSample CamWipeOutController::SamplePivots(OBJECT_HANDLE h, _float offsetY, _float faceYOffsetMul)
{
    PivotSample s{};
    if (!h.isValid()) return s;

    auto obj = ObjectManager()->Request_Object(h);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    const Vector3 foot(foot4.x, foot4.y, foot4.z);

    const _float half = cc->Get_HalfSize();
    const Vector3 base = foot + Vector3(0.f, half * 1.5f + offsetY, 0.f);

    const _float faceY = half * 2.f * faceYOffsetMul;
    const Vector3 face = foot + Vector3(0.f, faceY + offsetY, 0.f);

    s.basePivot = base;
    s.facePivot = face;
    s.valid = true;
    return s;
}

Vector3 CamWipeOutController::ClampOffset(const Vector3& offset, _float maxLen)
{
    if (maxLen <= 0.f) return offset;

    const _float len = offset.Length();
    if (len <= maxLen) return offset;
    if (len <= 0.f) return Vector3::Zero;

    return offset * (maxLen / len);
}

Vector3 CamWipeOutController::RotateYDegXZ(const Vector3& dirXZ, _float deg)
{
    const float rad = XMConvertToRadians(deg);
    const float c = cosf(rad);
    const float s = sinf(rad);

    Vector3 d = dirXZ;
    d.y = 0.f;

    Vector3 out;
    out.x = d.x * c + d.z * s;
    out.y = 0.f;
    out.z = -d.x * s + d.z * c;

    if (out.LengthSquared() <= 1e-10f) return Vector3(0.f, 0.f, 1.f);
    out.Normalize();
    return out;
}

_float CamWipeOutController::YawFromDirXZ(const Vector3& dirXZ)
{
    Vector3 d = dirXZ;
    d.y = 0.f;
    if (d.LengthSquared() <= 1e-10f) return 0.f;
    d.Normalize();
    return XMConvertToDegrees(atan2f(d.x, d.z));
}

Quaternion CamWipeOutController::YawPitchQuatDeg(_float yawDeg, _float pitchDeg)
{
    const _float yawRad = XMConvertToRadians(yawDeg);
    const _float pitchRad = XMConvertToRadians(pitchDeg);
    return Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);
}

Vector3 CamWipeOutController::OrbitPos(const Vector3& pivotWorld, const Quaternion& q, _float dist)
{
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    return pivotWorld + backDir * dist;
}

void CamWipeOutController::ApplyGoalPose_Snap(const ShotGoal& g)
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    const Vector3 pivotWorld = m_aBase + g.pivotExt;

    const Quaternion q = YawPitchQuatDeg(g.yawDeg, g.pitchDeg);
    const Vector3 camPos = OrbitPos(pivotWorld, q, g.dist);

    orbit->SnapFromCamPose(camPos, q);
    cam->Set_FOV(g.fov);
}

void CamWipeOutController::Reset()
{
    m_active = false;
    m_state = State::None;

    m_elapsed = 0.f;
    m_shotElapsed = 0.f;

    m_attacker.Reset();
    m_victim.Reset();

    m_sideSign = 1;

    m_aBase = Vector3::Zero;
    m_aFace = Vector3::Zero;
    m_aValid = false;

    m_vBase = Vector3::Zero;
    m_vFace = Vector3::Zero;
    m_vValid = false;

    m_dirXZ = Vector3(0.f, 0.f, 1.f);
    m_sep = 0.f;

    m_shotFrom = {};
    m_shotTo = {};
    m_hasShotFrom = false;

    m_prevOrbitCaptured = false;
    m_prevOrbit = {};

    m_prevFovCaptured = false;
    m_prevFov = 0.f;
}


void CamWipeOutController::Begin(OBJECT_HANDLE victimHandle)
{
    Reset();

    auto attacker = CamDirector()->GetCurHandle();
    if (!attacker.isValid()) return;
    if (!victimHandle.isValid()) return;

    m_attacker = attacker;
    m_victim = victimHandle;

    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    const _float offsetY = orbit->GetOffsetY();

    const PivotSample a = SamplePivots(m_attacker, offsetY);
    const PivotSample v = SamplePivots(m_victim, offsetY);

    m_aValid = a.valid;
    if (m_aValid)
    {
        m_aBase = a.basePivot;
        m_aFace = a.facePivot;
    }

    m_vValid = v.valid;
    if (m_vValid)
    {
        m_vBase = v.basePivot;
        m_vFace = v.facePivot;
    }

    if (!m_aValid) return;

    const Vector3 aFace = m_aFace;
    const Vector3 vFace = m_vValid ? m_vFace : (m_aFace + Vector3(0.f, 0.f, 1.f));

    Vector3 dir = vFace - aFace;
    dir.y = 0.f;

    if (dir.LengthSquared() <= 1e-10f) dir = Vector3(0.f, 0.f, 1.f);
    else dir.Normalize();

    m_dirXZ = dir;

    const Vector3 delta = vFace - aFace;
    m_sep = Vector3(delta.x, 0.f, delta.z).Length();
    m_sep = clamp(m_sep, WipeTuning::kSepMin, WipeTuning::kSepMax);

    m_sideSign = ResolveSideSign();

    m_prevOrbitCaptured = true;
    orbit->CaptureSnapshot(m_prevOrbit);

    m_prevFovCaptured = true;
    m_prevFov = cam->Get_FOV();

    orbit->Lock_Input();
    orbit->ParryMode_Begin();
    orbit->DialogueMode_Begin();
    orbit->DialogueYaw_Clear();

    m_active = true;

    m_state = State::Shot1_Enter;
    BeginShot(BuildShot1(), WipeTuning::kEnterBlendShot1Sec, WipeTuning::kHoldShot1Sec, true);

    ApplyGoalPose_Snap(m_shotFrom);
}

void CamWipeOutController::End()
{
    if (!m_active) return;

    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    orbit->ParryMode_End();
    orbit->DialogueMode_End();
    orbit->Unlock_Input();
    orbit->DialogueYaw_Clear();

    if (m_prevOrbitCaptured)
        orbit->RestoreSnapshot(m_prevOrbit);

    if (m_prevFovCaptured)
        cam->Set_FOV(m_prevFov);

    Reset();
}

void CamWipeOutController::Update(_float dt)
{
    if (!m_active) return;

    if (!m_attacker.isValid() || !m_victim.isValid())
    {
        End();
        return;
    }

    m_elapsed += dt;
    m_shotElapsed += dt;

    if (m_state == State::Shot1_Enter)
    {
        const _float u = (m_enterSec > 0.f) ? clamp(m_shotElapsed / m_enterSec, 0.f, 1.f) : 1.f;
        const _float t = Math::ApplyEase(WipeTuning::kApproachEase, u);

        ApplyInterpolated(m_shotFrom, m_shotTo, t);

        if (u >= 1.f)
        {
            m_state = State::Shot1_Hold;
            m_shotElapsed = 0.f;
            m_holdSec = WipeTuning::kHoldShot1Sec;
        }
        return;
    }

    if (IsHoldState(m_state))
    {
        ApplyHold(m_shotTo);

        if (m_shotElapsed >= m_holdSec)
            Advance();
        return;
    }
}

_bool CamWipeOutController::IsHoldState(State s) const
{
    return (s == State::Shot1_Hold || s == State::Shot2_Hold || s == State::Shot3_Hold || s == State::Shot4_Hold);
}

_int CamWipeOutController::ResolveSideSign() const
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto tf = orbit->Get_Component<CTransform>();

    Vector3 camLook = tf->Dir(STATE::LOOK);
    camLook.y = 0.f;

    if (camLook.LengthSquared() <= 1e-10f) camLook = Vector3(0.f, 0.f, 1.f);
    camLook.Normalize();

    const Vector3 sideA(-m_dirXZ.z, 0.f, m_dirXZ.x);

    return (sideA.Dot(camLook) >= 0.f) ? 1 : -1;
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShotCommon(_float angleDeg, _float pitchDeg, _float dist, _float fov, _float pivotClamp, _float attackerBias) const
{
    ShotGoal g{};

    const Vector3 aFace = m_aFace;
    const Vector3 vFace = m_vValid ? m_vFace : (m_aFace + m_dirXZ);

    Vector3 mid = (aFace + vFace) * 0.5f;

    const Vector3 biasDir = (aFace - mid);
    mid = mid + biasDir * attackerBias;

    const Vector3 basePivot = m_aBase;

    Vector3 desiredExt = mid - basePivot;
    desiredExt = ClampOffset(desiredExt, pivotClamp);

    g.pivotExt = desiredExt;

    const Vector3 camDir = RotateYDegXZ(m_dirXZ, (_float)m_sideSign * angleDeg);

    const _float yaw = YawFromDirXZ(camDir) + (_float)m_sideSign * WipeTuning::kSideYawBiasDeg;

    g.yawDeg = yaw;
    g.pitchDeg = pitchDeg;
    g.dist = dist;
    g.fov = fov;
    g.yawWeight = 1.f;
    return g;
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot1() const
{
    return BuildShotCommon(
        WipeTuning::kAngleShot1Deg,
        WipeTuning::kPitchBaseDeg,
        WipeTuning::kDistMid,
        WipeTuning::kFovMid,
        WipeTuning::kPivotClampShot1,
        WipeTuning::kAttackerBiasShot1
    );
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot2() const
{
    return BuildShotCommon(
        WipeTuning::kAngleShot2Deg,
        WipeTuning::kPitchCloseDeg,
        WipeTuning::kDistClose,
        WipeTuning::kFovClose,
        WipeTuning::kPivotClampShot2,
        WipeTuning::kAttackerBiasShot2
    );
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot3() const
{
    return BuildShotCommon(
        WipeTuning::kAngleShot3Deg,
        WipeTuning::kPitchBaseDeg,
        WipeTuning::kDistMid,
        WipeTuning::kFovMid,
        WipeTuning::kPivotClampShot3,
        WipeTuning::kAttackerBiasShot3
    );
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot4() const
{
    return BuildShotCommon(
        WipeTuning::kAngleShot4Deg,
        WipeTuning::kPitchWideDeg,
        WipeTuning::kDistWide,
        WipeTuning::kFovWide,
        WipeTuning::kPivotClampShot4,
        WipeTuning::kAttackerBiasShot4
    );
}

void CamWipeOutController::BeginShot(const ShotGoal& to, _float enterSec, _float holdSec, _bool captureFrom)
{
    m_enterSec = max(enterSec, 0.f);
    m_holdSec = max(holdSec, 0.f);

    m_shotTo = to;

    if (captureFrom)
    {
        CaptureCurAsFrom();
        m_hasShotFrom = true;
    }

    m_holdFrom = m_shotTo;
    m_holdTo = m_shotTo;

    _float dolly = 0.f;
    if (m_state == State::Shot1_Enter || m_state == State::Shot1_Hold) dolly = WipeTuning::kHoldDollyShot1;
    if (m_state == State::Shot2_Snap || m_state == State::Shot2_Hold) dolly = WipeTuning::kHoldDollyShot2;
    if (m_state == State::Shot3_Snap || m_state == State::Shot3_Hold) dolly = WipeTuning::kHoldDollyShot3;
    if (m_state == State::Shot4_Snap || m_state == State::Shot4_Hold) dolly = WipeTuning::kHoldDollyShot4;

    m_holdTo.dist = max(0.f, m_holdTo.dist - dolly);

    if (m_state == State::Shot4_Snap || m_state == State::Shot4_Hold)
        m_holdTo.fov = max(1.f, m_holdTo.fov - WipeTuning::kHoldFovPunchShot4);

    m_shotElapsed = 0.f;
}


void CamWipeOutController::CaptureCurAsFrom()
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    OrbitSnapshot s{};
    orbit->CaptureSnapshot(s);

    ShotGoal from{};

    const Vector3 pivotWorld = s.pose.pivotCurWorld;
    from.pivotExt = pivotWorld - m_aBase;

    from.yawDeg = s.pose.rotCurDeg.x;
    from.pitchDeg = s.pose.rotCurDeg.y;
    from.dist = s.pose.distCur;

    from.fov = cam->Get_FOV();

    m_shotFrom = from;
}

void CamWipeOutController::ApplyInterpolated(const ShotGoal& a, const ShotGoal& b, _float t)
{
    ShotGoal g{};

    g.pivotExt = Vector3::Lerp(a.pivotExt, b.pivotExt, t);

    const _float yaw = a.yawDeg + Math::WrapDeg(b.yawDeg - a.yawDeg) * t;
    g.yawDeg = yaw;

    g.pitchDeg = Math::Lerp(a.pitchDeg, b.pitchDeg, t);
    g.dist = Math::Lerp(a.dist, b.dist, t);
    g.fov = Math::Lerp(a.fov, b.fov, t);

    g.yawWeight = 1.f;

    ApplyGoalPose_Snap(g);
}

void CamWipeOutController::ApplyHold(const ShotGoal& g)
{
    if (m_holdSec <= 0.f)
    {
        ApplyGoalPose_Snap(m_holdTo);
        return;
    }

    const _float u = clamp(m_shotElapsed / m_holdSec, 0.f, 1.f);

    EaseType ease = WipeTuning::kHoldEaseShot1_3;
    if (m_state == State::Shot4_Hold) ease = WipeTuning::kHoldEaseShot4;

    const _float t = Math::ApplyEase(ease, u);

    ShotGoal out{};
    out.pivotExt = m_holdFrom.pivotExt;
    out.yawDeg = m_holdFrom.yawDeg;
    out.pitchDeg = m_holdFrom.pitchDeg;

    out.dist = Math::Lerp(m_holdFrom.dist, m_holdTo.dist, t);
    out.fov = Math::Lerp(m_holdFrom.fov, m_holdTo.fov, t);

    out.yawWeight = 1.f;

    ApplyGoalPose_Snap(out);
}

void CamWipeOutController::SnapTo(const ShotGoal& g)
{
    ApplyGoalPose_Snap(g);
}

void CamWipeOutController::Advance()
{
    if (m_state == State::Shot1_Hold)
    {
        m_state = State::Shot2_Snap;
        BeginShot(BuildShot2(), WipeTuning::kSnapShotSec, WipeTuning::kHoldShot2Sec, false);
        SnapTo(m_shotTo);
        m_state = State::Shot2_Hold;
        m_shotElapsed = 0.f;
        return;
    }

    if (m_state == State::Shot2_Hold)
    {
        m_state = State::Shot3_Snap;
        BeginShot(BuildShot3(), WipeTuning::kSnapShotSec, WipeTuning::kHoldShot3Sec, false);
        SnapTo(m_shotTo);
        m_state = State::Shot3_Hold;
        m_shotElapsed = 0.f;
        return;
    }

    if (m_state == State::Shot3_Hold)
    {
        m_state = State::Shot4_Snap;
        BeginShot(BuildShot4(), WipeTuning::kSnapShotSec, WipeTuning::kHoldShot4Sec, false);
        SnapTo(m_shotTo);
        m_state = State::Shot4_Hold;
        m_shotElapsed = 0.f;
        return;
    }

    if (m_state == State::Shot4_Hold)
    {
        End();
        return;
    }
}