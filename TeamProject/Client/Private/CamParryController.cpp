// CamParryController.cpp
#include "pch.h"
#include "CamParryController.h"

#include "CamDirector.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "CharacterController.h"

CamParryController::PivotSample CamParryController::SamplePivots(OBJECT_HANDLE h, _float offsetY, _float faceYOffsetMul)
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

    const Vector3 base = foot + Vector3(0.f, half * baseMul + offsetY, 0.f);
    const Vector3 face = foot + Vector3(0.f, half * faceMul + offsetY, 0.f);

    s.basePivot = base;
    s.facePivot = face;
    s.valid = true;
    return s;
}

Vector3 CamParryController::ClampOffset(const Vector3& offset, _float maxLen)
{
    if (maxLen <= 0.f) return offset;

    const _float len = offset.Length();
    if (len <= maxLen) return offset;
    if (len <= 0.f) return Vector3::Zero;

    return offset * (maxLen / len);
}

Vector3 CamParryController::RotateYDegXZ(const Vector3& dirXZ, _float deg)
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

_float CamParryController::YawFromDirXZ(const Vector3& dirXZ)
{
    Vector3 d = dirXZ;
    d.y = 0.f;
    if (d.LengthSquared() <= 1e-10f) return 0.f;
    d.Normalize();
    return XMConvertToDegrees(atan2f(d.x, d.z));
}

Quaternion CamParryController::YawPitchRollQuatDeg(_float yawDeg, _float pitchDeg, _float rollDeg)
{
    const _float yawRad = XMConvertToRadians(yawDeg);
    const _float pitchRad = XMConvertToRadians(-pitchDeg);
    const _float rollRad = XMConvertToRadians(rollDeg);
    return Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, rollRad);
}

Vector3 CamParryController::OrbitPos(const Vector3& pivotWorld, const Quaternion& q, _float dist)
{
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    return pivotWorld + backDir * dist;
}

Vector3 CamParryController::BasePivotWorld(_float baseVictimWeight) const
{
    if (!m_vValid) return m_aBase;
    return Vector3::Lerp(m_aBase, m_vBase, clamp(baseVictimWeight, 0.f, 1.f));
}

void CamParryController::ApplyGoalPose_Snap(const ShotGoal& g)
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);
    const Vector3 pivotWorld = basePivot + g.pivotExt;

    const _float baseYaw = CurCamYawDeg();
    const _float yaw = baseYaw + Math::WrapDeg(g.yawDeg - baseYaw) * g.yawWeight;

    const Quaternion q = YawPitchRollQuatDeg(yaw, g.pitchDeg, g.rollDeg);
    const Vector3 camPos = OrbitPos(pivotWorld, q, g.dist);

    orbit->SnapFromCamPose(camPos, q);
    cam->Set_FOV(g.fov);
}

_float CamParryController::CurCamYawDeg() const
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto tf = orbit->Get_Component<CTransform>();

    Vector3 camLook = tf->Dir(STATE::LOOK);
    camLook.y = 0.f;
    if (camLook.LengthSquared() <= 1e-10f) camLook = Vector3(0.f, 0.f, 1.f);
    camLook.Normalize();

    return YawFromDirXZ(camLook);
}

Vector3 CamParryController::CurCamPosWorld() const
{
    auto orbit = CamDirector()->GetOrbitCam();

    OrbitSnapshot s{};
    orbit->CaptureSnapshot(s);

    const Quaternion q = YawPitchRollQuatDeg(s.pose.rotCurDeg.x, s.pose.rotCurDeg.y, 0.f);
    return OrbitPos(s.pose.pivotCurWorld, q, s.pose.distCur);
}

