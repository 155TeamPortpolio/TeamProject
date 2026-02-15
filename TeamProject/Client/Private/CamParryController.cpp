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

void CamParryController::ApplyGoalPose_Snap(const ShotGoal& g)
{
    auto orbit = CamDirector()->GetOrbitCam();

    const Vector3 basePivot = m_aBase;
    const Vector3 pivotWorld = basePivot + g.pivotExt;

    const _float baseYaw = CurCamYawDeg();
    const _float yaw = baseYaw + Math::WrapDeg(g.yawDeg - baseYaw) * g.yawWeight;

    const Quaternion q = YawPitchRollQuatDeg(yaw, g.pitchDeg, g.rollDeg);
    const Vector3 camPos = OrbitPos(pivotWorld, q, g.dist);

    orbit->SnapFromCamPose(camPos, q);
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
}

void CamParryController::ClampAboveGround(ShotGoal& g) const
{
    auto attackerObj = ObjectManager()->Request_Object(m_attacker);
    auto attackerCC = attackerObj->Get_Component<CCharacterController>();

    const Vector4 foot4 = attackerCC->Get_FootPosition();
    const _float minFootY = foot4.y;

    const _float minPivotY = minFootY + tune.common.minPivotAboveFootY;
    const _float minCamY = minFootY + tune.common.minCamAboveFootY;

    const Vector3 basePivot = m_aBase;

    Vector3 pivotWorld = basePivot + g.pivotExt;
    if (pivotWorld.y < minPivotY) g.pivotExt.y += (minPivotY - pivotWorld.y);

    pivotWorld = basePivot + g.pivotExt;

    const Quaternion q = YawPitchRollQuatDeg(g.yawDeg, g.pitchDeg, g.rollDeg);
    const Vector3 camPos = OrbitPos(pivotWorld, q, g.dist);

    if (camPos.y < minCamY) g.pivotExt.y += (minCamY - camPos.y);
}

