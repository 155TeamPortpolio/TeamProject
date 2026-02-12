// CamWipeOutController.cpp
#include "pch.h"
#include "CamWipeOutController.h"

#include "CamDirector.h"
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

_float CamWipeOutController::FitDistForRadius(_float radius, _float fovYDeg, _float aspect, _float margin)
{
    const _float fovYRad = XMConvertToRadians(max(fovYDeg, 1.f));
    const _float halfY = fovYRad * 0.5f;
    const _float tanY = tanf(halfY);

    const _float a = max(aspect, 0.001f);
    const _float fovXRad = 2.f * atanf(tanY * a);
    const _float halfX = fovXRad * 0.5f;
    const _float tanX = tanf(halfX);

    _float needY = 0.f;
    if (tanY > 1e-6f) needY = radius / tanY;

    _float needX = 0.f;
    if (tanX > 1e-6f) needX = radius / tanX;

    return max(needX, needY) * max(margin, 1.f);
}

_float CamWipeOutController::FitDistForPair(const ShotGoal& g, const Vector3& aWorld, const Vector3& bWorld) const
{
    auto cam = CamDirector()->GetOrbitCamComp();
    const _float aspect = cam->Get_Aspect();

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);
    const Vector3 pivotWorld = basePivot + g.pivotExt;

    Vector3 a = aWorld;
    Vector3 b = bWorld;

    a.y = pivotWorld.y;
    b.y = pivotWorld.y;

    const Vector3 da(a.x - pivotWorld.x, 0.f, a.z - pivotWorld.z);
    const Vector3 db(b.x - pivotWorld.x, 0.f, b.z - pivotWorld.z);

    _float r = max(da.Length(), db.Length());
    r = max(r, tune.common.fitMinRadius);

    return FitDistForRadius(r, g.fov, aspect, tune.common.fitMargin);
}

