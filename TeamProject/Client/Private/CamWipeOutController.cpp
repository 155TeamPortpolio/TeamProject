#include "pch.h"
#include "CamWipeOutController.h"

#include "CamDirector.h"

#include "Helper_Func.h"
#include "GameInstance.h"
#include "CharacterController.h"

NS_BEGIN(Client)

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
    const _float pitchRad = XMConvertToRadians(-pitchDeg);
    return Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);
}

Vector3 CamWipeOutController::OrbitPos(const Vector3& pivotWorld, const Quaternion& q, _float dist)
{
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    return pivotWorld + backDir * dist;
}

Vector3 CamWipeOutController::BasePivotWorld(_float baseVictimWeight) const
{
    if (!m_vValid) return m_aBase;
    return Vector3::Lerp(m_aBase, m_vBase, clamp(baseVictimWeight, 0.f, 1.f));
}

void CamWipeOutController::ApplyGoalPose_Snap(const ShotGoal& g)
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);
    const Vector3 pivotWorld = basePivot + g.pivotExt;

    const _float baseYaw = CurCamYawDeg();
    const _float yaw = baseYaw + Math::WrapDeg(g.yawDeg - baseYaw) * g.yawWeight;

    const Quaternion q = YawPitchQuatDeg(yaw, g.pitchDeg);
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

    m_enterSec = 0.f;
    m_holdSec = 0.f;

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

    m_holdFrom = {};
    m_holdTo = {};

    m_shot4PivotFixed = Vector3::Zero;
    m_shot4RotFixed = Quaternion::Identity;
    m_shot4DistFrom = 0.f;
    m_shot4DistTo = 0.f;
    m_shot4FovFrom = 0.f;
    m_shot4FovTo = 0.f;
    m_shot4RailActive = false;
}

void CamWipeOutController::Begin()
{
    Reset();

    m_attacker = CamDirector()->GetCurHandle();
    m_victim = CamDirector()->GetCurTarget();

    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    const _float offsetY = orbit->GetOffsetY();

    const PivotSample a = SamplePivots(m_attacker, offsetY);
    m_aValid = a.valid;
    m_aBase = a.basePivot;
    m_aFace = a.facePivot;

    const PivotSample v = SamplePivots(m_victim, offsetY);
    if (v.valid)
    {
        m_vBase = v.basePivot;
        m_vFace = v.facePivot;
        m_vValid = true;
    }

    auto attackerObj = ObjectManager()->Request_Object(m_attacker);
    auto attackerTf = attackerObj->Get_Component<CTransform>();

    Vector3 attackerFwd = attackerTf->Dir(STATE::LOOK);
    attackerFwd.y = 0.f;
    if (attackerFwd.LengthSquared() <= 1e-10f) attackerFwd = Vector3(0.f, 0.f, 1.f);
    attackerFwd.Normalize();

    const Vector3 aFace = m_aFace;
    const Vector3 vFace = m_vValid ? m_vFace : (m_aFace + attackerFwd);

    Vector3 dir = vFace - aFace;
    dir.y = 0.f;
    if (dir.LengthSquared() <= 1e-10f) dir = attackerFwd;
    else dir.Normalize();

    m_dirXZ = dir;

    const Vector3 delta = vFace - aFace;
    m_sep = Vector3(delta.x, 0.f, delta.z).Length();
    m_sep = clamp(m_sep, tune.sepMin, tune.sepMax);

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
    BeginShot(BuildShot1(), tune.enterBlendShot1Sec, tune.holdShot1Sec, true);

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

    if (m_prevOrbitCaptured) orbit->RestoreSnapshot(m_prevOrbit);
    if (m_prevFovCaptured) cam->Set_FOV(m_prevFov);

    Reset();
}

