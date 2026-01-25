#include "pch.h"
#include "OrbitCam.h"
// Engine
#include "GameInstance.h"
#include "Character.h"
#include "Helper_Func.h"
#include "PhysicsSystem.h"
// Components
#include "CharacterController.h"
#include "EventListener.h"

void COrbitCam::Awake()
{
    auto cc = Get_Component<CCharacterController>();

    cc->Resize(0.2f, 0.2f);
    cc->Set_GravityEnabled(false);
    cc->Set_StepOffset(0.f);
    cc->Set_SlopeLimit(89.f);
    cc->Set_MinMoveDist(0.01f);
    cc->Set_ContactOffset(0.001f);
    cc->Set_RestOffset(0.f);
}

HRESULT COrbitCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CCharacterController>();
    Add_Component<CEventListener>();

    pose.targetRotDeg = Vector2(0.f, profile.startPitchDeg);
    pose.curRotDeg = pose.targetRotDeg;
    pose.wantDist = profile.startDistance;
    pose.goalDist = pose.wantDist;
    pose.curDist = pose.goalDist;
    pose.targetPivot = Vector3::Zero;
    pose.curPivot = pose.targetPivot;
    pose.pivotInternalOffset = Vector3::Zero;
    pose.pivotExternalOffset = Vector3::Zero;

    autoYawCtrl.Reset();
    targetSwitchCtrl.Reset();
    poseSmootherCtrl.Reset();

    ClampTargets();
    return S_OK;
}

HRESULT COrbitCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    auto event = Get_Component<CEventListener>();

    event->Add_Listener<TARGET_LOCK_DESC>([&](TARGET_LOCK_DESC desc)
        {
            if (!desc.tHandle.isValid()) return;
            if (desc.bLock == true) SetLockOn(desc.tHandle);
            else ClearLockOn();
        });

    return S_OK;
}

void COrbitCam::SetTarget(OBJECT_HANDLE handle)
{
    if (!handle.isValid()) return;

    const float keepWantDist = pose.wantDist;

    if (!targetHandle.isValid())
    {
        targetHandle = handle;

        targetSwitchCtrl.Reset();
        autoYawCtrl.OnTargetChanged(profile);

        pose.pivotInternalOffset = Vector3::Zero;

        pose.targetRotDeg = pose.curRotDeg;
        pose.wantDist = clamp(keepWantDist, profile.minDist, profile.maxDist);
        pose.goalDist = clamp(pose.goalDist, profile.minDist, profile.maxDist);
        pose.curDist = clamp(pose.curDist, profile.minDist, profile.maxDist);

        ClampTargets();
        return;
    }

    if (handle == targetHandle) return;

    const Vector3 holdPivotWorld = pose.curPivot;

    targetHandle = handle;
    autoYawCtrl.OnTargetChanged(profile);

    targetSwitchCtrl.BeginSwitch(profile, holdPivotWorld);

    pose.targetRotDeg = pose.curRotDeg;

    pose.wantDist = clamp(keepWantDist, profile.minDist, profile.maxDist);
    pose.goalDist = clamp(pose.goalDist, profile.minDist, profile.maxDist);
    pose.curDist = clamp(pose.curDist, profile.minDist, profile.maxDist);

    ClampTargets();
}

void COrbitCam::ClearTarget()
{
    targetHandle.Reset();
    lockOnCtrl.ForceClear();
    targetSwitchCtrl.Reset();
}

void COrbitCam::SetLockOn(OBJECT_HANDLE handle)
{
    if (!handle.isValid()) return;

    if (!lockOnCtrl.IsActive())
    {
        lockOnCtrl.Enter(handle, pose.goalDist, profile);
        return;
    }

    if (handle == lockOnCtrl.GetHandle()) return;

    lockOnCtrl.Switch(handle);
}

void COrbitCam::ClearLockOn()
{
    if (!lockOnCtrl.IsActiveOrBlending()) return;

    lockOnCtrl.BeginExit(profile);
    autoYawCtrl.OnManualInput(profile);
}

