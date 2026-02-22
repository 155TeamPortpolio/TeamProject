#include "pch.h"
#include "OrbitCam.h"
#include "CamOcclusionTracker.h"
#include "ICamCollidable.h"
#include "Enemy.h"
#include "BattleSystem.h"   
#include "BattlePlayer.h"
#include "CamSwitchController.h"
// Engine
#include "GameInstance.h"
#include "Helper_Func.h"
#include "PhysicsSystem.h"
#include "CamDirector.h"
// Component
#include "Character.h"
#include "CharacterController.h"
#include "EventListener.h"
#include "ObjectContainer.h"
#include "Animator3D.h"

namespace
{
    _float ExpAlphaSpeed(_float speed, _float dt)
    {
        const _float tau = 1.f / max(speed, 0.0001f);
        return Math::ExpAlpha(tau, dt);
    }

    Quaternion YawPitchQuat(const Vector2& rotDeg)
    {
        const float yawRad = XMConvertToRadians(rotDeg.x);
        const float pitchRad = XMConvertToRadians(rotDeg.y);
        return Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);
    }

    Vector3 OrbitCamPos(const Vector3& pivot, const Quaternion& q, float dist)
    {
        const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
        return pivot + backDir * dist;
    }

    Vector3 FootPos(OBJECT_HANDLE h)
    {
        auto obj = ObjectManager()->Request_Object(h);
        if (!obj)
            return Vector3{};
        auto cc = obj->Get_Component<CCharacterController>();
        return cc->Get_FootPosition();
    }

    Vector3 BasePivotPos(OBJECT_HANDLE h, _float offsetY)
    {
        auto obj = ObjectManager()->Request_Object(h);
        if (!obj)
            return Vector3{};
        auto cc = obj->Get_Component<CCharacterController>();
        const Vector3 foot = cc->Get_FootPosition();
        return foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + offsetY, 0.f);
    }

    void BuildOrbitBasis(const Vector2& rotDeg, Vector3& outLook, Vector3& outRight, Quaternion& outQ)
    {
        outQ = YawPitchQuat(rotDeg);
        const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), outQ);
        outLook = -backDir;
        outRight = Vector3::Transform(Vector3(1.f, 0.f, 0.f), outQ);
    }

    Vector3 LockPivotPos(OBJECT_HANDLE h, _float offsetY)
    {
        auto obj = ObjectManager()->Request_Object(h);
        if (!obj)
            return Vector3{};
        CEnemy::ENEMY_CLASS enemyClass = static_cast<CEnemy*>(obj)->GetEnemyClass();

        auto GetBipPivot = [&]() -> Vector3
            {
                auto anim = obj->Get_Component<CAnimator3D>();
                if (anim)
                {
                    _float4x4 m{};
                    if (anim->Get_BipWorld(&m))
                    {
                        const Vector3 bip(m._41, m._42, m._43);
                        return bip + Vector3(0.f, offsetY, 0.f);
                    }
                }

                auto tf = obj->Get_Component<CTransform>();
                return Vector3(tf->Get_WorldPos()) + Vector3(0.f, offsetY, 0.f);
            };

        auto GetCcPivot = [&]() -> Vector3
            {
                auto cc = obj->Get_Component<CCharacterController>();
                if (cc)
                {
                    const Vector3 foot = cc->Get_FootPosition();
                    return foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + offsetY, 0.f);
                }

                auto tf = obj->Get_Component<CTransform>();
                return Vector3(tf->Get_WorldPos()) + Vector3(0.f, offsetY, 0.f);
            };

        if (enemyClass == CEnemy::ENEMY_CLASS::BOSS)
            return GetBipPivot();

        return GetCcPivot();
    }
}

void COrbitCam::Awake()
{
    auto cc = Get_Component<CCharacterController>();
    cc->Resize(0.3f, 0.3f);
    cc->Set_GravityEnabled(false);
    cc->Set_StepOffset(0.f);
    cc->Set_SlopeLimit(89.f);
    cc->Set_MinMoveDist(0.1f);
}

HRESULT COrbitCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CCharacterController>();
    Add_Component<CEventListener>();

    m_pose.rotGoalDeg = Vector2(0.f, m_prof.startPitchDeg);
    m_pose.rotCurDeg = m_pose.rotGoalDeg;

    m_pose.distWanted = m_prof.startDist;
    m_pose.distGoal = m_pose.distWanted;
    m_pose.distCur = m_pose.distGoal;

    m_pose.pivotGoalWorld = Vector3::Zero;
    m_pose.pivotCurWorld = m_pose.pivotGoalWorld;

    m_pose.pivotInternalOffset = Vector3::Zero;
    m_pose.pivotExternalOffset = Vector3::Zero;

    Lock_Reset();
    AutoYaw_OnTarget();
    Switch_Reset();

    m_freezeMode = FreezeMode::None;
    m_freeze = 0.f;

    ClampTargets();
    ExternalHandoff_Reset();
    return S_OK;
}

HRESULT COrbitCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CEventListener>()->Add_Listener<TARGET_LOCK_DESC>([&](TARGET_LOCK_DESC desc)
        {
            if (!desc.bLock)
            {
                ClearLockOn();
                return;
            }

            if (!desc.tHandle.isValid()) return;
            SetLockOn(desc.tHandle);
        });

    return S_OK;
}

void COrbitCam::SetTarget(OBJECT_HANDLE h)
{
    if (!h.isValid()) return;
    if (h == m_target) return;

    const float keepWantedDist = m_pose.distWanted;

    auto ClampDist = [&]
        {
            m_pose.distWanted = clamp(keepWantedDist, m_prof.distMin, m_prof.distMax);
            m_pose.distGoal = clamp(m_pose.distGoal, m_prof.distMin, m_prof.distMax);
            m_pose.distCur = clamp(m_pose.distCur, m_prof.distMin, m_prof.distMax);
        };

    if (!m_target.isValid())
    {
        m_target = h;

        Switch_Reset();
        AutoYaw_OnTarget();

        m_pose.pivotInternalOffset = Vector3::Zero;
        m_pose.rotGoalDeg = m_pose.rotCurDeg;

        const Vector3 p = GetPivotTargetPos();
        PivotStab_Reset(p);
        m_pose.pivotGoalWorld = p;
        m_pose.pivotCurWorld = p;

        ClampDist();
        ClampTargets();
        return;
    }

    const Vector3 holdPivotWorld = m_pose.pivotCurWorld;

    m_target = h;
    AutoYaw_OnTarget();

    Switch_Begin(holdPivotWorld);
    m_pose.rotGoalDeg = m_pose.rotCurDeg;

    const Vector3 p = GetPivotTargetPos();
    PivotStab_Reset(p);

    ClampDist();
    ClampTargets();
}

void COrbitCam::ClearTarget()
{
    m_target.Reset();
    Lock_Reset();
    Switch_Reset();

    m_pivotStab = {};
    ExternalHandoff_Reset();
}

void COrbitCam::SetLockOn(OBJECT_HANDLE h)
{
    if (!h.isValid()) return;

    if (!Lock_Active())
    {
        Lock_Enter(h, m_pose.distGoal);
        return;
    }

    if (h == Lock_Handle()) return;

    Lock_Switch(h);

    m_lockFocus = m_pose.pivotCurWorld;
    m_hasLockFocus = true;
}

void COrbitCam::ClearLockOn()
{
    if (!Lock_On()) return;

    Lock_Exit();
    AutoYaw_OnInput();
}

void COrbitCam::Lock_ReenterBlend(_float blendInSec)
{
    Lock_ReenterBlend(blendInSec, m_prof.lockBlendInEase);
}

void COrbitCam::Lock_ReenterBlend(_float blendInSec, EaseType ease)
{
    if (!m_lock.active) return;
    if (!m_lock.handle.isValid()) return;

    m_lock.savedDist = m_pose.distCur;

    m_lockBlend.active = true;
    m_lockBlend.entering = true;
    m_lockBlend.elapsed = 0.f;
    m_lockBlend.duration = max(blendInSec, 0.0001f);
    m_lockBlend.ease = ease;
    m_lockBlend.weight = 0.f;
    m_lockBlend.holdFirstFrame = true;

    m_lockFocus = m_pose.pivotCurWorld;
    m_hasLockFocus = true;
}