void CamWipeOutController::Update(_float dt)
{
    if (!m_active) return;

    m_elapsed += dt;
    m_shotElapsed += dt;

    UpdatePivots(dt);

    if (m_state == State::Shot1_Enter)
    {
        const _float u = (m_enterSec > 0.f) ? clamp(m_shotElapsed / m_enterSec, 0.f, 1.f) : 1.f;
        const _float t = Math::ApplyEase(tune.approachEase, u);

        ApplyInterpolated(m_shotFrom, m_shotTo, t);

        if (u >= 1.f)
        {
            m_state = State::Shot1_Hold;
            m_shotElapsed = 0.f;
            return;
        }
        return;
    }

    if (IsHoldState(m_state))
    {
        ApplyHold();

        if (m_holdSec > 0.f && m_shotElapsed >= m_holdSec) Advance();
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

    const Vector3 baseLook(-m_dirXZ.x, 0.f, -m_dirXZ.z);
    const Vector3 sideA(-baseLook.z, 0.f, baseLook.x);

    return (sideA.Dot(camLook) >= 0.f) ? 1 : -1;
}

_float CamWipeOutController::CurCamYawDeg() const
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto tf = orbit->Get_Component<CTransform>();

    Vector3 camLook = tf->Dir(STATE::LOOK);
    camLook.y = 0.f;
    if (camLook.LengthSquared() <= 1e-10f) camLook = Vector3(0.f, 0.f, 1.f);
    camLook.Normalize();

    return YawFromDirXZ(camLook);
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShotCommon(_int sideSign, _float angleDeg, _float pitchDeg, _float dist, _float fov, _float pivotClamp, _float attackerBias, _float baseVictimWeight) const
{
    ShotGoal g{};

    auto attackerObj = ObjectManager()->Request_Object(m_attacker);
    auto attackerTf = attackerObj->Get_Component<CTransform>();

    Vector3 attackerFwd = attackerTf->Dir(STATE::LOOK);
    attackerFwd.y = 0.f;
    if (attackerFwd.LengthSquared() <= 1e-10f) attackerFwd = Vector3(0.f, 0.f, 1.f);
    attackerFwd.Normalize();

    const Vector3 aFace = m_aFace;
    const Vector3 vFace = m_vValid ? m_vFace : (m_aFace + attackerFwd);

    Vector3 mid = (aFace + vFace) * 0.5f;

    _float sep01 = 0.f;
    if (tune.sepMax > tune.sepMin) sep01 = (m_sep - tune.sepMin) / (tune.sepMax - tune.sepMin);
    sep01 = clamp(sep01, 0.f, 1.f);

    const _float bias = attackerBias * (1.f - sep01);

    const Vector3 biasDir = (aFace - mid);
    mid = mid + biasDir * bias;

    const _float aimY = Math::Lerp(m_aBase.y, m_aFace.y, tune.pelvisMul);
    mid.y = aimY;

    _float w = clamp(baseVictimWeight, 0.f, 1.f);
    if (!m_vValid) w = 0.f;

    const Vector3 basePivot = BasePivotWorld(w);

    Vector3 desired = mid - basePivot;

    const _float clampLen = max(pivotClamp, m_sep * 0.75f);

    Vector3 desiredXZ(desired.x, 0.f, desired.z);
    desiredXZ = ClampOffset(desiredXZ, clampLen);

    desired.x = desiredXZ.x;
    desired.z = desiredXZ.z;

    g.pivotExt = desired;

    const Vector3 baseLook(-m_dirXZ.x, 0.f, -m_dirXZ.z);
    const Vector3 camDir = RotateYDegXZ(baseLook, (_float)sideSign * angleDeg);

    g.yawDeg = YawFromDirXZ(camDir) + (_float)sideSign * tune.sideYawBiasDeg;

    g.pitchDeg = pitchDeg;
    g.dist = dist;
    g.fov = fov;
    g.yawWeight = 1.f;
    g.baseVictimWeight = w;
    return g;
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot1() const
{
    ShotGoal g = BuildShotCommon(m_sideSign, tune.angleShot1Deg, tune.pitchShot1UpDeg, tune.distClose, tune.fovClose, tune.pivotClampShot1, tune.attackerBiasShot1, 0.f);

    g.yawDeg = g.yawDeg + (_float)m_sideSign * tune.shot1YawDeltaDeg;
    g.yawWeight = tune.shot1YawWeight;

    return g;
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot2() const
{
    return BuildShotCommon(-m_sideSign, tune.angleShot2Deg, tune.pitchShot2HighDeg, tune.distShot2Far, tune.fovShot2, tune.pivotClampShot2, tune.attackerBiasShot2, tune.baseVictimWeightShot2);
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot3() const
{
    return BuildShotCommon(m_sideSign, tune.angleShot3Deg, tune.pitchBaseDeg, tune.distMid, tune.fovMid, tune.pivotClampShot3, tune.attackerBiasShot3, tune.baseVictimWeightShot3);
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot4() const
{
    ShotGoal g = BuildShotCommon(m_sideSign, tune.angleShot4Deg, tune.pitchShot4LevelDeg, tune.distShot4Far, tune.fovShot4Far, tune.pivotClampShot4, tune.attackerBiasShot4, tune.baseVictimWeightShot4);

    g.baseVictimWeight = 0.f;

    Vector3 pivotWorld = m_aFace;

    const _float aimY = Math::Lerp(m_aBase.y, m_aFace.y, tune.pelvisMul);
    pivotWorld.y = aimY;

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);

    Vector3 ext = pivotWorld - basePivot;

    Vector3 extXZ(ext.x, 0.f, ext.z);
    extXZ = ClampOffset(extXZ, tune.pivotClampShot4);

    ext.x = extXZ.x;
    ext.z = extXZ.z;

    g.pivotExt = ext;

    return g;
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
    if (m_state == State::Shot1_Enter || m_state == State::Shot1_Hold) dolly = tune.holdDollyShot1;
    if (m_state == State::Shot2_Snap || m_state == State::Shot2_Hold) dolly = tune.holdDollyShot2;
    if (m_state == State::Shot3_Snap || m_state == State::Shot3_Hold) dolly = tune.holdDollyShot3;
    if (m_state == State::Shot4_Snap || m_state == State::Shot4_Hold) dolly = tune.holdDollyShot4;

    m_holdTo.dist = max(0.f, m_holdTo.dist - dolly);

    if (m_state == State::Shot4_Snap || m_state == State::Shot4_Hold)
        m_holdTo.fov = max(1.f, m_holdTo.fov - tune.holdFovPunchShot4);

    m_shot4RailActive = false;

    m_shotElapsed = 0.f;
}

void CamWipeOutController::CaptureCurAsFrom()
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    OrbitSnapshot s{};
    orbit->CaptureSnapshot(s);

    ShotGoal from{};

    from.baseVictimWeight = 0.f;

    const Vector3 basePivot = BasePivotWorld(from.baseVictimWeight);
    const Vector3 pivotWorld = s.pose.pivotCurWorld;

    from.pivotExt = pivotWorld - basePivot;

    from.yawDeg = s.pose.rotCurDeg.x;
    from.pitchDeg = s.pose.rotCurDeg.y;
    from.dist = s.pose.distCur;

    from.fov = cam->Get_FOV();
    from.yawWeight = 1.f;

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

    g.yawWeight = Math::Lerp(a.yawWeight, b.yawWeight, t);

    g.baseVictimWeight = a.baseVictimWeight;

    ApplyGoalPose_Snap(g);
}

void CamWipeOutController::ApplyHold()
{
    if (m_holdSec <= 0.f)
    {
        ApplyGoalPose_Snap(m_holdTo);
        return;
    }

    const _float u = clamp(m_shotElapsed / m_holdSec, 0.f, 1.f);

    EaseType ease = tune.holdEaseShot1_3;
    if (m_state == State::Shot4_Hold) ease = tune.holdEaseShot4;

    const _float t = Math::ApplyEase(ease, u);

    if (m_state == State::Shot4_Hold && m_shot4RailActive)
    {
        const _float dist = Math::Lerp(m_shot4DistFrom, m_shot4DistTo, t);
        const Vector3 pos = OrbitPos(m_shot4PivotFixed, m_shot4RotFixed, dist);

        auto orbit = CamDirector()->GetOrbitCam();
        auto cam = CamDirector()->GetOrbitCamComp();

        orbit->SnapFromCamPose(pos, m_shot4RotFixed);

        const _float fov = Math::Lerp(m_shot4FovFrom, m_shot4FovTo, t);
        cam->Set_FOV(fov);

        return;
    }

    ShotGoal out{};

    out.pivotExt = m_holdFrom.pivotExt;
    out.yawDeg = m_holdFrom.yawDeg;
    out.pitchDeg = m_holdFrom.pitchDeg;

    out.dist = Math::Lerp(m_holdFrom.dist, m_holdTo.dist, t);
    out.fov = Math::Lerp(m_holdFrom.fov, m_holdTo.fov, t);

    out.yawWeight = m_holdTo.yawWeight;
    out.baseVictimWeight = m_holdTo.baseVictimWeight;

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
        BeginShot(BuildShot2(), tune.snapShotSec, tune.holdShot2Sec, false);
        SnapTo(m_shotTo);
        m_state = State::Shot2_Hold;
        m_shotElapsed = 0.f;
        return;
    }

    if (m_state == State::Shot2_Hold)
    {
        m_state = State::Shot3_Snap;
        BeginShot(BuildShot3(), tune.snapShotSec, tune.holdShot3Sec, false);
        SnapTo(m_shotTo);
        m_state = State::Shot3_Hold;
        m_shotElapsed = 0.f;
        return;
    }

    if (m_state == State::Shot3_Hold)
    {
        m_state = State::Shot4_Snap;
        BeginShot(BuildShot4(), tune.snapShotSec, tune.holdShot4Sec, false);
        SnapTo(m_shotTo);

        const Vector3 basePivot = BasePivotWorld(m_holdFrom.baseVictimWeight);
        m_shot4PivotFixed = basePivot + m_holdFrom.pivotExt;

        m_shot4RotFixed = YawPitchQuatDeg(m_holdFrom.yawDeg, m_holdFrom.pitchDeg);

        m_shot4DistFrom = m_holdFrom.dist;
        m_shot4DistTo = m_holdTo.dist;

        m_shot4FovFrom = m_holdFrom.fov;
        m_shot4FovTo = m_holdTo.fov;

        m_shot4RailActive = true;

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

void CamWipeOutController::UpdatePivots(_float dt)
{
    auto orbit = CamDirector()->GetOrbitCam();
    const _float offsetY = orbit->GetOffsetY();

    const PivotSample a = SamplePivots(m_attacker, offsetY);
    if (a.valid)
    {
        const _float t = clamp(dt * 18.f, 0.f, 1.f);
        m_aBase = Vector3::Lerp(m_aBase, a.basePivot, t);
        m_aFace = Vector3::Lerp(m_aFace, a.facePivot, t);
        m_aValid = true;
    }

    const PivotSample v = SamplePivots(m_victim, offsetY);
    if (v.valid)
    {
        const _float t = clamp(dt * 18.f, 0.f, 1.f);
        m_vBase = Vector3::Lerp(m_vBase, v.basePivot, t);
        m_vFace = Vector3::Lerp(m_vFace, v.facePivot, t);
        m_vValid = true;
    }
}

NS_END