void CamWipeOutController::ClampShot1AboveGround(ShotGoal& g) const
{
    auto attackerObj = ObjectManager()->Request_Object(m_attacker);
    auto attackerCC = attackerObj->Get_Component<CCharacterController>();

    const Vector4 foot4 = attackerCC->Get_FootPosition();
    const _float minFootY = foot4.y;

    const _float minPivotY = minFootY + tune.shot1.minPivotAboveFootY;
    const _float minCamY = minFootY + tune.shot1.minCamAboveFootY;

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);

    Vector3 pivotWorld = basePivot + g.pivotExt;
    if (pivotWorld.y < minPivotY) g.pivotExt.y += (minPivotY - pivotWorld.y);

    pivotWorld = basePivot + g.pivotExt;

    const Quaternion q = YawPitchQuatDeg(g.yawDeg, g.pitchDeg);
    const Vector3 camPos = OrbitPos(pivotWorld, q, g.dist);

    if (camPos.y < minCamY) g.pivotExt.y += (minCamY - camPos.y);
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShotCommon(_int sideSign, _float angleDeg, _float pitchDeg, _float dist, _float fov, _float pivotClamp, _float attackerBias, _float baseVictimWeight, _bool useMid) const
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

    Vector3 pivotWorld = attackerFace;

    if (useMid && m_vValid)
    {
        Vector3 mid = (attackerFace + victimFace) * 0.5f;

        _float sep01 = 0.f;
        if (tune.common.sepMax > tune.common.sepMin) sep01 = (m_sep - tune.common.sepMin) / (tune.common.sepMax - tune.common.sepMin);
        sep01 = clamp(sep01, 0.f, 1.f);

        const _float bias = attackerBias * (1.f - sep01);
        const Vector3 biasDir = (attackerFace - mid);
        mid = mid + biasDir * bias;

        const Vector3 sideDir(-m_dirXZ.z, 0.f, m_dirXZ.x);
        mid = mid + sideDir * ((_float)sideSign * pivotClamp * tune.common.frameBiasMul);

        pivotWorld = mid;
    }

    const _float aimY = Math::Lerp(m_aBase.y, m_aFace.y, tune.common.pelvisMul);
    pivotWorld.y = aimY;

    _float w = clamp(baseVictimWeight, 0.f, 1.f);
    if (!m_vValid) w = 0.f;

    g.baseVictimWeight = w;

    const Vector3 basePivot = BasePivotWorld(g.baseVictimWeight);

    Vector3 ext = pivotWorld - basePivot;

    Vector3 extXZ(ext.x, 0.f, ext.z);
    extXZ = ClampOffset(extXZ, pivotClamp);

    ext.x = extXZ.x;
    ext.z = extXZ.z;

    g.pivotExt = ext;

    const Vector3 baseLook(-m_dirXZ.x, 0.f, -m_dirXZ.z);
    const Vector3 camDir = RotateYDegXZ(baseLook, (_float)sideSign * angleDeg);

    g.yawDeg = YawFromDirXZ(camDir) + (_float)sideSign * tune.common.sideYawBiasDeg;
    g.pitchDeg = pitchDeg;
    g.dist = dist;
    g.fov = fov;
    g.yawWeight = 1.f;

    return g;
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot1() const
{
    const _float dist = tune.common.distClose * tune.shot1.distMul;
    const _float fov = tune.shot1.fov + tune.shot1.fovAdd;
    const _float pitch = tune.shot1.pitchUpDeg + tune.shot1.pitchAddDeg;

    ShotGoal g = BuildShotCommon(m_sideSign, tune.shot1.angleDeg, pitch, dist, fov, tune.shot1.pivotClamp, tune.shot1.attackerBias, 0.f, false);

    const _float baseYaw = CurCamYawDeg();
    g.yawDeg = baseYaw + (_float)m_sideSign * tune.shot1.yawDeltaDeg;
    g.yawWeight = tune.shot1.yawWeight;

    g.pivotExt.y += tune.shot1.pivotYAdd;

    g.dist = max(0.f, g.dist - tune.shot1.holdDolly);

    ClampShot1AboveGround(g);

    return g;
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot2() const
{
    ShotGoal g = BuildShotCommon(-m_sideSign, tune.shot2.angleDeg, tune.shot2.pitchHighDeg, tune.shot2.distFar, tune.shot2.fov, tune.shot2.pivotClamp, tune.shot2.attackerBias, tune.shot2.baseVictimWeight, true);

    if (m_vValid)
    {
        const _float fit = FitDistForPair(g, m_aFace, m_vFace);
        const _float s = clamp(tune.shot2.fitStrength, 0.f, 1.f);
        g.dist = max(g.dist, Math::Lerp(g.dist, fit, s));
    }

    return g;
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot3() const
{
    ShotGoal g = BuildShotCommon(m_sideSign, tune.shot3.angleDeg, tune.common.pitchBaseDeg, tune.common.distMid, tune.common.fovMid, tune.shot3.pivotClamp, tune.shot3.attackerBias, tune.shot3.baseVictimWeight, true);

    if (m_vValid)
    {
        const _float fit = FitDistForPair(g, m_aFace, m_vFace);
        const _float s = clamp(tune.shot3.fitStrength, 0.f, 1.f);
        g.dist = max(g.dist, Math::Lerp(g.dist, fit, s));
    }

    return g;
}

CamWipeOutController::ShotGoal CamWipeOutController::BuildShot4() const
{
    ShotGoal g = BuildShotCommon(m_sideSign, tune.shot4.angleDeg, tune.shot4.pitchLevelDeg, tune.shot4.distFar, tune.shot4.fovFar, tune.shot4.pivotClamp, tune.shot4.attackerBias, tune.shot4.baseVictimWeight, false);

    if (m_vValid)
    {
        const _float fit = FitDistForPair(g, m_aFace, m_vFace);
        const _float s = clamp(tune.shot4.fitStrength, 0.f, 1.f);
        g.dist = max(g.dist, Math::Lerp(g.dist, fit, s));
    }

    return g;
}

void CamWipeOutController::BeginShot(const ShotGoal& to, _float enterSec, _float holdSec, _bool captureFrom)
{
    m_enterSec = max(enterSec, 0.f);
    m_holdSec = max(holdSec, 0.f);

    m_shotTo = to;

    if (captureFrom) CaptureCurAsFrom();

    m_holdFrom = m_shotTo;
    m_holdTo = m_shotTo;

    _float dolly = 0.f;
    if (m_state == State::Shot2_Snap || m_state == State::Shot2_Hold) dolly = tune.shot2.holdDolly;
    if (m_state == State::Shot3_Snap || m_state == State::Shot3_Hold) dolly = tune.shot3.holdDolly;
    if (m_state == State::Shot4_Snap || m_state == State::Shot4_Hold) dolly = tune.shot4.holdDolly;

    m_holdTo.dist = max(0.f, m_holdTo.dist - dolly);

    m_shot4.railActive = false;
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

    ClampShot1AboveGround(from);

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

    if (m_state == State::Shot1_Enter || m_state == State::Shot1_Hold)
        ClampShot1AboveGround(g);

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

    if (m_state == State::Shot4_Hold && m_shot4.railActive)
    {
        const _float dist = Math::Lerp(m_shot4.distFrom, m_shot4.distTo, t);
        const Vector3 pos = OrbitPos(m_shot4.pivotFixed, m_shot4.rotFixed, dist);

        auto orbit = CamDirector()->GetOrbitCam();
        auto cam = CamDirector()->GetOrbitCamComp();

        orbit->SnapFromCamPose(pos, m_shot4.rotFixed);
        cam->Set_FOV(m_holdTo.fov);

        return;
    }

    ShotGoal out{};

    out.pivotExt = m_holdFrom.pivotExt;
    out.yawDeg = m_holdFrom.yawDeg;
    out.pitchDeg = m_holdFrom.pitchDeg;

    out.dist = Math::Lerp(m_holdFrom.dist, m_holdTo.dist, t);
    out.fov = m_holdTo.fov;

    out.yawWeight = m_holdTo.yawWeight;
    out.baseVictimWeight = m_holdTo.baseVictimWeight;

    if (m_state == State::Shot1_Hold)
        ClampShot1AboveGround(out);

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
        BeginShot(BuildShot2(), tune.common.snapShotSec, tune.shot2.holdSec, false);
        SnapTo(m_shotTo);
        m_state = State::Shot2_Hold;
        m_shotElapsed = 0.f;
        return;
    }

    if (m_state == State::Shot2_Hold)
    {
        m_state = State::Shot3_Snap;
        BeginShot(BuildShot3(), tune.common.snapShotSec, tune.shot3.holdSec, false);
        SnapTo(m_shotTo);
        m_state = State::Shot3_Hold;
        m_shotElapsed = 0.f;
        return;
    }

    if (m_state == State::Shot3_Hold)
    {
        m_state = State::Shot4_Snap;
        BeginShot(BuildShot4(), tune.common.snapShotSec, tune.shot4.holdSec, false);
        SnapTo(m_shotTo);

        const Vector3 basePivot = BasePivotWorld(m_holdFrom.baseVictimWeight);
        m_shot4.pivotFixed = basePivot + m_holdFrom.pivotExt;

        m_shot4.rotFixed = YawPitchQuatDeg(m_holdFrom.yawDeg, m_holdFrom.pitchDeg);

        m_shot4.distFrom = m_holdFrom.dist;
        m_shot4.distTo = m_holdTo.dist;

        m_shot4.railActive = true;

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
        const Vector3 attackerFaceNow = attackerSample.valid ? attackerSample.facePivot : m_aFace;

        Vector3 delta = victimSample.facePivot - attackerFaceNow;
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

void CamWipeOutController::Reset()
{
    m_active = false;
    m_state = State::None;

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

    m_victimBlocked = false;

    m_dirXZ = Vector3(0.f, 0.f, 1.f);
    m_sep = 0.f;

    m_shotFrom = {};
    m_shotTo = {};

    m_prevOrbitCaptured = false;
    m_prevOrbit = {};

    m_prevFovCaptured = false;
    m_prevFov = 0.f;

    m_holdFrom = {};
    m_holdTo = {};

    m_shot4 = {};
}

void CamWipeOutController::Begin()
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

    const Vector3 delta2 = victimFace - attackerFace;
    m_sep = Vector3(delta2.x, 0.f, delta2.z).Length();
    m_sep = clamp(m_sep, tune.common.sepMin, tune.common.sepMax);

    m_sideSign = ResolveSideSign();

    m_prevOrbitCaptured = true;
    orbit->CaptureSnapshot(m_prevOrbit);

    m_prevFovCaptured = true;
    m_prevFov = cam->Get_FOV();

    m_active = true;

    m_state = State::Shot1_Enter;
    BeginShot(BuildShot1(), tune.common.enterBlendShot1Sec, tune.shot1.holdSec, true);

    ApplyGoalPose_Snap(m_shotFrom);

    orbit->Lock_Input();
    orbit->ParryMode_Begin();
    orbit->DialogueMode_Begin();
    orbit->DialogueYaw_Clear();
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

    m_shotElapsed += dt;

    UpdatePivots(dt);

    if (m_state == State::Shot1_Enter)
    {
        const _float u = (m_enterSec > 0.f) ? clamp(m_shotElapsed / m_enterSec, 0.f, 1.f) : 1.f;
        const _float t = Math::ApplyEase(tune.common.approachEase, u);

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