void COrbitCam::ReturnPreset_Begin(const Vector3& pivotWorld, const Vector3& camPosTo, _float sec, EaseType ease)
{
    m_returnPreset.active = true;
    m_returnPreset.elapsed = 0.f;
    m_returnPreset.duration = max(sec, 0.0001f);
    m_returnPreset.ease = ease;

    m_returnPreset.pivotWorld = pivotWorld;

    m_returnPreset.camPosFrom = m_pTransform->Get_WorldPos();
    m_returnPreset.camPosTo = camPosTo;
}

Vector3 COrbitCam::ReturnPreset_EvalCamPos(_float dt, const Vector3& fallbackCamPos)
{
    if (!m_returnPreset.active) return fallbackCamPos;

    m_returnPreset.elapsed += dt;

    _float u = m_returnPreset.elapsed / m_returnPreset.duration;
    if (u >= 1.f)
    {
        m_returnPreset.active = false;
        return m_returnPreset.camPosTo;
    }

    u = clamp(u, 0.f, 1.f);
    const _float t = Math::ApplyEase(m_returnPreset.ease, u);

    return Vector3::Lerp(m_returnPreset.camPosFrom, m_returnPreset.camPosTo, t);
}

void COrbitCam::CaptureSnapshot(OrbitSnapshot& out) const
{
    out.pose = m_pose;
    out.lock = m_lock;
    out.lockBlend = m_lockBlend;
    out.autoYaw = m_autoYaw;
    out.sw = m_switch;
    out.target = m_target;
}

void COrbitCam::RestoreSnapshot(const OrbitSnapshot& s)
{
    m_target = s.target;

    m_pose = s.pose;

    m_lock = s.lock;
    m_lockBlend = s.lockBlend;
    m_autoYaw = s.autoYaw;
    m_switch = s.sw;

    m_lockFocus = {};
    m_hasLockFocus = false;

    SyncPivot();

    Lock_BlendUpdate(0.f);

    OrbitLockEval lockRes{};
    if (Lock_On())
        lockRes = EvalLock(0.f, m_pose.rotCurDeg.x, m_pose.distCur);

    ApplyPose(0.f, lockRes);
}

void COrbitCam::SyncFromCurTransform()
{
    SyncPivot();

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& p = cc->Get_Controller()->getPosition();
    const Vector3 camPos((float)p.x, (float)p.y, (float)p.z);

    Vector3 toPivot = m_pose.pivotCurWorld - camPos;
    const float dist = toPivot.Length();
    toPivot /= dist;

    Vector2 rotDeg{};
    CalcRotDeg(toPivot, rotDeg);

    m_pose.rotCurDeg = rotDeg;
    m_pose.rotGoalDeg = rotDeg;

    m_pose.distCur = dist;
    m_pose.distGoal = dist;
    m_pose.distWanted = dist;

    ClampTargets();

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(m_pose.pivotCurWorld.x, m_pose.pivotCurWorld.y, m_pose.pivotCurWorld.z, 1.f));
}

void COrbitCam::SnapFromCamPose(const Vector3& pos, const Quaternion& rot)
{
    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(Vector4(pos.x, pos.y, pos.z, 1.f));

    Vector3 footWorld{};
    Vector3 basePivotWorld{};
    GetBasePivot(m_target, footWorld, basePivotWorld);

    SnapPose(pos, rot, basePivotWorld);

    m_yawDeltaCapDeg = m_prof.yawDeltaCapDeg;
    m_pitchDeltaCapDeg = m_prof.pitchDeltaCapDeg;

    ClampTargets();

    m_pTransform->Set_WorldPos(Vector4(pos.x, pos.y, pos.z, 1.f));
    m_pTransform->LookAt(Vector4(m_pose.pivotCurWorld.x, m_pose.pivotCurWorld.y, m_pose.pivotCurWorld.z, 1.f));
}

void COrbitCam::SnapFromOrbitPose(const Vector3& pivotWorld, const Vector3& camPos, const Quaternion& camRot, _float dist)
{
    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    const Vector3 basePivotWorld = GetBasePivotTargetPos(m_target);

    Switch_Reset();

    m_pose.pivotExternalOffset = Vector3::Zero;
    m_pose.pivotInternalOffset = pivotWorld - basePivotWorld;

    PivotStab_Reset(pivotWorld);

    m_pose.pivotCurWorld = pivotWorld;
    m_pose.pivotGoalWorld = pivotWorld;

    Vector3 toPivot = pivotWorld - camPos;
    const float rawDist = toPivot.Length();
    toPivot /= rawDist;

    Vector2 rotDeg{};
    CalcRotDeg(toPivot, rotDeg);

    m_pose.rotCurDeg = rotDeg;
    m_pose.rotGoalDeg = rotDeg;

    dist = clamp(dist, m_prof.distMin, m_prof.distMax);

    m_pose.distCur = dist;
    m_pose.distGoal = dist;
    m_pose.distWanted = dist;

    m_yawDeltaCapDeg = m_prof.yawDeltaCapDeg;
    m_pitchDeltaCapDeg = m_prof.pitchDeltaCapDeg;

    ClampTargets();

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->Set_WorldQuaternion(camRot);
}

void COrbitCam::SnapFromExternalPose(const Vector3& pivotWorld, const Vector3& camPos, const Quaternion& camRot, _float dist)
{
    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->Set_WorldQuaternion(camRot);

    const Vector3 basePivotWorld = GetBasePivotTargetPos(m_target);

    Switch_Reset();

    m_pose.pivotExternalOffset = Vector3::Zero;
    m_pose.pivotInternalOffset = pivotWorld - basePivotWorld;

    PivotStab_Reset(pivotWorld);

    m_pose.pivotCurWorld = pivotWorld;
    m_pose.pivotGoalWorld = pivotWorld;

    Vector3 toPivot = pivotWorld - camPos;
    const float rawDist = toPivot.Length();
    if (rawDist > 0.f) toPivot /= rawDist;

    Vector2 rotDeg{};
    CalcRotDeg(toPivot, rotDeg);

    m_pose.rotCurDeg = rotDeg;
    m_pose.rotGoalDeg = rotDeg;

    dist = clamp(dist, m_prof.distMin, m_prof.distMax);

    m_pose.distCur = dist;
    m_pose.distGoal = dist;
    m_pose.distWanted = dist;

    m_yawDeltaCapDeg = m_prof.yawDeltaCapDeg;
    m_pitchDeltaCapDeg = m_prof.pitchDeltaCapDeg;

    ClampTargets();
}

_bool COrbitCam::Freeze_SkipUpdate(_float dt)
{
    if (!m_target.isValid()) return true;

    if (m_freeze > 0.f)
    {
        m_freeze -= dt;
        return true;
    }

    if (Freeze_On()) return true;

    if (Lock_On() && !Lock_Handle().isValid())
        ClearLockOn();

    return false;
}

void COrbitCam::UpdateSwitch(_float dt)
{
    if (!Switch_Active()) return;

    const Vector3 basePivotNow = GetBasePivotTargetPos(m_target);
    m_pose.pivotInternalOffset = Switch_EvalOffset(dt, basePivotNow);
}

void COrbitCam::ApplyInput(_float dt)
{
    const float lockW = Lock_Weight();
    const OrbitInputEval inRes = EvalInput(dt, lockW);

    m_pose.rotGoalDeg.x += inRes.yawDeltaDeg;
    m_pose.rotGoalDeg.y += inRes.pitchDeltaDeg;
    m_pose.distWanted -= inRes.zoomDelta;
}

OrbitLockEval COrbitCam::ApplyLock(_float dt)
{
    Lock_BlendUpdate(dt);

    OrbitLockEval lockRes{};
    if (!Lock_On()) return lockRes;

    if (Freeze_On()) return lockRes;

    lockRes = EvalLock(dt, m_pose.rotGoalDeg.x, m_pose.distWanted);

    if (ExternalHandoff_LockDistGraceOn())
        lockRes.hasDist = false;

    m_pose.rotGoalDeg.x += lockRes.yawAddDeg;

    return lockRes;
}

