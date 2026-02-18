#include "pch.h"
#include "CamParryController.h"

#include "CamDirector.h"
#include "GameInstance.h"
#include "CharacterController.h"
#include "Transform.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"
#include "Helper_Func.h"

CamParryController::PivotSample CamParryController::SamplePivots(OBJECT_HANDLE h, _float offsetY, _float faceYOffsetMul)
{
    PivotSample s{};
    if (!h.isValid()) return s;

    auto obj = ObjectManager()->Request_Object(h);
    const Vector3 pos = obj->Get_WorldPos();

    auto cc = obj->Get_Component<CCharacterController>();
    if (!cc)
    {
        const _float baseMul = 1.1f;
        const _float topMul = 1.3f;

        _float t = clamp(faceYOffsetMul, 0.f, 1.f);
        const _float faceMul = baseMul + (topMul - baseMul) * t;

        s.basePivot = pos + Vector3(0.f, offsetY, 0.f);
        s.facePivot = pos + Vector3(0.f, offsetY + faceMul * 0.25f, 0.f);
        s.valid = true;
        return s;
    }

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
    out.Normalize();
    return out;
}

_float CamParryController::YawFromDirXZ(const Vector3& dirXZ)
{
    Vector3 d = dirXZ;
    d.y = 0.f;
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

Vector3 CamParryController::BuildReturnPresetCamPos() const
{
    auto attackerObj = ObjectManager()->Request_Object(core.attacker);
    auto attackerTf = attackerObj->Get_Component<CTransform>();

    Vector3 fwd = attackerTf->Dir(STATE::LOOK);
    fwd.y = 0.f;
    fwd.Normalize();

    const Vector3 pivot = BasePivotWorld();
    return pivot + Vector3::Up * 1.f + fwd * -3.6f;
}

CamParryController::ShotGoal CamParryController::BuildExitShot_FromCamPos(const Vector3& pivotWorld, const Vector3& camPosWorld) const
{
    ShotGoal g{};

    Vector3 fwd{}, right{};
    BuildBasis(fwd, right);

    g.pivotExt = ExtFromPivotWorld(pivotWorld);

    Vector3 toPivot = pivotWorld - camPosWorld;
    const float dist = toPivot.Length();
    toPivot /= dist;

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    const _float yawWorldDeg = XMConvertToDegrees(yawRad);
    const _float pitchDeg = XMConvertToDegrees(pitchRad);

    const _float attackerYaw = YawFromDirXZ(fwd);

    g.yawDeg = Math::WrapDeg(yawWorldDeg - attackerYaw);
    g.pitchDeg = pitchDeg;
    g.rollDeg = 0.f;

    g.dist = (_float)dist;
    g.yawWeight = 1.f;

    return g;
}

void CamParryController::ApplyGoalPose_Snap(const ShotGoal& g)
{
    auto orbit = CamDirector()->GetOrbitCam();

    Vector3 fwd, right;
    BuildBasis(fwd, right);

    const Vector3 basePivot = BasePivotWorld();
    const Vector3 pivotWorld = basePivot + right * g.pivotExt.x + Vector3::Up * g.pivotExt.y + fwd * g.pivotExt.z;

    _float fxFwd = 0.f;
    if (core.state == State::Impact) fxFwd = 0.12f;

    shot.fxPointWorld = pivotWorld + fwd * fxFwd;

    const _float attackerYaw = YawFromDirXZ(fwd);
    const _float desiredYawWorld = attackerYaw + g.yawDeg;

    const _float baseYaw = CurCamYawDeg();
    const _float yaw = baseYaw + Math::WrapDeg(desiredYawWorld - baseYaw) * g.yawWeight;

    const Quaternion qPos = YawPitchRollQuatDeg(yaw, g.pitchDeg, 0.f);
    const Vector3 camPos = OrbitPos(pivotWorld, qPos, g.dist);

    const Quaternion qRot = YawPitchRollQuatDeg(yaw, g.pitchDeg, g.rollDeg);
    orbit->SnapFromOrbitPose(pivotWorld, camPos, qRot, g.dist);
}

_float CamParryController::CurCamYawDeg() const
{
    auto orbit = CamDirector()->GetOrbitCam();
    auto tf = orbit->Get_Component<CTransform>();

    Vector3 camLook = tf->Dir(STATE::LOOK);
    camLook.y = 0.f;
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

    const PivotSample attackerSample = SamplePivots(core.attacker, offsetY);
    if (attackerSample.valid)
    {
        const _float t = clamp(dt * 18.f, 0.f, 1.f);
        piv.aBase = Vector3::Lerp(piv.aBase, attackerSample.basePivot, t);
        piv.aFace = Vector3::Lerp(piv.aFace, attackerSample.facePivot, t);
    }

    const OBJECT_HANDLE target = CamDirector()->GetCurTarget();
    const PivotSample targetSample = SamplePivots(target, offsetY);
    if (targetSample.valid)
    {
        if (!piv.hasTBase)
        {
            piv.tBase = targetSample.basePivot;
            piv.hasTBase = true;
        }
        else
        {
            const _float t = clamp(dt * 18.f, 0.f, 1.f);
            piv.tBase = Vector3::Lerp(piv.tBase, targetSample.basePivot, t);
        }
    }

    auto attackerObj = ObjectManager()->Request_Object(core.attacker);
    auto attackerTf = attackerObj->Get_Component<CTransform>();

    Vector3 fwd = attackerTf->Dir(STATE::LOOK);
    fwd.y = 0.f;
    fwd.Normalize();

    const _float tDir = clamp(dt * 18.f, 0.f, 1.f);
    side.dirXZ = Vector3::Lerp(side.dirXZ, fwd, tDir);
    side.dirXZ.y = 0.f;
    side.dirXZ.Normalize();
}

void CamParryController::ClampAboveGround(ShotGoal& g) const
{
    auto attackerObj = ObjectManager()->Request_Object(core.attacker);
    auto attackerCC = attackerObj->Get_Component<CCharacterController>();

    const Vector3 foot = attackerCC->Get_FootPosition();
    const _float minFootY = foot.y;

    const _float minPivotY = minFootY + tune.common.minPivotAboveFootY;
    const _float minCamY = minFootY + tune.common.minCamAboveFootY;

    Vector3 fwd, right;
    BuildBasis(fwd, right);

    const Vector3 basePivot = BasePivotWorld();

    Vector3 pivotWorld = basePivot + right * g.pivotExt.x + Vector3::Up * g.pivotExt.y + fwd * g.pivotExt.z;
    if (pivotWorld.y < minPivotY) g.pivotExt.y += (minPivotY - pivotWorld.y);

    pivotWorld = basePivot + right * g.pivotExt.x + Vector3::Up * g.pivotExt.y + fwd * g.pivotExt.z;

    const _float attackerYaw = YawFromDirXZ(fwd);
    const _float yawWorld = attackerYaw + g.yawDeg;

    const Quaternion q = YawPitchRollQuatDeg(yawWorld, g.pitchDeg, 0.f);
    const Vector3 camPos = OrbitPos(pivotWorld, q, g.dist);

    if (camPos.y < minCamY) g.pivotExt.y += (minCamY - camPos.y);
}

CamParryController::ShotGoal CamParryController::BuildBaseShot_NoLens(_int sideSign) const
{
    ShotGoal g{};

    Vector3 fwd = side.dirXZ;
    fwd.y = 0.f;
    fwd.Normalize();

    Vector3 right = Vector3::Up.Cross(fwd);
    right.y = 0.f;
    right.Normalize();

    Vector3 pivotWorld = piv.aFace + fwd * tune.common.forwardOffset;

    const _float aimY = Math::Lerp(piv.aBase.y, piv.aFace.y, tune.common.pelvisMul);
    pivotWorld.y = aimY + tune.common.pivotYAdd;

    const Vector3 basePivot = piv.aBase;

    const Vector3 extWorld = pivotWorld - basePivot;
    Vector3 extLocal(extWorld.Dot(right), extWorld.y, extWorld.Dot(fwd));

    Vector3 extXZ(extLocal.x, 0.f, extLocal.z);
    extXZ = ClampOffset(extXZ, tune.common.pivotClamp);

    extLocal.x = extXZ.x;
    extLocal.z = extXZ.z;

    g.pivotExt = extLocal;

    const Vector3 baseLook(fwd.x, 0.f, fwd.z);

    const _int lookSign = -sideSign;
    const Vector3 camDir = RotateYDegXZ(baseLook, (_float)lookSign * tune.common.angleDeg);

    const _float attackerYaw = YawFromDirXZ(fwd);
    const _float yawWorld = YawFromDirXZ(camDir) + (_float)lookSign * tune.common.sideYawBiasDeg;

    g.yawDeg = Math::WrapDeg(yawWorld - attackerYaw);
    g.pitchDeg = tune.common.pitchDeg;
    g.rollDeg = 0.f;
    g.yawWeight = 1.f;

    ClampAboveGround(g);

    return g;
}

Vector3 CamParryController::BasePivotWorld() const
{
    constexpr _float kTargetWeight = 0.40f;
    if (!piv.hasTBase) return piv.aBase;
    return Vector3::Lerp(piv.aBase, piv.tBase, kTargetWeight);
}

void CamParryController::CaptureCurAsFrom()
{
    auto orbit = CamDirector()->GetOrbitCam();

    OrbitSnapshot s{};
    orbit->CaptureSnapshot(s);

    Vector3 fwd, right;
    BuildBasis(fwd, right);

    ShotGoal from{};

    from.pivotExt = ExtFromPivotWorld(s.pose.pivotCurWorld);

    const _float attackerYaw = YawFromDirXZ(fwd);
    from.yawDeg = Math::WrapDeg(s.pose.rotCurDeg.x - attackerYaw);

    from.pitchDeg = s.pose.rotCurDeg.y;
    from.rollDeg = 0.f;

    from.dist = s.pose.distCur;
    from.yawWeight = 1.f;

    ClampAboveGround(from);

    shot.shotFrom = from;
}

void CamParryController::ClampEnter_NoDrop(ShotGoal& g) const
{
    Vector3 fwd, right;
    BuildBasis(fwd, right);

    const Vector3 basePivot = BasePivotWorld();
    const Vector3 pivotWorld = basePivot + right * g.pivotExt.x + Vector3::Up * g.pivotExt.y + fwd * g.pivotExt.z;

    const _float attackerYaw = YawFromDirXZ(fwd);
    const _float yawWorld = attackerYaw + g.yawDeg;

    auto EvalCamY = [&](float pitchDeg)
        {
            const Quaternion q = YawPitchRollQuatDeg(yawWorld, pitchDeg, g.rollDeg);
            const Vector3 camPos = OrbitPos(pivotWorld, q, g.dist);
            return camPos.y;
        };

    const _float y0 = EvalCamY(g.pitchDeg);
    if (y0 >= piv.enterCamY) return;

    const _float yPlus = EvalCamY(g.pitchDeg + 1.f);
    const _bool plusRaises = (yPlus > y0);

    _float lo = plusRaises ? g.pitchDeg : -89.f;
    _float hi = plusRaises ? 89.f : g.pitchDeg;

    for (_int i = 0; i < 16; ++i)
    {
        const _float mid = (lo + hi) * 0.5f;
        const _float yMid = EvalCamY(mid);

        if (yMid < piv.enterCamY) lo = mid;
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

    Vector3 fwd, right;
    BuildBasis(fwd, right);

    ShotGoal g{};

    g.pivotExt = ExtFromPivotWorld(s.pose.pivotCurWorld);

    const _float attackerYaw = YawFromDirXZ(fwd);
    g.yawDeg = Math::WrapDeg(s.pose.rotCurDeg.x - attackerYaw);
    g.pitchDeg = s.pose.rotCurDeg.y;
    g.rollDeg = 0.f;

    g.dist = s.pose.distCur;
    g.yawWeight = 1.f;

    ClampAboveGround(g);

    shot.impactBase = g;
    shot.impactCaptured = true;
}

CamParryController::ShotGoal CamParryController::BuildImpactShot(_int sideSign, _float close01, _float u) const
{
    ShotGoal g = shot.impactBase;

    close01 = clamp(close01, 0.f, 1.f);
    u = clamp(u, 0.f, 1.f);

    Vector3 fwd, right;
    BuildBasis(fwd, right);

    _float punch = tune.impact.punchDistDelta;

    if (core.beginWasChain)
    {
        const _float alreadyCloser = max(0.f, core.chainRefDist - shot.impactBase.dist);
        punch = max(0.f, punch - alreadyCloser);
    }

    const _float distEnd = max(0.f, shot.impactBase.dist - punch);
    g.dist = Math::Lerp(shot.impactBase.dist, distEnd, close01);

    auto attackerObj = ObjectManager()->Request_Object(core.attacker);
    auto attackerCC = attackerObj->Get_Component<CCharacterController>();

    const Vector3 foot = attackerCC->Get_FootPosition();

    const _float attackerYaw = YawFromDirXZ(fwd);
    const _float yawWorldBase = attackerYaw + shot.impactBase.yawDeg;

    const Vector3 basePivot = BasePivotWorld();
    const Vector3 pivotWorldBase = basePivot + right * shot.impactBase.pivotExt.x + Vector3::Up * shot.impactBase.pivotExt.y + fwd * shot.impactBase.pivotExt.z;

    const Quaternion qStart = YawPitchRollQuatDeg(yawWorldBase, shot.impactBase.pitchDeg, 0.f);
    const _float startCamY = OrbitPos(pivotWorldBase, qStart, shot.impactBase.dist).y;

    const _float targetCamYFixed = foot.y + tune.impact.endCamAboveFootY;

    const _float mix = clamp(tune.impact.targetCamYMix, 0.f, 1.f);
    _float targetCamY = Math::Lerp(startCamY, targetCamYFixed, mix);

    if (core.beginWasChain) targetCamY = max(targetCamY, startCamY);

    g.pivotExt = shot.impactBase.pivotExt;

    if (!core.beginWasChain)
        g.pivotExt.y -= tune.impact.pivotDropY * close01;

    const Vector3 pivotWorld = basePivot + right * g.pivotExt.x + Vector3::Up * g.pivotExt.y + fwd * g.pivotExt.z;

    auto EvalCamY = [&](float pitchDeg)
        {
            const _float yawWorld = attackerYaw + g.yawDeg;
            const Quaternion q = YawPitchRollQuatDeg(yawWorld, pitchDeg, 0.f);
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
    g.pitchDeg = Math::Lerp(shot.impactBase.pitchDeg, pitchEnd, close01);

    const _float tRoll = Math::ApplyEase(tune.impact.rollEase, u);
    g.rollDeg = (_float)sideSign * tune.impact.rollMaxDeg * tRoll * tune.impact.rollArcMul;

    ClampAboveGround(g);

    return g;
}

void CamParryController::ComputeSideFromCam()
{
    Vector3 fwd, right;
    BuildBasis(fwd, right);

    Vector3 rel = CurCamPosWorld() - BasePivotWorld();
    rel.y = 0.f;

    side.isLeft = (rel.Dot(right) < 0.f);
    side.sideSign = side.isLeft ? -1 : 1;
}

string CamParryController::BuildParryKey() const
{
    const CHARACTER charaName = CamDirector()->GetCharacterName();
    string key;

    key = "Parry/";
    key += Helper::EnumToString(charaName);
    key += side.isLeft ? "_Left" : "_Right";
    return key;
}

void CamParryController::BuildBasis(Vector3& outFwd, Vector3& outRight) const
{
    outFwd = side.dirXZ;
    outFwd.y = 0.f;
    outFwd.Normalize();

    outRight = Vector3::Up.Cross(outFwd);
    outRight.y = 0.f;
    outRight.Normalize();
}

Vector3 CamParryController::PivotWorldFromExt(const Vector3& ext) const
{
    Vector3 fwd, right;
    BuildBasis(fwd, right);

    const Vector3 basePivot = BasePivotWorld();
    return basePivot + right * ext.x + Vector3::Up * ext.y + fwd * ext.z;
}

Vector3 CamParryController::ExtFromPivotWorld(const Vector3& pivotWorld) const
{
    Vector3 fwd, right;
    BuildBasis(fwd, right);

    const Vector3 basePivot = BasePivotWorld();
    const Vector3 extWorld = pivotWorld - basePivot;

    return Vector3(extWorld.Dot(right), extWorld.y, extWorld.Dot(fwd));
}

_float CamParryController::EvalImpactFovOffset(_float u, _float close01, _float baseFov) const
{
    u = clamp(u, 0.f, 1.f);
    close01 = clamp(close01, 0.f, 1.f);

    const _float count = (_float)max(1, tune.impact.fovWaveCount);
    const _float phase = 2.f * XM_PI * count * u;
    const _float osc = sinf(phase);

    const _float rampIn = clamp(u / 0.10f, 0.f, 1.f);
    const _float ramp = Math::ApplyEase(EaseType::OutSine, rampIn);

    const _float bias = -tune.impact.fovBiasDeg * close01;

    const _float amp = tune.impact.fovWaveAmpDeg * close01 * ramp;
    const _float wave = amp * osc;

    _float offset = bias + wave;

    const _float minFov = 8.f;
    const _float maxFov = 120.f;

    if (baseFov + offset < minFov) offset = minFov - baseFov;
    if (baseFov + offset > maxFov) offset = maxFov - baseFov;

    return offset;
}

void CamParryController::ApplyImpactFov(_float u, _float close01)
{
    const _float baseFov = CameraManager()->GetFov() - lens.fovAppliedOffset;
    const _float desiredOffset = EvalImpactFovOffset(u, close01, baseFov);

    const _float delta = desiredOffset - lens.fovAppliedOffset;
    if (delta != 0.f) CameraManager()->SetFov(delta, 0.f);

    lens.fovAppliedOffset = desiredOffset;
}

void CamParryController::BeginRecoverFov()
{
    lens.recoverFovActive = true;
    lens.recoverFovElapsed = 0.f;
    lens.recoverFovFrom = lens.fovAppliedOffset;
}

void CamParryController::UpdateRecoverFov(_float dt)
{
    if (!lens.recoverFovActive) return;

    lens.recoverFovElapsed += dt;

    const _float dur = max(tune.impact.recoverFovSec, 0.0001f);
    const _float u = clamp(lens.recoverFovElapsed / dur, 0.f, 1.f);
    const _float t = Math::ApplyEase(tune.impact.recoverFovEase, u);

    const _float baseOffset = Math::Lerp(lens.recoverFovFrom, 0.f, t);

    const _float count = (_float)max(1, tune.impact.fovWaveCount);
    const _float phase = 2.f * XM_PI * count * u;
    const _float osc = sinf(phase);

    const _float k = 3.5f;
    const _float decay = expf(-k * u);

    const _float amp = tune.impact.fovWaveAmpDeg * 0.65f;
    const _float settle = amp * osc * decay;

    const _float desiredOffset = baseOffset + settle;

    const _float delta = desiredOffset - lens.fovAppliedOffset;
    if (delta != 0.f) CameraManager()->SetFov(delta, 0.f);

    lens.fovAppliedOffset = desiredOffset;

    if (u >= 1.f) lens.recoverFovActive = false;
}

_bool CamParryController::IsChainParry() const
{
    return BattleSystem()->GetBattlePlayer()->Is_ChainParry();
}

void CamParryController::Reset()
{
    if (lens.fovAppliedOffset != 0.f)
        CameraManager()->SetFov(-lens.fovAppliedOffset, 0.f);

    core.active = false;
    core.state = State::None;
    core.elapsed = 0.f;

    core.attacker.Reset();

    side.sideSign = 1;
    side.isLeft = false;
    side.dirXZ = Vector3(0.f, 0.f, 1.f);

    piv.aBase = Vector3::Zero;
    piv.aFace = Vector3::Zero;

    piv.tBase = Vector3::Zero;
    piv.hasTBase = false;

    piv.enterCamY = 0.f;

    shot.shotFrom = {};
    shot.shotTo = {};

    shot.impactBase = {};
    shot.impactCaptured = false;

    shot.fxPointWorld = Vector3::Zero;

    wait.seqKey.clear();
    wait.seqStarted = false;

    shot.holdShot = {};
    shot.holdActive = false;

    lens.fovSaved = 0.f;
    lens.fovAppliedOffset = 0.f;

    lens.recoverFovActive = false;
    lens.recoverFovElapsed = 0.f;
    lens.recoverFovFrom = 0.f;

    core.beginWasChain = false;
    core.chainRefDist = 0.f;

    exit.returnLockBlend = false;
    exit.returnLockHandle.Reset();

    exit.exitTo = {};
    exit.exitPivotWorld = Vector3::Zero;
    exit.exitCamPosTo = Vector3::Zero;
    exit.exitSec = 0.f;

    exit.exitPivotFrom = Vector3::Zero;
    exit.exitCamPosFrom = Vector3::Zero;
}

void CamParryController::Begin()
{
    if (core.active && core.state != State::WaitEnd) return;

    const _bool continuingChain = core.active && core.state == State::WaitEnd && IsChainReentryOpen() && core.beginWasChain;
    const _float prevChainRefDist = core.chainRefDist;

    Reset();

    core.beginWasChain = IsChainParry();
    const _bool chain = core.beginWasChain;

    core.attacker = CamDirector()->GetCurHandle();
    auto orbit = CamDirector()->GetOrbitCam();

    CamDirector()->SetTarget(core.attacker);

    lens.fovSaved = CameraManager()->GetFov();
    lens.fovAppliedOffset = 0.f;

    const _float offsetY = orbit->GetOffsetY();

    const PivotSample attackerSample = SamplePivots(core.attacker, offsetY);
    piv.aBase = attackerSample.basePivot;
    piv.aFace = attackerSample.facePivot;

    const OBJECT_HANDLE target = CamDirector()->GetCurTarget();
    const PivotSample targetSample = SamplePivots(target, offsetY);
    if (targetSample.valid)
    {
        piv.tBase = targetSample.basePivot;
        piv.hasTBase = true;
    }

    auto attackerObj = ObjectManager()->Request_Object(core.attacker);
    auto attackerTf = attackerObj->Get_Component<CTransform>();

    Vector3 attackerFwd = attackerTf->Dir(STATE::LOOK);
    attackerFwd.y = 0.f;
    attackerFwd.Normalize();

    side.dirXZ = attackerFwd;

    ComputeSideFromCam();

    CaptureCurAsFrom();
    piv.enterCamY = CurCamPosWorld().y;

    const _float startDist = shot.shotFrom.dist;

    if (chain)
    {
        if (continuingChain && prevChainRefDist > 0.f) core.chainRefDist = prevChainRefDist;
        else core.chainRefDist = startDist;

        if (startDist > core.chainRefDist) core.chainRefDist = startDist;
    }
    else core.chainRefDist = startDist;

    shot.shotTo = BuildBaseShot_NoLens(side.sideSign);
    shot.shotTo.dist = startDist;

    const _int lookSign = -side.sideSign;
    shot.shotTo.yawDeg = Math::WrapDeg(shot.shotTo.yawDeg + (_float)lookSign * tune.impact.impactStartYawExtraDeg);

    core.active = true;
    core.state = State::Enter;
    core.elapsed = 0.f;

    ApplyGoalPose_Snap(shot.shotFrom);

    orbit->ParryMode_Begin();
}


void CamParryController::End()
{
    if (!core.active) return;

    BeginRecoverFov();

    auto orbit = CamDirector()->GetOrbitCam();

    OrbitSnapshot snap{};
    orbit->CaptureSnapshot(snap);

    exit.returnLockHandle = snap.lock.handle;
    exit.returnLockBlend = exit.returnLockHandle.isValid() && (snap.lock.active || snap.lockBlend.active || snap.lockBlend.weight > 0.f);

    if (exit.returnLockBlend)
    {
        exit.exitPivotWorld = BasePivotWorld();
        exit.exitCamPosTo = BuildReturnPresetCamPos();

        exit.exitTo = BuildExitShot_FromCamPos(exit.exitPivotWorld, exit.exitCamPosTo);

        exit.exitPivotFrom = snap.pose.pivotCurWorld;
        exit.exitCamPosFrom = CurCamPosWorld();

        exit.exitSec = 1.f;

        orbit->Lock_ReenterBlend(exit.exitSec);

        core.state = State::ExitBlend;
        core.elapsed = 0.f;

        wait.seqKey.clear();
        wait.seqStarted = false;

        return;
    }

    CameraManager()->Set_BlendEase(tune.impact.recoverRollEase);

    wait.seqKey = BuildParryKey();
    CamDirector()->RequestSequence(wait.seqKey);

    core.state = State::WaitEnd;
    core.elapsed = 0.f;
    wait.seqStarted = false;
}


void CamParryController::Update(_float dt)
{
    if (!core.active) return;

    core.elapsed += dt;

    if (core.state == State::Enter || core.state == State::Impact)
        UpdatePivots(dt);

    if (core.state == State::Enter)
    {
        const _float u = (tune.common.enterSec > 0.f) ? clamp(core.elapsed / tune.common.enterSec, 0.f, 1.f) : 1.f;
        const _float t = Math::ApplyEase(tune.common.approachEase, u);

        ApplyInterpolated_Enter(shot.shotFrom, shot.shotTo, t);

        if (u >= 1.f)
        {
            CaptureCurAsImpactBase();
            core.state = State::Impact;
            core.elapsed = 0.f;
        }
        return;
    }

    if (core.state == State::Impact)
    {
        if (!shot.impactCaptured) CaptureCurAsImpactBase();

        const _float u = (tune.common.impactSec > 0.f) ? clamp(core.elapsed / tune.common.impactSec, 0.f, 1.f) : 1.f;
        const _float close01 = Math::ApplyEase(tune.common.impactEase, u);

        ShotGoal g = BuildImpactShot(side.sideSign, close01, u);
        ApplyGoalPose_Snap(g);

        ApplyImpactFov(u, close01);

        if (u >= 1.f)
        {
            shot.holdShot = g;
            shot.holdActive = true;

            End();
            return;
        }
        return;
    }

    if (core.state == State::ExitBlend)
    {
        UpdateRecoverFov(dt);

        auto orbit = CamDirector()->GetOrbitCam();
        orbit->Lock_BlendUpdate_External(dt);

        const _float dur = max(exit.exitSec, 0.0001f);
        const _float u = clamp(core.elapsed / dur, 0.f, 1.f);
        const _float t = Math::ApplyEase(EaseType::InOutSine, u);

        Vector3 pivotWorld = Vector3::Lerp(exit.exitPivotFrom, exit.exitPivotWorld, t);
        Vector3 camPosWorld = Vector3::Lerp(exit.exitCamPosFrom, exit.exitCamPosTo, t);

        auto attackerObj = ObjectManager()->Request_Object(core.attacker);
        auto attackerCC = attackerObj->Get_Component<CCharacterController>();
        const Vector3 foot = attackerCC->Get_FootPosition();

        const _float minPivotY = foot.y + tune.common.minPivotAboveFootY;
        const _float minCamY = foot.y + tune.common.minCamAboveFootY;

        _float lift = 0.f;
        if (pivotWorld.y < minPivotY) lift = max(lift, minPivotY - pivotWorld.y);
        if (camPosWorld.y < minCamY) lift = max(lift, minCamY - camPosWorld.y);

        pivotWorld.y += lift;
        camPosWorld.y += lift;

        Vector3 toPivot = pivotWorld - camPosWorld;
        const float dist = toPivot.Length();
        toPivot /= dist;

        const _float yawPivotWorldDeg = XMConvertToDegrees(atan2f(toPivot.x, toPivot.z));

        OrbitLockEval lockRes = orbit->Lock_Eval_External(dt, yawPivotWorldDeg, (_float)dist);

        Vector3 lookAt = pivotWorld;
        if (lockRes.weight > 0.f) lookAt = Vector3::Lerp(pivotWorld, lockRes.focusPos, lockRes.weight);

        Vector3 toLook = lookAt - camPosWorld;
        const float lookDist = toLook.Length();
        toLook /= lookDist;

        const _float yawWorldDeg = XMConvertToDegrees(atan2f(toLook.x, toLook.z));
        const _float pitchDeg = XMConvertToDegrees(asinf(clamp(-toLook.y, -1.f, 1.f)));
        const _float rollDeg = Math::Lerp(shot.holdShot.rollDeg, 0.f, t);

        const Quaternion qRot = YawPitchRollQuatDeg(yawWorldDeg, pitchDeg, rollDeg);

        orbit->SnapFromOrbitPose(pivotWorld, camPosWorld, qRot, (_float)dist);

        if (u >= 1.f)
        {
            orbit->ParryMode_ResumeSync();
            orbit->ParryMode_End();

            core.state = State::WaitEnd;
            core.elapsed = 0.f;

            shot.holdActive = false;
            wait.seqKey.clear();
            wait.seqStarted = true;
        }
        return;
    }


    if (core.state == State::WaitEnd)
    {
        UpdateRecoverFov(dt);

        if (exit.returnLockBlend)
        {
            if (lens.recoverFovActive) return;

            if (lens.fovAppliedOffset != 0.f)
                CameraManager()->SetFov(-lens.fovAppliedOffset, 0.f);
            lens.fovAppliedOffset = 0.f;

            if (!core.beginWasChain) core.chainRefDist = 0.f;

            Reset();
            return;
        }

        if (shot.holdActive && core.elapsed < tune.impact.recoverRollSec)
            ApplyGoalPose_Snap(shot.holdShot);

        if (!wait.seqStarted)
        {
            if (CamDirector()->IsPlaying(wait.seqKey))
                wait.seqStarted = true;
            return;
        }

        if (CamDirector()->IsPlaying(wait.seqKey)) return;
        if (lens.recoverFovActive) return;

        if (lens.fovAppliedOffset != 0.f)
            CameraManager()->SetFov(-lens.fovAppliedOffset, 0.f);
        lens.fovAppliedOffset = 0.f;

        auto orbit = CamDirector()->GetOrbitCam();
        orbit->ParryMode_ResumeSync();
        orbit->ParryMode_End();

        if (!core.beginWasChain) core.chainRefDist = 0.f;

        Reset();
        return;
    }
}

_bool CamParryController::IsChainReentryOpen() const
{
    if (!core.active) return false;
    if (core.state != State::WaitEnd) return false;
    if (lens.recoverFovActive) return false;
    if (core.elapsed < tune.impact.recoverRollSec) return false;
    return true;
}