#include "pch.h"
#include "CamParryController.h"

#include "CamDirector.h"
#include "GameInstance.h"
#include "CharacterController.h"

namespace
{
    Vector3 OrbitBasePivotWorld(OBJECT_HANDLE h, _float offsetY)
    {
        auto obj = ObjectManager()->Request_Object(h);
        auto cc = obj->Get_Component<CCharacterController>();

        const Vector4 foot4 = cc->Get_FootPosition();
        const Vector3 foot(foot4.x, foot4.y, foot4.z);

        return foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + offsetY, 0.f);
    }
}

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
    const _float pitchRad = XMConvertToRadians(pitchDeg);
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

_int CamParryController::ChooseSideSignByCamDist() const
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

    const Vector3 attackerFace = m_aFace;
    const Vector3 victimFace = m_vValid ? m_vFace : (m_aFace + m_dirXZ);

    _float bias = clamp(tune.common.contactBias, 0.f, 1.f);
    Vector3 pivotWorld = Vector3::Lerp(attackerFace, victimFace, bias);
    pivotWorld += m_dirXZ * tune.common.forwardOffset;

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

    const Vector3 baseLook(m_dirXZ.x, 0.f, m_dirXZ.z);
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

    ClampAboveGround(from);

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

void CamParryController::ClampEnter_NoDrop(ShotGoal& g) const
{
    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);
    const Vector3 pivotWorld = basePivot + g.pivotExt;

    auto EvalCamY = [&](float pitchDeg)
        {
            const Quaternion q = YawPitchRollQuatDeg(g.yawDeg, pitchDeg, g.rollDeg);
            const Vector3 camPos = OrbitPos(pivotWorld, q, g.dist);
            return camPos.y;
        };

    const _float y0 = EvalCamY(g.pitchDeg);
    if (y0 >= m_enterCamY) return;

    const _float yPlus = EvalCamY(g.pitchDeg + 1.f);
    const _bool plusRaises = (yPlus > y0);

    _float lo = plusRaises ? g.pitchDeg : -89.f;
    _float hi = plusRaises ? 89.f : g.pitchDeg;

    for (_int i = 0; i < 16; ++i)
    {
        const _float mid = (lo + hi) * 0.5f;
        const _float yMid = EvalCamY(mid);

        if (yMid < m_enterCamY) lo = mid;
        else hi = mid;
    }

    g.pitchDeg = hi;
}

void CamParryController::ApplyInterpolated_Enter(const ShotGoal& a, const ShotGoal& b, _float t)
{
    ShotGoal g{};

    g.pivotExt = Vector3::Lerp(a.pivotExt, b.pivotExt, t);

    const _float yaw = a.yawDeg + Math::WrapDeg(b.yawDeg - a.yawDeg) * t;
    g.yawDeg = yaw;

    g.pitchDeg = Math::Lerp(a.pitchDeg, b.pitchDeg, t);
    g.rollDeg = a.rollDeg + Math::WrapDeg(b.rollDeg - a.rollDeg) * t;

    g.dist = a.dist;
    g.fov = a.fov;

    g.yawWeight = Math::Lerp(a.yawWeight, b.yawWeight, t);
    g.baseVictimWeight = a.baseVictimWeight;

    ClampAboveGround(g);
    ClampEnter_NoDrop(g);

    ApplyGoalPose_Snap(g);
}

CamParryController::ShotGoal CamParryController::BuildBaseShot_NoLens(_int sideSign) const
{
    ShotGoal g{};

    const Vector3 attackerFace = m_aFace;
    const Vector3 victimFace = m_vValid ? m_vFace : (m_aFace + m_dirXZ);

    _float bias = clamp(tune.common.contactBias, 0.f, 1.f);
    Vector3 pivotWorld = Vector3::Lerp(attackerFace, victimFace, bias);
    pivotWorld += m_dirXZ * tune.common.forwardOffset;

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

    const Vector3 baseLook(m_dirXZ.x, 0.f, m_dirXZ.z);
    const Vector3 camDir = RotateYDegXZ(baseLook, (_float)sideSign * tune.common.angleDeg);

    g.yawDeg = YawFromDirXZ(camDir) + (_float)sideSign * tune.common.sideYawBiasDeg;
    g.pitchDeg = tune.common.pitchDeg;
    g.rollDeg = 0.f;

    g.yawWeight = 1.f;

    ClampAboveGround(g);

    return g;
}