void COrbitCam::ApplyAutoYaw(_float dt, const OrbitLockEval& lockRes)
{
    if (Freeze_On()) return;
    if (lockRes.weight > 0.f) return;

    const Vector3 foot = GetFoot();
    const Vector3 camLook = m_pTransform->Dir(STATE::LOOK);
    const Vector3 camRight = m_pTransform->Dir(STATE::RIGHT);

    m_pose.rotGoalDeg.x += EvalAutoYaw(dt, foot, camLook, camRight, m_pose.rotGoalDeg.x);
}

void COrbitCam::Priority_Update(_float dt)
{
    if (Freeze_SkipUpdate(dt)) return;
    ExternalHandoff_Update(dt);
    UpdateSwitch(dt);

    const Vector3 rawPivot = GetPivotTargetPos();
    m_pose.pivotGoalWorld = PivotStab_Eval(dt, rawPivot);

    ApplyInput(dt);

    const OrbitLockEval lockRes = ApplyLock(dt);

    ClampTargets();

    _float evalDistWanted = m_pose.distWanted;
    if (lockRes.hasDist) evalDistWanted = lockRes.dist;
    evalDistWanted = clamp(evalDistWanted, m_prof.distMin, m_prof.distMax);

    const float rotA = ExpAlphaSpeed(m_prof.rotSmooth, dt);
    const float pivotA = ExpAlphaSpeed(m_prof.pivotSmooth, dt);

    Vector2 rotGoalLocal = m_pose.rotGoalDeg;

    if (m_dialogueYaw.active && lockRes.weight <= 0.f)
    {
        const float w = clamp(m_dialogueYaw.weight, 0.f, 1.f);
        const float delta = Math::WrapDeg(m_dialogueYaw.yawGoalDeg - rotGoalLocal.x);
        rotGoalLocal.x = rotGoalLocal.x + delta * w;
    }

    Vector2 rotCurNext = m_pose.rotCurDeg + (rotGoalLocal - m_pose.rotCurDeg) * rotA;
    const Vector3 pivotCurNext = m_pose.pivotCurWorld + (m_pose.pivotGoalWorld - m_pose.pivotCurWorld) * pivotA;

    if (lockRes.weight <= 0.f && m_prof.autoYaw && !m_dialogueYaw.active && !m_dialogueMode)
    {
        const Vector3 foot = GetFoot();

        Vector3 camLook{};
        Vector3 camRight{};
        Quaternion q{};
        BuildOrbitBasis(rotCurNext, camLook, camRight, q);

        m_pose.rotGoalDeg.x += EvalAutoYaw(dt, foot, camLook, camRight, m_pose.rotGoalDeg.x);

        ClampTargets();

        rotGoalLocal = m_pose.rotGoalDeg;
        rotCurNext = m_pose.rotCurDeg + (rotGoalLocal - m_pose.rotCurDeg) * rotA;
    }

    OrbitCollideEval colRes{};

    const _bool handoffCollisionGrace = ExternalHandoff_CollisionGraceOn();

    if (m_dialogueMode || handoffCollisionGrace)
    {
        m_hitDist = false;
        m_yawDeltaCapDeg = m_prof.yawDeltaCapDeg;
        m_pitchDeltaCapDeg = m_prof.pitchDeltaCapDeg;

        m_pose.distGoal = clamp(evalDistWanted, m_prof.distMin, m_prof.distMax);
    }
    else
    {
        colRes = EvalCollideDist(dt, m_prof, pivotCurNext, evalDistWanted, m_pose.rotCurDeg, rotCurNext, m_pose.distGoal);

        m_hitDist = colRes.hit;
        m_yawDeltaCapDeg = colRes.yawDeltaCapDeg;
        m_pitchDeltaCapDeg = colRes.pitchDeltaCapDeg;

        m_pose.distGoal = colRes.goalDist;

        if (colRes.hit)
        {
            if (m_pose.distGoal > colRes.allowedDist) m_pose.distGoal = colRes.allowedDist;
            if (m_pose.distCur > colRes.allowedDist) m_pose.distCur = colRes.allowedDist;
        }
    }

    const float distA = ExpAlphaSpeed(m_prof.distSmooth, dt);

    m_pose.rotCurDeg = rotCurNext;
    m_pose.pivotCurWorld = pivotCurNext;
    m_pose.distCur = m_pose.distCur + (m_pose.distGoal - m_pose.distCur) * distA;

    if (!m_dialogueMode && m_hitDist)
    {
        const _float clampDist = colRes.allowedDist;

        if (colRes.hitType == OrbitHitType::Ground)
        {
            if (m_pose.distCur > clampDist) m_pose.distCur = clampDist;
            if (m_pose.distGoal > clampDist) m_pose.distGoal = clampDist;
        }
        else
        {
            if (m_pose.distCur > clampDist) m_pose.distCur = clampDist;
            if (m_pose.distGoal > clampDist) m_pose.distGoal = clampDist;
        }
    }

    ApplyPose(dt, lockRes);
    EvalOcclusion();
}

void COrbitCam::EvalOcclusion()
{
    m_occlusion.BeginFrame();

    auto scene = PhysicsSystem()->Get_Scene();
    if (!scene) { m_occlusion.Dispatch(); return; }

    const Vector3 pivot = GetPivotPos();
    const Vector3 camPos = m_pTransform->Get_WorldPos();

    Vector3 seg = camPos - pivot;
    const float maxDist = seg.Length();
    if (maxDist <= 0.f) { m_occlusion.Dispatch(); return; }

    const Vector3 dir = seg / maxDist;

    struct Filter final : PxQueryFilterCallback
    {
        OBJECT_HANDLE ignoreA{};
        OBJECT_HANDLE ignoreB{};

        PxQueryHitType::Enum preFilter(const PxFilterData&, const PxShape* shape, const PxRigidActor* actor, PxHitFlags&) override
        {
            if (!actor) return PxQueryHitType::eNONE;
            if (shape && (shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)) return PxQueryHitType::eNONE;

            auto obj = reinterpret_cast<CGameObject*>(actor->userData);
            if (!obj) return PxQueryHitType::eNONE;

            OBJECT_HANDLE h = obj->Get_Handle();
            if (h == ignoreA) return PxQueryHitType::eNONE;
            if (h == ignoreB) return PxQueryHitType::eNONE;

            return PxQueryHitType::eTOUCH;
        }

        PxQueryHitType::Enum postFilter(const PxFilterData&, const PxQueryHit&) override
        {
            return PxQueryHitType::eTOUCH;
        }
    } filter;

    filter.ignoreA = m_target;
    filter.ignoreB = Lock_Handle();

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eNO_BLOCK;

    PxRaycastBufferN<64> buf;
    PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

    const PxVec3 originPx(pivot.x, pivot.y, pivot.z);
    const PxVec3 dirPx(dir.x, dir.y, dir.z);

    const _bool ok = scene->raycast(originPx, dirPx, maxDist, buf, hitFlags, filterData, &filter);
    if (ok)
    {
        for (PxU32 i = 0; i < buf.nbTouches; ++i)
        {
            const PxRaycastHit& h = buf.touches[i];

            auto obj = reinterpret_cast<CGameObject*>(h.actor->userData);
            if (!obj) continue;

            const Vector3 hitPos(h.position.x, h.position.y, h.position.z);
            const Vector3 hitN(h.normal.x, h.normal.y, h.normal.z);
            const _float hitDist = (_float)h.distance;

            m_occlusion.AddHit(obj, hitPos, hitN, hitDist);
        }
    }

    m_occlusion.Dispatch();
}

void COrbitCam::DialogueYaw_Set(_float yawGoalDeg, _float weight)
{
    m_dialogueYaw.active = true;
    m_dialogueYaw.yawGoalDeg = yawGoalDeg;
    m_dialogueYaw.weight = clamp(weight, 0.f, 1.f);
}

void COrbitCam::SwitchMode_Begin()
{
    m_freezeMode = FreezeMode::Switch;
}

void COrbitCam::SwitchMode_End()
{
    if (m_freezeMode == FreezeMode::Switch)
        m_freezeMode = FreezeMode::None;
}

void COrbitCam::ParryMode_Begin()
{
    m_freezeMode = FreezeMode::Parry;
}

