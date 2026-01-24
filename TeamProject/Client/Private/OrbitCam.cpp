#include "pch.h"
#include "OrbitCam.h"
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
    autoYawCtrl.OnTargetChanged(profile);

    if (!handle.isValid()) return;

    const float keepWantDist = pose.wantDist;

    if (!targetHandle.isValid())
    {
        targetHandle = handle;

        targetSwitch = {};
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

    targetSwitch.active = true;
    targetSwitch.elapsed = 0.f;
    targetSwitch.holdPivotWorld = holdPivotWorld;

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

    Vector3 toBase = basePivot - camPos;
    float d = toBase.Length();
    d = clamp(d, profile.minDist, profile.maxDist);

    const Vector3 desiredPivot = camPos + forward * d;

    pose.pivotInternalOffset = desiredPivot - basePivot;

    pose.targetPivot = desiredPivot;
    pose.curPivot = desiredPivot;

    Vector3 toPivot = desiredPivot - camPos;
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

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(desiredPivot.x, desiredPivot.y, desiredPivot.z, 1.f));
}

void COrbitCam::CaptureSnapshot(OrbitCamSnapshot& out) const
{
    out.pose = pose;
    out.targetSwitch = targetSwitch;
    lockOnCtrl.Capture(out.lockOn);
    autoYawCtrl.Capture(out.autoYawFollow);
    out.targetHandle = targetHandle;
}

void COrbitCam::RestoreSnapshot(const OrbitCamSnapshot& s)
{
    targetHandle = s.targetHandle;
    pose = s.pose;
    targetSwitch = s.targetSwitch;
    lockOnCtrl.Restore(s.lockOn);
    autoYawCtrl.Restore(s.autoYawFollow);

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

    if (lockOnCtrl.IsActiveOrBlending() && !lockOnCtrl.GetHandle().isValid())
        ClearLockOn();

    UpdateTargetSwitch(dt);

    pose.targetPivot = GetPivotTargetPos();

    lockOnCtrl.UpdateBlend(dt);

    UpdateInput(dt);

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
        const Vector3 pivot = pose.targetPivot;
        const float want = pose.wantDist;
        const float allowed = GetCollisionAllowedDist(pivot, want);

        const _bool isCollisionConstrained = (allowed < want - 0.001f);

        m_curMaxYawSpeedDeg = isCollisionConstrained ? profile.maxYawSpeedDegWhenColliding : profile.maxYawSpeedDeg;
        m_curMaxPitchSpeedDeg = isCollisionConstrained ? profile.maxPitchSpeedDegWhenColliding : profile.maxPitchSpeedDeg;

        const float target = min(want, allowed);
        const float speed = (target < pose.goalDist) ? profile.collisionZoomInSpeed : profile.collisionZoomOutSpeed;

        pose.goalDist = Math::MoveTowards(pose.goalDist, target, speed * dt);
        pose.goalDist = clamp(pose.goalDist, profile.minDist, profile.maxDist);
    }

    SmoothStates(dt);
    ApplyOrbitPose(dt, lockRes);
}

void COrbitCam::UpdateInput(_float dt)
{
#ifdef _USING_GUI
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered()) return;
#endif

    const float dx = InputDevice()->Mouse_DeltaX();
    const float dy = InputDevice()->Mouse_DeltaY();

    const float w = lockOnCtrl.GetWeight();

    float yawDeltaDeg = 0.f;
    if (w <= 0.f) yawDeltaDeg = dx * input.sensitivityX;

    float pitchDeltaDeg = dy * input.sensitivityY;

    if (dx != 0.f || dy != 0.f) autoYawCtrl.OnManualInput(profile);

    const float wheel = InputDevice()->Mouse_DeltaW() * 0.5f;
    if (wheel != 0.f) pose.wantDist -= wheel * input.zoomSpeed;

    const float maxYawThisFrame = m_curMaxYawSpeedDeg * dt;
    const float maxPitchThisFrame = m_curMaxPitchSpeedDeg * dt;

    yawDeltaDeg = clamp(yawDeltaDeg, -maxYawThisFrame, maxYawThisFrame);
    pitchDeltaDeg = clamp(pitchDeltaDeg, -maxPitchThisFrame, maxPitchThisFrame);

    pose.targetRotDeg.x += yawDeltaDeg;
    pose.targetRotDeg.y += pitchDeltaDeg;
}