void CamParryController::CaptureCurAsImpactBase()
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = CamDirector()->GetOrbitCamComp();

    OrbitSnapshot s{};
    orbit->CaptureSnapshot(s);

    m_impactPivotWorld = s.pose.pivotCurWorld;

    ShotGoal g{};

    g.baseVictimWeight = 0.f;

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);
    g.pivotExt = m_impactPivotWorld - basePivot;

    g.yawDeg = s.pose.rotCurDeg.x;
    g.pitchDeg = s.pose.rotCurDeg.y;
    g.rollDeg = 0.f;

    g.dist = s.pose.distCur;
    g.fov = cam->Get_FOV();

    g.yawWeight = 1.f;

    ClampAboveGround(g);

    m_impactBase = g;
    m_impactCaptured = true;
}

CamParryController::ShotGoal CamParryController::BuildImpactShot(_int sideSign, _float close01, _float roll01, _float u) const
{
    ShotGoal g = m_impactBase;

    close01 = clamp(close01, 0.f, 1.f);
    roll01 = clamp(roll01, 0.f, 1.f);
    u = clamp(u, 0.f, 1.f);

    const _float distEnd = max(0.f, m_impactBase.dist - tune.impact.punchDistDelta);
    g.dist = Math::Lerp(m_impactBase.dist, distEnd, close01);

    auto attackerObj = ObjectManager()->Request_Object(m_attacker);
    auto attackerCC = attackerObj->Get_Component<CCharacterController>();
    const _float footY = XMVectorGetY(attackerCC->Get_FootPosition());

    const _float targetCamYFixed = footY + tune.impact.endCamAboveFootY;

    const Vector3 basePivot0 = BasePivotWorld(g.baseVictimWeight);
    Vector3 pivotWorld0 = basePivot0 + (m_impactPivotWorld - basePivot0);

    const Quaternion qStart = YawPitchRollQuatDeg(g.yawDeg, m_impactBase.pitchDeg, 0.f);
    const _float startCamY = OrbitPos(pivotWorld0, qStart, m_impactBase.dist).y;

    const _float mix = clamp(tune.impact.targetCamYMix, 0.f, 1.f);
    const _float targetCamY = Math::Lerp(startCamY, targetCamYFixed, mix);

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);
    g.pivotExt = m_impactPivotWorld - basePivot;
    g.pivotExt.y -= tune.impact.pivotDropY * close01;

    const Vector3 pivotWorld = basePivot + g.pivotExt;

    auto EvalCamY = [&](float pitchDeg)
        {
            const Quaternion q = YawPitchRollQuatDeg(g.yawDeg, pitchDeg, 0.f);
            const Vector3 camPos = OrbitPos(pivotWorld, q, g.dist);
            return camPos.y;
        };

    const _float y0 = EvalCamY(0.f);
    const _float yPlus = EvalCamY(1.f);
    const _bool plusRaises = (yPlus > y0);

    _float lo = -89.f;
    _float hi = 89.f;

    for (_int i = 0; i < 16; ++i)
    {
        const _float mid = (lo + hi) * 0.5f;
        const _float yMid = EvalCamY(mid);

        if (plusRaises)
        {
            if (yMid < targetCamY) lo = mid;
            else hi = mid;
        }
        else
        {
            if (yMid < targetCamY) hi = mid;
            else lo = mid;
        }
    }

    const _float pitchEnd = hi;
    g.pitchDeg = Math::Lerp(m_impactBase.pitchDeg, pitchEnd, close01);

    g.fov = m_impactBase.fov - tune.impact.punchFovAdd * close01;

    const _float env = sinf(XM_PI * u);

    const _float fovPulse = sinf(2.f * XM_PI * (_float)tune.impact.fovPulseCount * u);
    g.fov = g.fov + tune.impact.fovPulseAmp * fovPulse * env;

    g.rollDeg = (_float)sideSign * tune.impact.rollMaxDeg * roll01 * tune.impact.rollArcMul;

    const _float rollPulse = sinf(2.f * XM_PI * (_float)tune.impact.rollShakeCount * u);
    g.rollDeg = g.rollDeg + (_float)sideSign * tune.impact.rollShakeDeg * rollPulse * env;

    ClampAboveGround(g);

    return g;
}