void COrbitCam::ParryMode_End()
{
    if (m_freezeMode == FreezeMode::Parry)
        m_freezeMode = FreezeMode::None;
}

void COrbitCam::ResumeSync()
{
    m_pose.rotGoalDeg = m_pose.rotCurDeg;

    m_pose.distWanted = m_pose.distCur;
    m_pose.distGoal = m_pose.distCur;

    m_pose.pivotGoalWorld = m_pose.pivotCurWorld;

    m_switch = {};
    m_freeze = 0.f;

    m_hitDist = false;
    m_yawDeltaCapDeg = m_prof.yawDeltaCapDeg;
    m_pitchDeltaCapDeg = m_prof.pitchDeltaCapDeg;

    PivotStab_Reset(m_pose.pivotCurWorld);

    ClampTargets();
}

void COrbitCam::ClampTargets()
{
    m_pose.rotGoalDeg.y = clamp(m_pose.rotGoalDeg.y, m_prof.pitchLimitMinDeg, m_prof.pitchLimitMaxDeg);
    m_pose.distWanted = clamp(m_pose.distWanted, m_prof.distMin, m_prof.distMax);
}

Vector3 COrbitCam::GetPivotTargetPos() const
{
    const Vector3 basePivot = GetBasePivotTargetPos(m_target);
    return basePivot + m_pose.pivotInternalOffset + m_pose.pivotExternalOffset;
}

float COrbitCam::GetDist() const
{
    if (m_pose.distCur > m_prof.distMax) return m_prof.distMax;
    return m_pose.distCur;
}

void COrbitCam::ApplyPose(_float dt, const OrbitLockEval& lockRes)
{
    const Vector3 pivot = GetPivotPos();

    const Quaternion qYawPitch = YawPitchQuat(m_pose.rotCurDeg);

    const float dist = GetDist();
    const Vector3 orbitPos = OrbitCamPos(pivot, qYawPitch, dist);

    Vector3 camPos = orbitPos;

    if (m_returnPreset.active)
        camPos = ReturnPreset_EvalCamPos(dt, orbitPos);

    if (m_returnPreset.active)
    {
        Vector3 toPivot = pivot - camPos;
        const float d = toPivot.Length();
        if (d > 0.f)
        {
            toPivot /= d;

            Vector2 rot{};
            CalcRotDeg(toPivot, rot);

            m_pose.rotCurDeg = rot;
            m_pose.rotGoalDeg = rot;

            m_pose.distCur = d;
            m_pose.distGoal = d;
            m_pose.distWanted = d;
        }
    }

    const _bool hardGroundClamped = HardClampCameraPosToGround(camPos);

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    if (lockRes.weight > 0.f)
    {
        Vector3 lookAt = Vector3::Lerp(pivot, lockRes.focusPos, lockRes.weight);
        m_pTransform->LookAt(Vector4(lookAt.x, lookAt.y, lookAt.z, 1.f));
        return;
    }

    if (hardGroundClamped)
    {
        m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
        return;
    }

    m_pTransform->Set_WorldQuaternion(qYawPitch);
}

Vector3 COrbitCam::GetFoot() const
{
    return FootPos(m_target);
}

Vector3 COrbitCam::GetBasePivotTargetPos(OBJECT_HANDLE h) const
{
    return BasePivotPos(h, m_prof.offsetY);
}

void COrbitCam::Lock_CaptureEnterPreset()
{
    m_lockEnterPreset.active = true;

    m_lockEnterPreset.rotGoalDeg = m_pose.rotGoalDeg;
    m_lockEnterPreset.rotCurDeg = m_pose.rotCurDeg;

    m_lockEnterPreset.distWanted = m_pose.distWanted;
    m_lockEnterPreset.distGoal = m_pose.distGoal;
    m_lockEnterPreset.distCur = m_pose.distCur;

    m_lockEnterPreset.pivotInternalOffset = m_pose.pivotInternalOffset;
    m_lockEnterPreset.pivotExternalOffset = m_pose.pivotExternalOffset;
}

void COrbitCam::AutoYaw_OnTarget()
{
    m_autoYaw.holdTimer = m_prof.autoYawDelay;
    m_autoYaw.hasPrevFoot = false;
    m_autoYaw.prevFootWorld = {};
}

_float COrbitCam::EvalAutoYaw(_float dt, const Vector3& foot, const Vector3& camLookWorld, const Vector3& camRightWorld, _float curYawDeg)
{
    if (Freeze_On()) return 0.f;
    if (!m_prof.autoYaw) return 0.f;

    if (m_autoYaw.holdTimer > 0.f)
    {
        m_autoYaw.holdTimer -= dt;
        m_autoYaw.prevFootWorld = foot;
        m_autoYaw.hasPrevFoot = true;
        return 0.f;
    }

    if (!m_autoYaw.hasPrevFoot)
    {
        m_autoYaw.prevFootWorld = foot;
        m_autoYaw.hasPrevFoot = true;
        return 0.f;
    }

    Vector3 move = foot - m_autoYaw.prevFootWorld;
    m_autoYaw.prevFootWorld = foot;

    move.y = 0.f;

    const float len = move.Length();
    if (len < 0.002f) return 0.f;

    move /= len;

    Vector3 camLook = camLookWorld;
    Vector3 camRight = camRightWorld;

    camLook.y = 0.f;
    camRight.y = 0.f;

    camLook.Normalize();
    camRight.Normalize();

    const float localZ = move.Dot(camLook);
    const float localX = move.Dot(camRight);

    if (localZ < 0.f && fabsf(localZ) > fabsf(localX)) return 0.f;

    const float desiredYawDeg = XMConvertToDegrees(atan2f(move.x, move.z));
    const float deltaYawDeg = Math::WrapDeg(desiredYawDeg - curYawDeg);

    const float a = ExpAlphaSpeed(m_prof.autoYawSpeed, dt);

    return deltaYawDeg * a;
}

void COrbitCam::Switch_Begin(const Vector3& holdPivotWorld)
{
    m_switch.active = true;
    m_switch.elapsed = 0.f;
    m_switch.holdPivotWorld = holdPivotWorld;
}

Vector3 COrbitCam::Switch_EvalOffset(_float dt, const Vector3& basePivotNow)
{
    if (!m_switch.active) return Vector3::Zero;

    m_switch.elapsed += dt;

    _float ratio = m_switch.elapsed / m_prof.switchBlendSec;
    if (ratio >= 1.f)
    {
        m_switch.active = false;
        return Vector3::Zero;
    }

    ratio = clamp(ratio, 0.f, 1.f);
    ratio = Math::ApplyEase(m_prof.switchEase, ratio);

    const Vector3 keepOff = m_switch.holdPivotWorld - basePivotNow;
    return Vector3::Lerp(keepOff, Vector3::Zero, ratio);
}

OrbitInputEval COrbitCam::EvalInput(_float dt, _float lockW)
{
    OrbitInputEval out{};

    if (m_lockInput) return out;
    if (Freeze_On()) return out;

#ifdef _USING_GUI
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered()) return out;
#endif

    const float mouseDx = InputDevice()->Mouse_DeltaX();
    const float mouseDy = InputDevice()->Mouse_DeltaY();
    const float wheel = InputDevice()->Mouse_DeltaW() * 0.5f;

    if (lockW <= 0.f) out.yawDeltaDeg = mouseDx * m_input.sensX;
    out.pitchDeltaDeg = mouseDy * m_input.sensY;

    if (wheel != 0.f) out.zoomDelta = wheel * m_input.zoomSpeed;

    if (mouseDx != 0.f || mouseDy != 0.f || wheel != 0.f) AutoYaw_OnInput();

    const float yawCapThisFrame = m_yawDeltaCapDeg * dt;
    const float pitchCapThisFrame = m_pitchDeltaCapDeg * dt;

    out.yawDeltaDeg = clamp(out.yawDeltaDeg, -yawCapThisFrame, yawCapThisFrame);
    out.pitchDeltaDeg = clamp(out.pitchDeltaDeg, -pitchCapThisFrame, pitchCapThisFrame);

    return out;
}