_int CamParryController::ChooseSideSignByCamDistance() const
{
    const Vector3 curPos = CurCamPosWorld();

    ShotGoal a = BuildBaseShot(1);
    ShotGoal b = BuildBaseShot(-1);

    const Vector3 basePivotA = BasePivotWorld(a.baseVictimWeight);
    const Vector3 pivotWorldA = basePivotA + a.pivotExt;
    const Quaternion qA = YawPitchRollQuatDeg(a.yawDeg, a.pitchDeg, 0.f);
    const Vector3 posA = OrbitPos(pivotWorldA, qA, a.dist);

    const Vector3 basePivotB = BasePivotWorld(b.baseVictimWeight);
    const Vector3 pivotWorldB = basePivotB + b.pivotExt;
    const Quaternion qB = YawPitchRollQuatDeg(b.yawDeg, b.pitchDeg, 0.f);
    const Vector3 posB = OrbitPos(pivotWorldB, qB, b.dist);

    const _float da = (posA - curPos).LengthSquared();
    const _float db = (posB - curPos).LengthSquared();

    return (da <= db) ? 1 : -1;
}

void CamParryController::UpdatePivots(_float dt)
{
    auto orbit = CamDirector()->GetOrbitCam();
    const _float offsetY = orbit->GetOffsetY();

    const PivotSample attackerSample = SamplePivots(m_attacker, offsetY);
    if (attackerSample.valid)
    {
        const _float t = clamp(dt * 18.f, 0.f, 1.f);
        m_aBase = Vector3::Lerp(m_aBase, attackerSample.basePivot, t);
        m_aFace = Vector3::Lerp(m_aFace, attackerSample.facePivot, t);
        m_aValid = true;
    }

    if (m_victimBlocked)
    {
        m_vBase = Vector3::Zero;
        m_vFace = Vector3::Zero;
        m_vValid = false;
        return;
    }

    const PivotSample victimSample = SamplePivots(m_victim, offsetY);
    if (victimSample.valid)
    {
        Vector3 delta = victimSample.facePivot - m_aFace;
        delta.y = 0.f;
        const _float dist = delta.Length();

        if (dist > tune.common.maxVictimDist)
        {
            m_vBase = Vector3::Zero;
            m_vFace = Vector3::Zero;
            m_vValid = false;
            m_victimBlocked = true;
            return;
        }

        const _float t = clamp(dt * 18.f, 0.f, 1.f);
        m_vBase = Vector3::Lerp(m_vBase, victimSample.basePivot, t);
        m_vFace = Vector3::Lerp(m_vFace, victimSample.facePivot, t);
        m_vValid = true;
    }
}

void CamParryController::ClampAboveGround(ShotGoal& g) const
{
    auto attackerObj = ObjectManager()->Request_Object(m_attacker);
    auto attackerCC = attackerObj->Get_Component<CCharacterController>();

    const Vector4 foot4 = attackerCC->Get_FootPosition();
    const _float minFootY = foot4.y;

    const _float minPivotY = minFootY + tune.common.minPivotAboveFootY;
    const _float minCamY = minFootY + tune.common.minCamAboveFootY;

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);

    Vector3 pivotWorld = basePivot + g.pivotExt;
    if (pivotWorld.y < minPivotY) g.pivotExt.y += (minPivotY - pivotWorld.y);

    pivotWorld = basePivot + g.pivotExt;

    const Quaternion q = YawPitchRollQuatDeg(g.yawDeg, g.pitchDeg, g.rollDeg);
    const Vector3 camPos = OrbitPos(pivotWorld, q, g.dist);

    if (camPos.y < minCamY) g.pivotExt.y += (minCamY - camPos.y);
}