CamParryController::ShotGoal CamParryController::BuildBaseShot_NoLens(_int sideSign) const
{
    ShotGoal g{};

    Vector3 pivotWorld = m_aFace + m_dirXZ * tune.common.forwardOffset;

    const _float aimY = Math::Lerp(m_aBase.y, m_aFace.y, tune.common.pelvisMul);
    pivotWorld.y = aimY + tune.common.pivotYAdd;

    const Vector3 basePivot = m_aBase;

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

void CamParryController::CaptureCurAsFrom()
{
    auto orbit = CamDirector()->GetOrbitCam();

    OrbitSnapshot s{};
    orbit->CaptureSnapshot(s);

    ShotGoal from{};

    const Vector3 basePivot = m_aBase;
    const Vector3 pivotWorld = s.pose.pivotCurWorld;

    from.pivotExt = pivotWorld - basePivot;

    from.yawDeg = s.pose.rotCurDeg.x;
    from.pitchDeg = s.pose.rotCurDeg.y;
    from.rollDeg = 0.f;

    from.dist = s.pose.distCur;

    from.yawWeight = 1.f;

    ClampAboveGround(from);

    m_shotFrom = from;
}

void CamParryController::ClampEnter_NoDrop(ShotGoal& g) const
{
    const Vector3 basePivot = m_aBase;
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

    g.yawWeight = Math::Lerp(a.yawWeight, b.yawWeight, t);

    ClampAboveGround(g);
    ClampEnter_NoDrop(g);

    ApplyGoalPose_Snap(g);
}

void CamParryController::CaptureCurAsImpactBase()
{
    auto orbit = CamDirector()->GetOrbitCam();

    OrbitSnapshot s{};
    orbit->CaptureSnapshot(s);

    m_impactPivotWorld = s.pose.pivotCurWorld;

    ShotGoal g{};

    const Vector3 basePivot = m_aBase;
    g.pivotExt = m_impactPivotWorld - basePivot;

    g.yawDeg = s.pose.rotCurDeg.x;
    g.pitchDeg = s.pose.rotCurDeg.y;
    g.rollDeg = 0.f;

    g.dist = s.pose.distCur;

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

    const Quaternion qStart = YawPitchRollQuatDeg(g.yawDeg, m_impactBase.pitchDeg, 0.f);
    const _float startCamY = OrbitPos(m_impactPivotWorld, qStart, m_impactBase.dist).y;

    const _float mix = clamp(tune.impact.targetCamYMix, 0.f, 1.f);
    const _float targetCamY = Math::Lerp(startCamY, targetCamYFixed, mix);

    const Vector3 basePivot = m_aBase;
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

    const _float env = sinf(XM_PI * u);

    g.rollDeg = (_float)sideSign * tune.impact.rollMaxDeg * roll01 * tune.impact.rollArcMul;

    const _float rollPulse = sinf(2.f * XM_PI * (_float)tune.impact.rollShakeCount * u);
    g.rollDeg = g.rollDeg + (_float)sideSign * tune.impact.rollShakeDeg * rollPulse * env;

    ClampAboveGround(g);

    return g;
}

void CamParryController::ComputeSideFromCam()
{
    auto orbit = CamDirector()->GetOrbitCam();
    const _float offsetY = orbit->GetOffsetY();

    const Vector3 basePivot = OrbitBasePivotWorld(m_attacker, offsetY);
    Vector3 rel = CurCamPosWorld() - basePivot;
    rel.y = 0.f;

    Vector3 fwd = m_dirXZ;
    fwd.y = 0.f;
    if (fwd.LengthSquared() <= 1e-10f) fwd = Vector3(0.f, 0.f, 1.f);
    fwd.Normalize();

    Vector3 right = Vector3::Up.Cross(fwd);
    right.y = 0.f;
    if (right.LengthSquared() <= 1e-10f) right = Vector3(1.f, 0.f, 0.f);
    right.Normalize();

    m_isLeft = (rel.Dot(right) < 0.f);
    m_sideSign = m_isLeft ? -1 : 1;
}


string CamParryController::BuildParryKey() const
{
    const CHARACTER charaName = CamDirector()->GetCharacterName();

    string key = "Parry/";
    key += Helper::EnumToString(charaName);
    key += m_isLeft ? "_Left" : "_Right";
    return key;
}

void CamParryController::Reset()
{
    m_active = false;
    m_state = State::None;

    m_elapsed = 0.f;

    m_attacker.Reset();

    m_sideSign = 1;
    m_isLeft = false;

    m_aBase = Vector3::Zero;
    m_aFace = Vector3::Zero;
    m_aValid = false;

    m_dirXZ = Vector3(0.f, 0.f, 1.f);

    m_shotFrom = {};
    m_shotTo = {};

    m_prevOrbitCaptured = false;
    m_prevOrbit = {};

    m_enterCamY = 0.f;

    m_impactBase = {};
    m_impactPivotWorld = Vector3::Zero;
    m_impactCaptured = false;
}

void CamParryController::Begin()
{
    Reset();

    m_attacker = CamDirector()->GetCurHandle();

    auto orbit = CamDirector()->GetOrbitCam();

    const _float offsetY = orbit->GetOffsetY();

    const PivotSample attackerSample = SamplePivots(m_attacker, offsetY);
    m_aValid = attackerSample.valid;
    m_aBase = attackerSample.basePivot;
    m_aFace = attackerSample.facePivot;

    auto attackerObj = ObjectManager()->Request_Object(m_attacker);
    auto attackerTf = attackerObj->Get_Component<CTransform>();

    Vector3 attackerFwd = attackerTf->Dir(STATE::LOOK);
    attackerFwd.y = 0.f;
    if (attackerFwd.LengthSquared() <= 1e-10f) attackerFwd = Vector3(0.f, 0.f, 1.f);
    attackerFwd.Normalize();

    m_dirXZ = attackerFwd;

    ComputeSideFromCam();

    m_prevOrbitCaptured = true;
    orbit->CaptureSnapshot(m_prevOrbit);

    CaptureCurAsFrom();
    m_enterCamY = CurCamPosWorld().y;

    m_shotTo = BuildBaseShot_NoLens(m_sideSign);
    m_shotTo.dist = m_shotFrom.dist;

    m_active = true;
    m_state = State::Enter;
    m_elapsed = 0.f;

    ApplyGoalPose_Snap(m_shotFrom);

    orbit->ParryMode_Begin();
}

void CamParryController::End()
{
    if (!m_active) return;

    auto orbit = CamDirector()->GetOrbitCam();

    orbit->ParryMode_End();

    CamDirector()->RequestSequence(BuildParryKey());

    Reset();
}

void CamParryController::Update(_float dt)
{
    if (!m_active) return;

    m_elapsed += dt;

    if (m_state != State::WaitEnd)
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
            m_state = State::WaitEnd;

        return;
    }

    if (m_state == State::WaitEnd)
    {
        End();
        return;
    }
}