OrbitCollideEval COrbitCam::EvalCollideDist(_float dt, const OrbitProfile& prof, const Vector3& pivotWorld, _float distWanted, const Vector2& rotCurDeg, const Vector2& rotGoalDeg, _float distGoal)
{
    OrbitCollideEval out = CalcAllowDist(prof, pivotWorld, distWanted, rotCurDeg, rotGoalDeg);

    out.rawAllowedDist = out.allowedDist;
    out.hit = (out.rawAllowedDist < distWanted - 0.001f);

    if (!out.hit)
        out.hitType = OrbitHitType::None;

    out.yawDeltaCapDeg = out.hit ? prof.yawHitDeltaCapDeg : prof.yawDeltaCapDeg;
    out.pitchDeltaCapDeg = out.hit ? prof.pitchHitDeltaCapDeg : prof.pitchDeltaCapDeg;

    if (out.hit && out.hitType == OrbitHitType::Common)
        out.allowedDist = clamp(out.rawAllowedDist + prof.commonSoftSlack, prof.distMin, distWanted);

    const _float targetDist = min(distWanted, out.allowedDist);

    _float zoomInSpeed = prof.zoomInCollide;
    if (out.hit && out.hitType == OrbitHitType::Common)
        zoomInSpeed *= prof.commonZoomInMul;

    const _float zoomSpeed = (targetDist < distGoal) ? zoomInSpeed : prof.zoomOutCollide;

    _float nextGoal = Math::MoveTowards(distGoal, targetDist, zoomSpeed * dt);
    nextGoal = clamp(nextGoal, prof.distMin, prof.distMax);

    out.goalDist = nextGoal;
    return out;
}

OrbitCollideEval COrbitCam::CalcAllowDist(const OrbitProfile& prof, const Vector3& pivotWorld, _float distWanted, const Vector2& rotCurDeg, const Vector2& rotGoalDeg)
{
    OrbitCollideEval out{};
    out.allowedDist = distWanted;
    out.rawAllowedDist = distWanted;
    out.hitType = OrbitHitType::None;

    const _float camRadius = Get_Component<CCharacterController>()->Get_Radius();
    const _float padding = 0.1f;
    const _float stepDeg = 4.f;

    auto scene = PhysicsSystem()->Get_Scene();
    if (!scene) return out;

    const _float startYaw = rotCurDeg.x;
    const _float startPitch = rotCurDeg.y;

    const _float endYaw = rotGoalDeg.x;
    const _float endPitch = rotGoalDeg.y;

    const _float deltaYaw = Math::WrapDeg(endYaw - startYaw);
    const _float deltaPitch = endPitch - startPitch;

    const _float maxAbs = max(fabsf(deltaYaw), fabsf(deltaPitch));
    _int steps = (_int)ceilf(maxAbs / stepDeg);
    if (steps < 1) steps = 1;
    if (steps > 12) steps = 12;

    PxSphereGeometry geom(camRadius);

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER;

    CRaycastFilterCallback filterCallback(ENUM(COLLISION_GROUP::COMMON) + ENUM(COLLISION_GROUP::GROUND), false);

    for (_int i = 1; i <= steps; ++i)
    {
        const _float ratio = (_float)i / (_float)steps;

        const _float yawRad = XMConvertToRadians(startYaw + deltaYaw * ratio);
        const _float pitchRad = XMConvertToRadians(startPitch + deltaPitch * ratio);

        const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

        Vector3 dir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
        dir.Normalize();

        PxTransform posePx(PxVec3(pivotWorld.x, pivotWorld.y, pivotWorld.z));

        PxSweepBuffer hit;
        PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

        const _float sweepDist = distWanted + padding;

        const _bool ok = scene->sweep(geom, posePx, PxVec3(dir.x, dir.y, dir.z), sweepDist, hit, hitFlags, filterData, &filterCallback);
        if (!ok || !hit.hasBlock) continue;

        _float allowed = hit.block.distance - padding;
        allowed = clamp(allowed, prof.distMin, distWanted);

        OrbitHitType stepHitType = OrbitHitType::Common;

        if (hit.block.shape)
        {
            const PxFilterData shapeFilter = hit.block.shape->getQueryFilterData();
            const PxU32 shapeGroup = shapeFilter.word0;

            const _bool isGround = (shapeGroup & ENUM(COLLISION_GROUP::GROUND)) != 0;
            const _bool isCommon = (shapeGroup & ENUM(COLLISION_GROUP::COMMON)) != 0;

            if (isGround) stepHitType = OrbitHitType::Ground;
            else if (isCommon) stepHitType = OrbitHitType::Common;
        }

        if (allowed < out.allowedDist)
        {
            out.allowedDist = allowed;
            out.hitType = stepHitType;
        }
        else if (fabsf(allowed - out.allowedDist) <= 0.001f)
        {
            if (stepHitType == OrbitHitType::Ground)
                out.hitType = OrbitHitType::Ground;
        }
    }

    out.rawAllowedDist = out.allowedDist;
    out.hit = (out.allowedDist < distWanted - 0.001f);

    if (!out.hit)
        out.hitType = OrbitHitType::None;

    return out;
}

void COrbitCam::Lock_Reset()
{
    m_lock = {};
    m_lockBlend = {};
    m_lockFocus = {};
    m_hasLockFocus = false;
    m_lockAir = {};
    Lock_ClearEnterPreset();
    ExternalHandoff_Reset();
}

void COrbitCam::Lock_Enter(OBJECT_HANDLE h, _float curDist)
{
    Lock_CaptureEnterPreset();

    m_lock.active = true;
    m_lock.handle = h;
    m_lock.savedDist = curDist;
    m_lockAir = {};

    Lock_BlendStart(true);
}

void COrbitCam::Lock_Exit()
{
    if (!Lock_On()) return;

    m_lockAir = {};

    if (!m_lockBlend.active)
    {
        Lock_BlendStart(false);
        return;
    }

    if (m_lockBlend.entering) Lock_BlendStart(false);
}

void COrbitCam::Lock_BlendUpdate(_float dt)
{
    if (!m_lockBlend.active)
    {
        m_lockBlend.weight = m_lock.active ? 1.f : 0.f;
        return;
    }

    if (m_lockBlend.holdFirstFrame)
    {
        m_lockBlend.holdFirstFrame = false;
        return;
    }

    m_lockBlend.elapsed += dt;

    _float ratio = m_lockBlend.elapsed / m_lockBlend.duration;
    if (ratio >= 1.f)
    {
        m_lockBlend.active = false;

        if (m_lockBlend.entering) m_lockBlend.weight = 1.f;
        else
        {
            m_lockBlend.weight = 0.f;
            m_lock = {};
            m_lockFocus = {};
            m_hasLockFocus = false;
            m_lockAir = {};
            Lock_ClearEnterPreset();
        }
        return;
    }

    ratio = clamp(ratio, 0.f, 1.f);
    const _float e = Math::ApplyEase(m_lockBlend.ease, ratio);

    m_lockBlend.weight = m_lockBlend.entering ? e : (1.f - e);
}

OrbitLockEval COrbitCam::EvalLock(_float dt, _float curYawDeg, _float curDist)
{
    OrbitLockEval out{};

    const _float w = Lock_Weight();
    out.weight = w;

    if (!m_target.isValid()) return out;

    const Vector3 basePlayerPivot = GetBasePivotTargetPos(m_target);

    Vector3 playerPivot = basePlayerPivot;
    if (m_lockBlend.active && m_lockBlend.entering)
        playerPivot = Vector3::Lerp(m_pose.pivotCurWorld, basePlayerPivot, w);

    return EvalLock_PlayerPivot(dt, playerPivot, curYawDeg, curDist);
}

void COrbitCam::Lock_BlendStart(_bool entering)
{
    m_lockBlend.active = true;
    m_lockBlend.entering = entering;
    m_lockBlend.elapsed = 0.f;

    if (entering)
    {
        m_lockBlend.duration = m_prof.lockBlendIn;
        m_lockBlend.ease = m_prof.lockBlendInEase;
        m_lockBlend.weight = 0.f;
    }
    else
    {
        m_lockBlend.duration = m_prof.lockBlendOut;
        m_lockBlend.ease = m_prof.lockBlendOutEase;
        m_lockBlend.weight = 1.f;
    }

    if (m_lockBlend.duration <= 0.f)
        m_lockBlend.duration = 0.0001f;
}