CamParryController::ShotGoal CamParryController::BuildBaseShot(_int sideSign) const
{
    ShotGoal g{};

    auto attackerObj = ObjectManager()->Request_Object(m_attacker);
    auto attackerTf = attackerObj->Get_Component<CTransform>();

    Vector3 attackerFwd = attackerTf->Dir(STATE::LOOK);
    attackerFwd.y = 0.f;
    if (attackerFwd.LengthSquared() <= 1e-10f) attackerFwd = Vector3(0.f, 0.f, 1.f);
    attackerFwd.Normalize();

    const Vector3 attackerFace = m_aFace;
    const Vector3 victimFace = m_vValid ? m_vFace : (m_aFace + attackerFwd);

    Vector3 pivotWorld = Vector3::Lerp(attackerFace, victimFace, 0.5f);
    pivotWorld += attackerFwd * tune.common.forwardOffset;

    const _float aimY = Math::Lerp(m_aBase.y, m_aFace.y, tune.common.pelvisMul);
    pivotWorld.y = aimY + tune.common.pivotYAdd;

    g.baseVictimWeight = 0.f;

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);

    Vector3 ext = pivotWorld - basePivot;

    Vector3 extXZ(ext.x, 0.f, ext.z);
    extXZ = ClampOffset(extXZ, tune.common.pivotClamp);

    ext.x = extXZ.x;
    ext.z = extXZ.z;

    g.pivotExt = ext;

    const Vector3 baseLook(-m_dirXZ.x, 0.f, -m_dirXZ.z);
    const Vector3 camDir = RotateYDegXZ(baseLook, (_float)sideSign * tune.common.angleDeg);

    g.yawDeg = YawFromDirXZ(camDir) + (_float)sideSign * tune.common.sideYawBiasDeg;
    g.pitchDeg = tune.common.pitchDeg;
    g.rollDeg = 0.f;

    g.dist = tune.common.dist;
    g.fov = tune.common.fov;
    g.yawWeight = 1.f;

    ClampAboveGround(g);

    return g;
}

CamParryController::ShotGoal CamParryController::BuildImpactShot(_int sideSign, _float close01, _float roll01) const
{
    ShotGoal g = BuildBaseShot(sideSign);

    close01 = clamp(close01, 0.f, 1.f);
    roll01 = clamp(roll01, 0.f, 1.f);

    g.dist = max(0.f, g.dist - tune.impact.punchDistDelta * close01);
    g.fov = g.fov + tune.impact.punchFovAdd * close01;

    g.rollDeg = (_float)sideSign * tune.impact.rollMaxDeg * roll01;

    ClampAboveGround(g);

    return g;
}

void CamParryController::CaptureCurAsFrom()
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
    from.rollDeg = 0.f;

    from.dist = s.pose.distCur;
    from.fov = cam->Get_FOV();

    from.yawWeight = 1.f;

    m_shotFrom = from;
}

void CamParryController::ApplyInterpolated(const ShotGoal& a, const ShotGoal& b, _float t)
{
    ShotGoal g{};

    g.pivotExt = Vector3::Lerp(a.pivotExt, b.pivotExt, t);

    const _float yaw = a.yawDeg + Math::WrapDeg(b.yawDeg - a.yawDeg) * t;
    g.yawDeg = yaw;

    g.pitchDeg = Math::Lerp(a.pitchDeg, b.pitchDeg, t);
    g.rollDeg = a.rollDeg + Math::WrapDeg(b.rollDeg - a.rollDeg) * t;

    g.dist = Math::Lerp(a.dist, b.dist, t);
    g.fov = Math::Lerp(a.fov, b.fov, t);

    g.yawWeight = Math::Lerp(a.yawWeight, b.yawWeight, t);
    g.baseVictimWeight = a.baseVictimWeight;

    ClampAboveGround(g);

    ApplyGoalPose_Snap(g);
}

void CamParryController::Reset()
{
    m_active = false;
    m_state = State::None;

    m_elapsed = 0.f;

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

    m_shotFrom = {};
    m_shotTo = {};

    m_holdFrom = {};
    m_holdTo = {};

    m_prevOrbitCaptured = false;
    m_prevOrbit = {};

    m_prevFovCaptured = false;
    m_prevFov = 0.f;

    m_victimBlocked = false;
}