void CamParryController::PrepareHold(const ShotGoal& impactEnd)
{
    m_holdFrom = impactEnd;

    m_holdMid = m_holdFrom;
    m_holdMid.rollDeg = 0.f;
    m_holdMid.yawWeight = 1.f;

    m_holdMid.pitchDeg = m_holdFrom.pitchDeg - tune.hold.risePitchLiftDeg;
    m_holdMid.dist = m_holdFrom.dist + tune.hold.riseDistAdd;
    m_holdMid.yawDeg = m_holdFrom.yawDeg + (_float)m_sideSign * tune.hold.riseYawSweepDeg;

    _float fovTo = m_prevFovCaptured ? m_prevFov : m_holdFrom.fov;
    _float fovW = clamp(tune.hold.fovRestoreMid01, 0.f, 1.f);
    m_holdMid.fov = Math::Lerp(m_holdFrom.fov, fovTo, fovW);

    m_holdTo = m_holdFrom;
    m_holdTo.baseVictimWeight = 0.f;
    m_holdTo.rollDeg = 0.f;
    m_holdTo.yawWeight = 1.f;

    if (m_prevOrbitCaptured)
    {
        m_holdTo.yawDeg = m_prevOrbit.pose.rotCurDeg.x;
        m_holdTo.pitchDeg = m_prevOrbit.pose.rotCurDeg.y;
        m_holdTo.dist = m_prevOrbit.pose.distCur;
    }

    if (m_prevFovCaptured) m_holdTo.fov = m_prevFov;

    const Vector3 basePivot = BasePivotWorld(m_holdFrom.baseVictimWeight);
    m_holdPivotFromWorld = basePivot + m_holdFrom.pivotExt;
    m_holdPivotMidWorld = m_holdPivotFromWorld + Vector3(0.f, tune.hold.pivotUp, 0.f);

    auto orbit = CamDirector()->GetOrbitCam();
    const _float offsetY = orbit->GetOffsetY();

    const OBJECT_HANDLE curHandle = CamDirector()->GetCurHandle();
    const Vector3 curBaseOrbit = OrbitBasePivotWorld(curHandle, offsetY);

    const Vector3 ext = m_prevOrbitCaptured ? m_prevOrbit.pose.pivotExternalOffset : Vector3::Zero;
    m_holdPivotToWorld = curBaseOrbit + ext;
}