void COrbitCam::SyncPivot()
{
    const Vector3 pivot = GetPivotTargetPos();
    PivotStab_Reset(pivot);

    m_pose.pivotGoalWorld = pivot;
    m_pose.pivotCurWorld = pivot;
}

void COrbitCam::CalcRotDeg(const Vector3& lookDir, Vector2& outRotDeg) const
{
    const float yawRad = atan2f(lookDir.x, lookDir.z);
    const float pitchRad = asinf(clamp(-lookDir.y, -1.f, 1.f));

    outRotDeg.x = XMConvertToDegrees(yawRad);
    outRotDeg.y = XMConvertToDegrees(pitchRad);
}

void COrbitCam::GetBasePivot(const OBJECT_HANDLE& h, Vector3& outFootWorld, Vector3& outBasePivotWorld) const
{
    outFootWorld = FootPos(h);
    outBasePivotWorld = BasePivotPos(h, m_prof.offsetY);
}

void COrbitCam::SnapPose(const Vector3& camPos, const Quaternion& camRot, const Vector3& basePivotWorld)
{
    Vector3 forward = Vector3::Transform(Vector3(0.f, 0.f, 1.f), camRot);
    forward.Normalize();

    float dist = (basePivotWorld - camPos).Length();
    dist = clamp(dist, m_prof.distMin, m_prof.distMax);

    const Vector3 holdPivotWorld = camPos + forward * dist;

    Switch_Reset();

    m_pose.pivotExternalOffset = Vector3::Zero;
    m_pose.pivotInternalOffset = holdPivotWorld - basePivotWorld;

    m_pose.pivotCurWorld = holdPivotWorld;
    m_pose.pivotGoalWorld = holdPivotWorld;

    Vector3 toPivot = holdPivotWorld - camPos;
    const float rawDist = toPivot.Length();
    toPivot /= rawDist;

    Vector2 rotDeg{};
    CalcRotDeg(toPivot, rotDeg);

    m_pose.rotCurDeg = rotDeg;
    m_pose.rotGoalDeg = rotDeg;

    m_pose.distCur = dist;
    m_pose.distGoal = dist;
    m_pose.distWanted = dist;
}

void COrbitCam::PivotStab_Reset(const Vector3& pivot)
{
    m_pivotStab.hasLast = true;

    m_pivotStab.lastRawPivot = pivot;
    m_pivotStab.rawVel = Vector3::Zero;

    m_pivotStab.filteredPivot = pivot;
    m_pivotStab.filteredVel = Vector3::Zero;
}

Vector3 COrbitCam::PivotStab_Eval(_float dt, const Vector3& rawPivot)
{
    if (!m_pivotStab.hasLast)
    {
        PivotStab_Reset(rawPivot);
        return rawPivot;
    }

    Vector3 rawDelta = rawPivot - m_pivotStab.lastRawPivot;
    Vector3 rawVel = rawDelta / max(dt, 0.0001f);

    const float rawJump = rawDelta.Length();
    const float rawSpeed = rawVel.Length();

    if (rawJump > m_pivotStab.outlierDist || rawSpeed > m_pivotStab.outlierVel)
    {
        PivotStab_Reset(rawPivot);
        return rawPivot;
    }

    m_pivotStab.lastRawPivot = rawPivot;

    float aVel = Math::ExpAlpha(m_pivotStab.velTau, dt);
    m_pivotStab.filteredVel = Math::Lerp(m_pivotStab.filteredVel, rawVel, aVel);

    const float speed = m_pivotStab.filteredVel.Length();
    const float dynamicTau = m_pivotStab.rawTau / (1.f + speed * 0.15f);

    float aPos = Math::ExpAlpha(dynamicTau, dt);
    m_pivotStab.filteredPivot = Math::Lerp(m_pivotStab.filteredPivot, rawPivot, aPos);

    return m_pivotStab.filteredPivot;
}