void CamParryController::Begin()
{
    Reset();

    m_attacker = CamDirector()->GetCurHandle();
    m_victim = CamDirector()->GetCurTarget();

    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    const _float offsetY = orbit->GetOffsetY();

    const PivotSample attackerSample = SamplePivots(m_attacker, offsetY);
    m_aValid = attackerSample.valid;
    m_aBase = attackerSample.basePivot;
    m_aFace = attackerSample.facePivot;

    const PivotSample victimSample = SamplePivots(m_victim, offsetY);
    if (victimSample.valid)
    {
        m_vBase = victimSample.basePivot;
        m_vFace = victimSample.facePivot;
        m_vValid = true;

        Vector3 delta = m_vFace - m_aFace;
        delta.y = 0.f;
        const _float dist = delta.Length();

        if (dist > tune.common.maxVictimDist)
        {
            m_vBase = Vector3::Zero;
            m_vFace = Vector3::Zero;
            m_vValid = false;
            m_victimBlocked = true;
        }
    }

    auto attackerObj = ObjectManager()->Request_Object(m_attacker);
    auto attackerTf = attackerObj->Get_Component<CTransform>();

    Vector3 attackerFwd = attackerTf->Dir(STATE::LOOK);
    attackerFwd.y = 0.f;
    if (attackerFwd.LengthSquared() <= 1e-10f) attackerFwd = Vector3(0.f, 0.f, 1.f);
    attackerFwd.Normalize();

    const Vector3 attackerFace = m_aFace;
    const Vector3 victimFace = m_vValid ? m_vFace : (m_aFace + attackerFwd);

    Vector3 dir = victimFace - attackerFace;
    dir.y = 0.f;
    if (dir.LengthSquared() <= 1e-10f) dir = attackerFwd;
    else dir.Normalize();

    m_dirXZ = dir;

    m_sideSign = ChooseSideSignByCamDistance();

    m_prevOrbitCaptured = true;
    orbit->CaptureSnapshot(m_prevOrbit);

    m_prevFovCaptured = true;
    m_prevFov = cam->Get_FOV();

    CaptureCurAsFrom();

    m_shotTo = BuildBaseShot(m_sideSign);

    m_holdFrom = {};
    m_holdTo = {};

    m_active = true;
    m_state = State::Enter;
    m_elapsed = 0.f;

    ApplyGoalPose_Snap(m_shotFrom);

    orbit->Lock_Input();
    orbit->ParryMode_Begin();
    orbit->DialogueMode_Begin();
    orbit->DialogueYaw_Clear();
}

void CamParryController::End()
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

void CamParryController::Update(_float dt)
{
    if (!m_active) return;

    m_elapsed += dt;

    UpdatePivots(dt);

    if (m_state == State::Enter)
    {
        const _float u = (tune.common.enterSec > 0.f) ? clamp(m_elapsed / tune.common.enterSec, 0.f, 1.f) : 1.f;
        const _float t = Math::ApplyEase(tune.common.approachEase, u);

        ApplyInterpolated(m_shotFrom, m_shotTo, t);

        if (u >= 1.f)
        {
            m_state = State::Impact;
            m_elapsed = 0.f;
        }
        return;
    }

    if (m_state == State::Impact)
    {
        const _float u = (tune.common.impactSec > 0.f) ? clamp(m_elapsed / tune.common.impactSec, 0.f, 1.f) : 1.f;

        const _float close01 = Math::ApplyEase(tune.common.impactEase, u);
        const _float roll01 = sinf(XM_PI * u);

        ShotGoal g = BuildImpactShot(m_sideSign, close01, roll01);
        ApplyGoalPose_Snap(g);

        if (u >= 1.f)
        {
            m_holdFrom = BuildImpactShot(m_sideSign, 1.f, 0.f);
            m_holdTo = BuildBaseShot(m_sideSign);

            m_state = State::Hold;
            m_elapsed = 0.f;
        }
        return;
    }

    if (m_state == State::Hold)
    {
        if (tune.common.holdSec <= 0.f)
        {
            ApplyGoalPose_Snap(m_holdTo);
            End();
            return;
        }

        const _float u = clamp(m_elapsed / tune.common.holdSec, 0.f, 1.f);
        const _float t = Math::ApplyEase(tune.common.holdEase, u);

        ApplyInterpolated(m_holdFrom, m_holdTo, t);

        if (u >= 1.f)
        {
            End();
            return;
        }
        return;
    }
}