void COrbitCam::ClampTargets()
{
    pose.targetRotDeg.y = clamp(pose.targetRotDeg.y, profile.pitchMin, profile.pitchMax);
    pose.wantDist = clamp(pose.wantDist, profile.minDist, profile.maxDist);
}

void COrbitCam::SmoothStates(_float dt)
{
    float rot = 1.f - expf(-profile.rotSmoothSpeed * dt);
    rot = clamp(rot, 0.f, 1.f);
    pose.curRotDeg = pose.curRotDeg + (pose.targetRotDeg - pose.curRotDeg) * rot;

    float dist = 1.f - expf(-profile.distSmoothSpeed * dt);
    dist = clamp(dist, 0.f, 1.f);
    pose.curDist = pose.curDist + (pose.goalDist - pose.curDist) * dist;

    float pivot = 1.f - expf(-profile.pivotSmoothSpeed * dt);
    pivot = clamp(pivot, 0.f, 1.f);
    pose.curPivot = pose.curPivot + (pose.targetPivot - pose.curPivot) * pivot;
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

void COrbitCam::UpdateTargetSwitch(_float dt)
{
    if (!targetSwitch.active) return;

    targetSwitch.elapsed += dt;

    float t = targetSwitch.elapsed / profile.targetSwitchBlendSec;
    if (t >= 1.f)
    {
        targetSwitch.active = false;
        pose.pivotInternalOffset = Vector3::Zero;
        return;
    }

    t = clamp(t, 0.f, 1.f);
    t = Math::ApplyEase(profile.targetSwitchEase, t);

    const Vector3 basePivotNow = GetBasePivotTargetPos(targetHandle);
    const Vector3 keepOffsetNow = targetSwitch.holdPivotWorld - basePivotNow;

    pose.pivotInternalOffset = Vector3::Lerp(keepOffsetNow, Vector3::Zero, t);
}

_float COrbitCam::GetCollisionAllowedDist(const Vector3& pivot, float wantDist)
{
    auto cc = Get_Component<CCharacterController>();

    const float camRadius = cc->Get_Radius();
    const float padding = 0.1f;
    const float stepDeg = 4.f;

    auto scene = PhysicsSystem()->Get_Scene();
    if (!scene) return wantDist;

    const float startYaw = pose.curRotDeg.x;
    const float startPitch = pose.curRotDeg.y;

    const float endYaw = pose.targetRotDeg.x;
    const float endPitch = pose.targetRotDeg.y;

    const float deltaYaw = Math::WrapDeg(endYaw - startYaw);
    const float deltaPitch = endPitch - startPitch;

    const float maxAbs = max(fabsf(deltaYaw), fabsf(deltaPitch));
    int steps = (int)ceilf(maxAbs / stepDeg);
    if (steps < 1) steps = 1;
    if (steps > 12) steps = 12;

    PxSphereGeometry geom(camRadius);

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER;

    CRaycastFilterCallback filterCallback(cc->Get_CollisionMask(), false);

    float minAllowed = wantDist;

    for (int i = 1; i <= steps; ++i)
    {
        const float t = (float)i / (float)steps;

        const float yawRad = XMConvertToRadians(startYaw + deltaYaw * t);
        const float pitchRad = XMConvertToRadians(startPitch + deltaPitch * t);

        const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

        Vector3 dir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
        dir.Normalize();

        PxVec3 pxOrigin(pivot.x, pivot.y, pivot.z);
        PxVec3 pxDir(dir.x, dir.y, dir.z);

        PxTransform posePx(pxOrigin);

        PxSweepBuffer hit;
        PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

        const float sweepDist = wantDist + padding;

        _bool ok = scene->sweep(geom, posePx, pxDir, sweepDist, hit, hitFlags, filterData, &filterCallback);

        if (!ok || !hit.hasBlock) continue;

        float allowed = hit.block.distance - padding;
        allowed = clamp(allowed, profile.minDist, wantDist);

        if (allowed < minAllowed) minAllowed = allowed;
    }

    return minAllowed;
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