OrbitLockEval COrbitCam::EvalLock_PlayerPivot(_float dt, const Vector3& playerPivot, _float curYawDeg, _float curDist)
{
    OrbitLockEval out{};

    const _float w = Lock_Weight();
    out.weight = w;

    out.focusPos = m_hasLockFocus ? m_lockFocus : playerPivot;

    if (m_lockBlend.active && !m_lockBlend.entering)
    {
        out.hasDist = true;
        const _float k = 1.f - w;
        out.dist = curDist + (m_lock.savedDist - curDist) * k;
    }

    if (!m_lock.handle.isValid()) return out;

    constexpr _float kEps = 0.0001f;

    constexpr _float kYawFreezeXZStart = 0.35f;
    constexpr _float kYawFreezeXZEnd = 0.90f;

    constexpr _float kOverheadFadeStartDeg = 55.f;
    constexpr _float kOverheadFadeEndDeg = 82.f;

    constexpr _float kOverheadEnterElevationDeg = 68.f;
    constexpr _float kOverheadExitElevationDeg = 55.f;
    constexpr _float kOverheadEnterXZ = 0.80f;
    constexpr _float kOverheadExitXZ = 1.20f;

    constexpr _float kOverheadYawMul = 0.15f;
    constexpr _float kOverheadFocusMinXZ = 0.90f;

    constexpr _float kLockLookYawMaxDegPerSec = 720.f;
    constexpr _float kLockLookPitchMaxDegPerSec = 540.f;

    constexpr _float kFocusUpMaxDeg = 72.f;
    constexpr _float kFocusUpMaxDegParry = 68.f;

    constexpr _float kTargetPivotTauXZ = 0.10f;
    constexpr _float kTargetPivotTauY = 0.20f;

    const Vector3 rawTargetPivot = LockPivotPos(m_lock.handle, m_prof.offsetY);

    if (!m_lockAir.hasFilteredTargetPivot)
    {
        m_lockAir.filteredTargetPivot = rawTargetPivot;
        m_lockAir.hasFilteredTargetPivot = true;
    }
    else
    {
        const _float aXZ = Math::ExpAlpha(kTargetPivotTauXZ, dt);
        const _float aY = Math::ExpAlpha(kTargetPivotTauY, dt);

        m_lockAir.filteredTargetPivot.x = Math::Lerp(m_lockAir.filteredTargetPivot.x, rawTargetPivot.x, aXZ);
        m_lockAir.filteredTargetPivot.z = Math::Lerp(m_lockAir.filteredTargetPivot.z, rawTargetPivot.z, aXZ);
        m_lockAir.filteredTargetPivot.y = Math::Lerp(m_lockAir.filteredTargetPivot.y, rawTargetPivot.y, aY);
    }

    const Vector3 targetPivot = m_lockAir.filteredTargetPivot;

    const Vector3 toTargetRaw = targetPivot - playerPivot;
    const _float toTargetLenRaw = toTargetRaw.Length();
    if (toTargetLenRaw <= kEps) return out;

    Vector3 flatRaw = toTargetRaw;
    flatRaw.y = 0.f;
    const _float lenXZRaw = flatRaw.Length();

    const _float elevationDegRaw = XMConvertToDegrees(atan2f(fabsf(toTargetRaw.y), max(lenXZRaw, kEps)));

    if (!m_lockAir.overheadActive)
    {
        if (elevationDegRaw >= kOverheadEnterElevationDeg || lenXZRaw <= kOverheadEnterXZ)
            m_lockAir.overheadActive = true;
    }
    else
    {
        if (elevationDegRaw <= kOverheadExitElevationDeg && lenXZRaw >= kOverheadExitXZ)
            m_lockAir.overheadActive = false;
    }

    if (lenXZRaw > kYawFreezeXZEnd)
    {
        m_lockAir.lastGoodFlatDir = flatRaw / lenXZRaw;
        m_lockAir.hasLastGoodFlatDir = true;
    }
    else if (!m_lockAir.hasLastGoodFlatDir && lenXZRaw > kEps)
    {
        m_lockAir.lastGoodFlatDir = flatRaw / lenXZRaw;
        m_lockAir.hasLastGoodFlatDir = true;
    }

    if (w <= 0.f) return out;

    const Vector3 toTarget = targetPivot - playerPivot;
    const _float toTargetLen = toTarget.Length();
    if (toTargetLen <= kEps) return out;

    Vector3 flat = toTarget;
    flat.y = 0.f;

    const _float lenXZ = flat.Length();

    _float xzYawScale = 0.f;
    if (lenXZ >= kYawFreezeXZEnd) xzYawScale = 1.f;
    else if (lenXZ > kYawFreezeXZStart) xzYawScale = (lenXZ - kYawFreezeXZStart) / (kYawFreezeXZEnd - kYawFreezeXZStart);

    const _float elevationDeg = XMConvertToDegrees(atan2f(fabsf(toTarget.y), max(lenXZ, kEps)));

    _float overheadT = 0.f;
    if (elevationDeg >= kOverheadFadeEndDeg) overheadT = 1.f;
    else if (elevationDeg > kOverheadFadeStartDeg) overheadT = (elevationDeg - kOverheadFadeStartDeg) / (kOverheadFadeEndDeg - kOverheadFadeStartDeg);

    _float overheadYawScale = 1.f - overheadT;
    overheadYawScale = overheadYawScale * overheadYawScale;

    _float yawFollowScale = xzYawScale * overheadYawScale;
    if (m_lockAir.overheadActive) yawFollowScale *= kOverheadYawMul;

    Vector3 yawDir{};
    _bool hasYawDir = false;

    if (lenXZ > kEps)
    {
        yawDir = flat / lenXZ;
        hasYawDir = true;
    }

    if (m_lockAir.overheadActive && m_lockAir.hasLastGoodFlatDir)
    {
        yawDir = m_lockAir.lastGoodFlatDir;
        hasYawDir = true;
    }

    if (hasYawDir)
    {
        const _float desiredYawDeg = XMConvertToDegrees(atan2f(yawDir.x, yawDir.z));
        const _float deltaYawDeg = Math::WrapDeg(desiredYawDeg - curYawDeg);

        const _float a = ExpAlphaSpeed(m_prof.lockYawSpeed, dt);

        _float yawAdd = deltaYawDeg * a * w * yawFollowScale;

        const _float yawStepCap = kLockLookYawMaxDegPerSec * max(dt, 0.f);
        yawAdd = clamp(yawAdd, -yawStepCap, yawStepCap);

        out.yawAddDeg = yawAdd;
    }

    _float distT = lenXZ / (lenXZ + m_prof.lockFocusDist);
    distT = clamp(distT, 0.f, 1.f);

    const _float focusT = m_prof.lockFocusNear + (m_prof.lockFocusFar - m_prof.lockFocusNear) * distT;

    Vector3 desiredFocus = Vector3::Lerp(playerPivot, targetPivot, focusT);

    desiredFocus.x = Math::Lerp(playerPivot.x, desiredFocus.x, yawFollowScale);
    desiredFocus.z = Math::Lerp(playerPivot.z, desiredFocus.z, yawFollowScale);

    if (m_lockAir.overheadActive && m_lockAir.hasLastGoodFlatDir)
    {
        Vector3 desiredFlat = desiredFocus - playerPivot;
        desiredFlat.y = 0.f;

        _float desiredFlatLen = desiredFlat.Length();
        _float anchorLen = max(desiredFlatLen, kOverheadFocusMinXZ);

        const Vector3 anchoredXZ = playerPivot + m_lockAir.lastGoodFlatDir * anchorLen;

        desiredFocus.x = Math::Lerp(desiredFocus.x, anchoredXZ.x, overheadT);
        desiredFocus.z = Math::Lerp(desiredFocus.z, anchoredXZ.z, overheadT);
    }

    const Vector3 camPos = m_pTransform->Get_WorldPos();

    auto ClampFocusUp = [&](const Vector3& inFocus) -> Vector3
        {
            Vector3 outFocus = inFocus;

            Vector3 dir = outFocus - camPos;
            const _float len = dir.Length();
            if (len <= kEps) return outFocus;

            dir /= len;

            Vector2 rot{};
            CalcRotDeg(dir, rot);

            const _float upPitchLimit = (m_freezeMode == FreezeMode::Parry) ? -kFocusUpMaxDegParry : -kFocusUpMaxDeg;
            rot.y = max(rot.y, upPitchLimit);
            rot.y = clamp(rot.y, -89.f, 89.f);

            const Quaternion q = YawPitchQuat(rot);
            const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
            const Vector3 lookDir = -backDir;

            outFocus = camPos + lookDir * len;
            return outFocus;
        };

    desiredFocus = ClampFocusUp(desiredFocus);

    if (m_hasLockFocus)
    {
        Vector3 prevDir = m_lockFocus - camPos;
        Vector3 rawDir = desiredFocus - camPos;

        const _float prevLen = prevDir.Length();
        const _float rawLen = rawDir.Length();

        if (prevLen > kEps && rawLen > kEps)
        {
            prevDir /= prevLen;
            rawDir /= rawLen;

            Vector2 prevRot{};
            Vector2 rawRot{};

            CalcRotDeg(prevDir, prevRot);
            CalcRotDeg(rawDir, rawRot);

            const _float upPitchLimit = (m_freezeMode == FreezeMode::Parry) ? -kFocusUpMaxDegParry : -kFocusUpMaxDeg;
            rawRot.y = max(rawRot.y, upPitchLimit);

            const _float maxYawStep = kLockLookYawMaxDegPerSec * max(dt, 0.f);
            const _float maxPitchStep = kLockLookPitchMaxDegPerSec * max(dt, 0.f);

            const _float yawDelta = clamp(Math::WrapDeg(rawRot.x - prevRot.x), -maxYawStep, maxYawStep);
            const _float pitchDelta = clamp(rawRot.y - prevRot.y, -maxPitchStep, maxPitchStep);

            const _float yawClamped = prevRot.x + yawDelta;
            const _float pitchClamped = clamp(max(prevRot.y + pitchDelta, upPitchLimit), -89.f, 89.f);

            const Quaternion q = YawPitchQuat(Vector2(yawClamped, pitchClamped));
            const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
            const Vector3 lookDir = -backDir;

            desiredFocus = camPos + lookDir * rawLen;
        }
    }

    out.focusPos = desiredFocus;

    m_lockFocus = out.focusPos;
    m_hasLockFocus = true;

    if (m_prof.lockAutoZoom && !out.hasDist)
    {
        _float autoZoomLenXZ = lenXZ;
        if (m_lockAir.overheadActive) autoZoomLenXZ = max(autoZoomLenXZ, kOverheadFocusMinXZ);

        const _float wanted = autoZoomLenXZ * m_prof.lockAutoZoomFactor;
        const _float clampedDist = clamp(wanted, m_prof.distMin, m_prof.distMax);

        if (curDist < clampedDist)
        {
            out.hasDist = true;
            out.dist = curDist + (clampedDist - curDist) * w;
        }
    }

    return out;
}

_bool COrbitCam::GetStableOrbitBeginPose(Vector3& outPivotWorld, Vector2& outRotDeg, _float& outDist) const
{
    if (Lock_On() && m_lockEnterPreset.active && m_target.isValid())
    {
        const Vector3 basePivot = GetBasePivotTargetPos(m_target);
        outPivotWorld = basePivot + m_lockEnterPreset.pivotInternalOffset + m_lockEnterPreset.pivotExternalOffset;
        outRotDeg = m_lockEnterPreset.rotCurDeg;
        outDist = clamp(m_lockEnterPreset.distWanted, m_prof.distMin, m_prof.distMax);
        return true;
    }

    outPivotWorld = m_pose.pivotCurWorld;
    outRotDeg = m_pose.rotCurDeg;
    outDist = m_pose.distCur;
    return false;
}