void CamParryController::ApplyHoldOrbitReturn(_float u)
{
    u = clamp(u, 0.f, 1.f);

    const _float split = clamp(tune.hold.riseRatio, 0.01f, 0.99f);

    auto LerpGoalNoPivot = [&](const ShotGoal& a, const ShotGoal& b, _float t)
        {
            ShotGoal g{};
            g.baseVictimWeight = a.baseVictimWeight;

            g.yawDeg = a.yawDeg + Math::WrapDeg(b.yawDeg - a.yawDeg) * t;
            g.pitchDeg = Math::Lerp(a.pitchDeg, b.pitchDeg, t);
            g.rollDeg = a.rollDeg + Math::WrapDeg(b.rollDeg - a.rollDeg) * t;

            g.dist = Math::Lerp(a.dist, b.dist, t);
            g.fov = Math::Lerp(a.fov, b.fov, t);

            g.yawWeight = Math::Lerp(a.yawWeight, b.yawWeight, t);
            return g;
        };

    ShotGoal g{};
    Vector3 pivotWorld{};

    if (u < split)
    {
        const _float ua = u / split;
        const _float t = Math::ApplyEase(tune.hold.riseEase, ua);

        g = LerpGoalNoPivot(m_holdFrom, m_holdMid, t);
        pivotWorld = Vector3::Lerp(m_holdPivotFromWorld, m_holdPivotMidWorld, t);
    }
    else
    {
        const _float ub = (u - split) / (1.f - split);
        const _float t = Math::ApplyEase(tune.hold.returnEase, ub);

        g = LerpGoalNoPivot(m_holdMid, m_holdTo, t);
        pivotWorld = Vector3::Lerp(m_holdPivotMidWorld, m_holdPivotToWorld, t);
    }

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);
    g.pivotExt = pivotWorld - basePivot;

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
    m_holdMid = {};
    m_holdTo = {};

    m_holdPivotFromWorld = Vector3::Zero;
    m_holdPivotMidWorld = Vector3::Zero;
    m_holdPivotToWorld = Vector3::Zero;

    m_prevOrbitCaptured = false;
    m_prevOrbit = {};

    m_prevFovCaptured = false;
    m_prevFov = 0.f;

    m_victimBlocked = false;

    m_enterCamY = 0.f;

    m_impactBase = {};
    m_impactPivotWorld = Vector3::Zero;
    m_impactCaptured = false;
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

    Vector3 dir = attackerFwd;

    if (m_vValid)
    {
        Vector3 toVictim = m_vFace - m_aFace;
        toVictim.y = 0.f;

        if (toVictim.LengthSquared() > 1e-10f)
        {
            toVictim.Normalize();

            if (toVictim.Dot(attackerFwd) > 0.f)
            {
                _float w = clamp(tune.common.aimVictimBlend, 0.f, 1.f);
                dir = Vector3::Lerp(attackerFwd, toVictim, w);
                if (dir.LengthSquared() <= 1e-10f) dir = attackerFwd;
                else dir.Normalize();
            }
        }
    }

    m_dirXZ = dir;

    m_sideSign = ChooseSideSignByCamDist();

    m_prevOrbitCaptured = true;
    orbit->CaptureSnapshot(m_prevOrbit);

    m_prevFovCaptured = true;
    m_prevFov = cam->Get_FOV();

    CaptureCurAsFrom();
    m_enterCamY = CurCamPosWorld().y;

    m_shotTo = BuildBaseShot_NoLens(m_sideSign);
    m_shotTo.dist = m_shotFrom.dist;
    m_shotTo.fov = m_shotFrom.fov;

    m_holdFrom = {};
    m_holdMid = {};
    m_holdTo = {};

    m_holdPivotFromWorld = Vector3::Zero;
    m_holdPivotMidWorld = Vector3::Zero;
    m_holdPivotToWorld = Vector3::Zero;

    m_impactBase = {};
    m_impactPivotWorld = Vector3::Zero;
    m_impactCaptured = false;

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

    if (m_prevOrbitCaptured)
    {
        OrbitSnapshot now{};
        orbit->CaptureSnapshot(now);

        OrbitSnapshot s = m_prevOrbit;

        s.target = CamDirector()->GetCurHandle();
        s.sw = {};

        s.pose.pivotInternalOffset = Vector3::Zero;

        s.pose.rotCurDeg = now.pose.rotCurDeg;
        s.pose.rotGoalDeg = now.pose.rotGoalDeg;

        s.pose.distCur = now.pose.distCur;
        s.pose.distGoal = now.pose.distGoal;
        s.pose.distWanted = now.pose.distWanted;

        orbit->RestoreSnapshot(s);
    }

    if (m_prevFovCaptured) cam->Set_FOV(m_prevFov);

    //auto charaName = CamDirector()->GetCharacterName();

    //switch (charaName)
    //{
    //case CHARACTER::Corin:
    //    CamDirector()->RequestSequence("Parry/Corin_Left");
    //    break;

    //case CHARACTER::JaneDoe:
    //    CamDirector()->RequestSequence("Parry/JaneDoe_Left");
    //    break;

    //case CHARACTER::Miyabi:
    //    CamDirector()->RequestSequence("Parry/Miyabi_Left");
    //    break;
    //}

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

        ApplyInterpolated_Enter(m_shotFrom, m_shotTo, t);

        if (u >= 1.f)
        {
            CaptureCurAsImpactBase();
            m_state = State::Impact;
            m_elapsed = 0.f;
        }
        return;
    }

    if (m_state == State::Impact)
    {
        if (!m_impactCaptured) CaptureCurAsImpactBase();

        const _float u = (tune.common.impactSec > 0.f) ? clamp(m_elapsed / tune.common.impactSec, 0.f, 1.f) : 1.f;

        const _float close01 = Math::ApplyEase(tune.common.impactEase, u);
        const _float roll01 = sinf(XM_PI * u);

        ShotGoal g = BuildImpactShot(m_sideSign, close01, roll01, u);
        ApplyGoalPose_Snap(g);

        if (u >= 1.f)
        {
            if (tune.common.holdSec <= 0.f)
            {
                End();
                return;
            }

            PrepareHold(g);

            m_state = State::Hold;
            m_elapsed = 0.f;
        }
        return;
    }

    if (m_state == State::Hold)
    {
        if (tune.common.holdSec <= 0.f)
        {
            End();
            return;
        }

        const _float u = clamp(m_elapsed / tune.common.holdSec, 0.f, 1.f);

        ApplyHoldOrbitReturn(u);

        if (u >= 1.f)
        {
            ApplyHoldOrbitReturn(1.f);
            End();
            return;
        }
        return;
    }
}