void COrbitCam::SyncFromCurTransform()
{
    const Vector3 pivot = GetPivotTargetPos();
    pose.targetPivot = pivot;
    pose.curPivot = pivot;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    const Vector3 camPos((float)c.x, (float)c.y, (float)c.z);

    Vector3 toPivot = pivot - camPos;
    const float rawDist = toPivot.Length();
    toPivot /= rawDist;

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    pose.curRotDeg.x = XMConvertToDegrees(yawRad);
    pose.curRotDeg.y = XMConvertToDegrees(pitchRad);
    pose.targetRotDeg = pose.curRotDeg;

    pose.curDist = rawDist;
    pose.goalDist = rawDist;
    pose.wantDist = rawDist;

    ClampTargets();

    m_pTransform->Set_WorldPos(Vector4((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::SnapFromCamPose(const Vector3& camPos, const Quaternion& camRot)
{
    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    auto obj = ObjectManager()->Request_Object(targetHandle);
    if (!obj) return;

    auto targetCC = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = targetCC->Get_FootPosition();
    const Vector3 foot{foot4.x, foot4.y, foot4.z};

    const Vector3 basePivot = foot + Vector3(0.f, targetCC->Get_HalfSize() * 1.5f + profile.offsetY, 0.f);

    Vector3 forward = Vector3::Transform(Vector3(0.f, 0.f, 1.f), camRot);
    forward.Normalize();

    float d = (basePivot - camPos).Length();
    d = clamp(d, profile.minDist, profile.maxDist);

    const Vector3 holdPivotWorld = camPos + forward * d;

    targetSwitchCtrl.Reset();

    pose.pivotExternalOffset = Vector3::Zero;
    pose.pivotInternalOffset = holdPivotWorld - basePivot;

    pose.curPivot = holdPivotWorld;
    pose.targetPivot = holdPivotWorld;

    {
        Vector3 toPivot = holdPivotWorld - camPos;
        const float rawDist = toPivot.Length();
        toPivot /= rawDist;

        const float yawRad = atan2f(toPivot.x, toPivot.z);
        const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

        pose.curRotDeg.x = XMConvertToDegrees(yawRad);
        pose.curRotDeg.y = XMConvertToDegrees(pitchRad);
        pose.targetRotDeg = pose.curRotDeg;
    }

    pose.curDist = d;
    pose.goalDist = d;
    pose.wantDist = d;

    m_curMaxYawSpeedDeg = profile.maxYawSpeedDeg;
    m_curMaxPitchSpeedDeg = profile.maxPitchSpeedDeg;

    poseSmootherCtrl.Reset();

    ClampTargets();

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(holdPivotWorld.x, holdPivotWorld.y, holdPivotWorld.z, 1.f));
}

void COrbitCam::CaptureSnapshot(OrbitCamSnapshot& out) const
{
    out.pose = pose;
    lockOnCtrl.Capture(out.lockOn);
    autoYawCtrl.Capture(out.autoYawFollow);
    targetSwitchCtrl.Capture(out.targetSwitch);
    out.targetHandle = targetHandle;
}

void COrbitCam::RestoreSnapshot(const OrbitCamSnapshot& s)
{
    targetHandle = s.targetHandle;
    pose = s.pose;

    lockOnCtrl.Restore(s.lockOn);
    autoYawCtrl.Restore(s.autoYawFollow);
    targetSwitchCtrl.Restore(s.targetSwitch);

    const Vector3 pivot = GetPivotTargetPos();
    pose.targetPivot = pivot;
    pose.curPivot = pivot;

    const float yawRad = XMConvertToRadians(pose.curRotDeg.x);
    const float pitchRad = XMConvertToRadians(pose.curRotDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 camPos = pivot + backDir * pose.curDist;

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    lockOnCtrl.UpdateBlend(0.f);

    OrbitLockOnEvalResult lockRes{};
    if (lockOnCtrl.IsActiveOrBlending())
        lockRes = lockOnCtrl.Evaluate(0.f, profile, targetHandle, pose.curRotDeg.x, pose.curDist, [&](OBJECT_HANDLE h) { return GetBasePivotTargetPos(h); });

    Vector3 lookAt = pivot;
    if (lockRes.weight > 0.f) lookAt = Vector3::Lerp(pivot, lockRes.focusPos, lockRes.weight);

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(lookAt.x, lookAt.y, lookAt.z, 1.f));
}

void COrbitCam::Priority_Update(_float dt)
{
    if (!targetHandle.isValid()) return;

    if (m_freezeRemain > 0.f)
    {
        m_freezeRemain -= dt;
        return;
    }

    if (lockOnCtrl.IsActiveOrBlending() && !lockOnCtrl.GetHandle().isValid())
        ClearLockOn();

    if (targetSwitchCtrl.IsActive())
    {
        const Vector3 basePivotNow = GetBasePivotTargetPos(targetHandle);
        pose.pivotInternalOffset = targetSwitchCtrl.EvaluateInternalOffset(dt, profile, basePivotNow);
    }

    pose.targetPivot = GetPivotTargetPos();
    lockOnCtrl.UpdateBlend(dt);

    {
        const float w = lockOnCtrl.GetWeight();

        const OrbitInputEvalResult inRes =
            inputCtrl.Evaluate(dt, profile, input, w, m_curMaxYawSpeedDeg, m_curMaxPitchSpeedDeg, autoYawCtrl, m_inputLocked);

        pose.targetRotDeg.x += inRes.yawDeltaDeg;
        pose.targetRotDeg.y += inRes.pitchDeltaDeg;
        pose.wantDist -= inRes.zoomDelta;
    }

    OrbitLockOnEvalResult lockRes{};
    if (lockOnCtrl.IsActiveOrBlending())
    {
        lockRes = lockOnCtrl.Evaluate(dt, profile, targetHandle, pose.targetRotDeg.x, pose.wantDist, [&](OBJECT_HANDLE h) { return GetBasePivotTargetPos(h); });

        pose.targetRotDeg.x += lockRes.yawAddDeg;
        if (lockRes.hasDist) pose.wantDist = lockRes.dist;
    }

    if (lockRes.weight <= 0.f)
    {
        const Vector3 foot = GetTargetFootPos();
        const Vector3 camLook = m_pTransform->Dir(STATE::LOOK);
        const Vector3 camRight = m_pTransform->Dir(STATE::RIGHT);

        pose.targetRotDeg.x += autoYawCtrl.EvaluateYawAddDeg(dt, profile, foot, camLook, camRight, pose.targetRotDeg.x);
    }

    ClampTargets();

    {
        auto cc = Get_Component<CCharacterController>();
        auto scene = PhysicsSystem()->Get_Scene();

        const Vector3 pivot = pose.targetPivot;

        const OrbitCollisionDistEvalResult distRes =
            collisionDistCtrl.Evaluate(dt, profile, scene, cc, pivot, pose.wantDist, pose.curRotDeg, pose.targetRotDeg, pose.goalDist);

        m_distConstrained = distRes.constrained;

        m_curMaxYawSpeedDeg = distRes.maxYawSpeedDeg;
        m_curMaxPitchSpeedDeg = distRes.maxPitchSpeedDeg;
        pose.goalDist = distRes.goalDist;
    }

    poseSmootherCtrl.Smooth(dt, profile, pose);
    ApplyOrbitPose(dt, lockRes);
}



void COrbitCam::ClampTargets()
{
    pose.targetRotDeg.y = clamp(pose.targetRotDeg.y, profile.pitchMin, profile.pitchMax);
    pose.wantDist = clamp(pose.wantDist, profile.minDist, profile.maxDist);
}

Vector3 COrbitCam::GetPivotTargetPos() const
{
    const Vector3 basePivot = GetBasePivotTargetPos(targetHandle);
    return basePivot + pose.pivotInternalOffset + pose.pivotExternalOffset;
}

float COrbitCam::GetEffectiveDist() const
{
    if (pose.curDist > profile.maxDist) return profile.maxDist;
    return pose.curDist;
}

void COrbitCam::ApplyOrbitPose(_float dt, const OrbitLockOnEvalResult& lockRes)
{
    const Vector3 pivot = GetPivotPos();

    const float yawRad = XMConvertToRadians(pose.curRotDeg.x);
    const float pitchRad = XMConvertToRadians(pose.curRotDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const float dist = GetEffectiveDist();
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 camPos = pivot + backDir * dist;

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    Vector3 lookAt = pivot;
    if (lockRes.weight > 0.f) lookAt = Vector3::Lerp(pivot, lockRes.focusPos, lockRes.weight);

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(lookAt.x, lookAt.y, lookAt.z, 1.f));
}

Vector3 COrbitCam::GetTargetFootPos() const
{
    auto obj = ObjectManager()->Request_Object(targetHandle);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    return Vector3(foot4.x, foot4.y, foot4.z);
}

Vector3 COrbitCam::GetBasePivotTargetPos(OBJECT_HANDLE handle) const
{
    auto obj = ObjectManager()->Request_Object(handle);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    const Vector3 foot{foot4.x, foot4.y, foot4.z};

    return foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + profile.offsetY, 0.f);
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