_bool COrbitCam::QueryGroundMinCamY_BySweep(const Vector3& candidateCamPos, _float& outMinCamY)
{
    auto scene = PhysicsSystem()->Get_Scene();
    if (!scene) return false;

    const _float ccRadius = Get_Component<CCharacterController>()->Get_Radius();
    const _float probeRadius = max(0.01f, ccRadius * m_prof.hardGroundRadiusScale);

    const _float probeUp = max(0.01f, m_prof.hardGroundProbeUp);
    const _float probeDown = max(0.01f, m_prof.hardGroundProbeDown);
    const _float padding = max(0.f, m_prof.hardGroundPadding);

    const Vector3 startPos = candidateCamPos + Vector3(0.f, probeUp, 0.f);
    const _float sweepDist = probeUp + probeDown;

    PxSphereGeometry geom(probeRadius);
    PxTransform posePx(PxVec3(startPos.x, startPos.y, startPos.z));

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER;

    CRaycastFilterCallback filterCallback(ENUM(COLLISION_GROUP::GROUND), false);

    PxSweepBuffer hit;
    PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

    const PxVec3 dirPx(0.f, -1.f, 0.f);

    const _bool ok = scene->sweep(geom, posePx, dirPx, sweepDist, hit, hitFlags, filterData, &filterCallback);
    if (!ok || !hit.hasBlock) return false;

    const _float impactCenterY = startPos.y - (_float)hit.block.distance;
    outMinCamY = impactCenterY + padding;
    return true;
}

_bool COrbitCam::HardClampCameraPosToGround(Vector3& inOutCamPos)
{
    if (!m_prof.hardGroundClamp) return false;

    _float minCamY = 0.f;
    if (!QueryGroundMinCamY_BySweep(inOutCamPos, minCamY)) return false;
    if (inOutCamPos.y >= minCamY) return false;

    inOutCamPos.y = minCamY;
    return true;
}

void COrbitCam::ExternalHandoff_Reset()
{
    m_externalHandoff = {};
}

void COrbitCam::CommitExternalHandoff(const Vector3& pivotWorld, const Vector3& camPosWorld, const Quaternion& camRot, const Vector3& lookAtWorld, _float pivotRecoverSec, _float collisionGraceSec, _float lockDistGraceSec)
{
    const _float dist = (pivotWorld - camPosWorld).Length();

    SnapFromExternalPose(pivotWorld, camPosWorld, camRot, dist);

    m_returnPreset = {};
    m_switch = {};

    m_pose.rotGoalDeg = m_pose.rotCurDeg;
    m_pose.distWanted = m_pose.distCur;
    m_pose.distGoal = m_pose.distCur;
    m_pose.pivotGoalWorld = m_pose.pivotCurWorld;

    PivotStab_Reset(m_pose.pivotCurWorld);

    m_lockFocus = lookAtWorld;
    m_hasLockFocus = true;

    AutoYaw_OnInput();

    m_freeze = max(m_freeze, 0.02f);
    if (m_freezeMode == FreezeMode::Parry)
        m_freezeMode = FreezeMode::None;

    m_externalHandoff.active = true;
    m_externalHandoff.elapsed = 0.f;
    m_externalHandoff.pivotRecoverSec = max(pivotRecoverSec, 0.0001f);
    m_externalHandoff.collisionGraceRemain = max(collisionGraceSec, 0.f);
    m_externalHandoff.lockDistGraceRemain = max(lockDistGraceSec, 0.f);
    m_externalHandoff.pivotInternalFrom = m_pose.pivotInternalOffset;
    m_externalHandoff.pivotInternalTo = Vector3::Zero;

    ClampTargets();
}

void COrbitCam::ExternalHandoff_Update(_float dt)
{
    if (!m_externalHandoff.active) return;

    if (m_externalHandoff.collisionGraceRemain > 0.f)
        m_externalHandoff.collisionGraceRemain = max(0.f, m_externalHandoff.collisionGraceRemain - dt);

    if (m_externalHandoff.lockDistGraceRemain > 0.f)
        m_externalHandoff.lockDistGraceRemain = max(0.f, m_externalHandoff.lockDistGraceRemain - dt);

    m_externalHandoff.elapsed += dt;

    const _float u = clamp(m_externalHandoff.elapsed / m_externalHandoff.pivotRecoverSec, 0.f, 1.f);
    const _float t = Math::ApplyEase(EaseType::InOutSine, u);

    m_pose.pivotInternalOffset = Vector3::Lerp(m_externalHandoff.pivotInternalFrom, m_externalHandoff.pivotInternalTo, t);

    if (u >= 1.f && m_externalHandoff.collisionGraceRemain <= 0.f && m_externalHandoff.lockDistGraceRemain <= 0.f)
        m_externalHandoff.active = false;
}

_bool COrbitCam::ExternalHandoff_CollisionGraceOn() const
{
    return m_externalHandoff.collisionGraceRemain > 0.f;
}

_bool COrbitCam::ExternalHandoff_LockDistGraceOn() const
{
    return m_externalHandoff.lockDistGraceRemain > 0.f;
}

void COrbitCam::DrawDebugPivot()
{
#ifdef _USING_GUI
    const Vector3 pivotWorld = m_pose.pivotCurWorld;

    const Vector3 camPos = m_pTransform->Get_WorldPos();
    const Vector3 camLook = m_pTransform->Dir(STATE::LOOK);
    const Vector3 camUp = m_pTransform->Dir(STATE::UP);

    const auto& io = ImGui::GetIO();
    const float w = io.DisplaySize.x;
    const float h = io.DisplaySize.y;
    const float aspect = w / max(h, 0.0001f);

    const float fovDeg = CameraManager()->GetFov();
    const float fovRad = XMConvertToRadians(fovDeg);

    const Matrix view = Matrix::CreateLookAt(camPos, camPos + camLook, camUp);
    const Matrix proj = Matrix::CreatePerspectiveFieldOfView(fovRad, aspect, 0.1f, 1000.f);

    const Matrix viewProj = view * proj;
    const Vector4 clip = Vector4::Transform(Vector4(pivotWorld.x, pivotWorld.y, pivotWorld.z, 1.f), viewProj);

    const float invW = 1.f / clip.w;
    const float ndcX = clip.x * invW;
    const float ndcY = clip.y * invW;

    const float sx = (ndcX * 0.5f + 0.5f) * w;
    const float sy = (-ndcY * 0.5f + 0.5f) * h;

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    dl->AddCircleFilled(ImVec2(sx, sy), 4.f, IM_COL32(255, 0, 0, 255));
#endif
}

COrbitCam* COrbitCam::Create()
{
    auto inst = new COrbitCam();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Object Create Failed : COrbitCam");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* COrbitCam::Clone(INIT_DESC* pArg)
{
    auto inst = new COrbitCam(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Object Clone Failed : COrbitCam");
        Safe_Release(inst);
    }
    return inst;
}

void COrbitCam::Render_GUI()
{
    Get_Component<CTransform>()->Render_GUI();

#ifdef _USING_GUI
    ImGui::SeparatorText("OrbitCam");

    const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
    const float childHeight = (textLineHeight * 11) + (ImGui::GetStyle().WindowPadding.y * 2);

    const Vector3 pivotWorld = m_pose.pivotCurWorld;
    const Vector3 localOffset = m_pose.pivotInternalOffset + m_pose.pivotExternalOffset;

    const _bool isChainParry = BattleSystem()->GetBattlePlayer()->Is_ChainParry();

    ImGui::BeginChild("##OrbitCamChild", ImVec2{0, childHeight}, true);

    if (ImGui::BeginTable("##OrbitCamInfo", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 160.f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Fov");
        ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f", CameraManager()->GetFov());

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("ChainParry");
        ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(isChainParry ? "O" : "X");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Distance");
        ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", m_pose.distCur);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Pivot World");
        ImGui::TableSetColumnIndex(1); ImGui::Text("x %.3f   y %.3f   z %.3f", pivotWorld.x, pivotWorld.y, pivotWorld.z);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Pivot LocalOffset");
        ImGui::TableSetColumnIndex(1); ImGui::Text("x %.3f   y %.3f   z %.3f", localOffset.x, localOffset.y, localOffset.z);

        ImGui::EndTable();
    }

    ImGui::EndChild();

    const auto& io = ImGui::GetIO();
    const float w = io.DisplaySize.x;
    const float h = io.DisplaySize.y;

    const Matrix view = *CameraManager()->Get_ViewMatrix();
    const Matrix proj = *CameraManager()->Get_ProjMatrix();

    const _float4 vp = {0.f, 0.f, w, h};

    Vector2 screen{};

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    if (Helper::WorldToScreen(pivotWorld, screen, view, proj, vp))
        dl->AddCircleFilled(ImVec2(screen.x, screen.y), 4.f, IM_COL32(255, 0, 0, 255));
#endif
}