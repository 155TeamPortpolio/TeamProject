#include "pch.h"
#include "OrbitCam.h"
// Engine
#include "GameInstance.h"
#include "Character.h"
#include "Helper_Func.h"
#include "PhysicsSystem.h"
#include "CharacterController.h"
#include "EventListener.h"

void COrbitCam::Awake()
{
    auto cc = Get_Component<Engine::CCharacterController>();

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
    Add_Component<Engine::CCharacterController>();
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

    LockOn_Reset();
    AutoYaw_OnTargetChanged();
    TargetSwitch_Reset();

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

        TargetSwitch_Reset();
        AutoYaw_OnTargetChanged();

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
    AutoYaw_OnTargetChanged();

    TargetSwitch_BeginSwitch(holdPivotWorld);

    pose.targetRotDeg = pose.curRotDeg;

    pose.wantDist = clamp(keepWantDist, profile.minDist, profile.maxDist);
    pose.goalDist = clamp(pose.goalDist, profile.minDist, profile.maxDist);
    pose.curDist = clamp(pose.curDist, profile.minDist, profile.maxDist);

    ClampTargets();
}

void COrbitCam::ClearTarget()
{
    targetHandle.Reset();
    LockOn_ForceClear();
    TargetSwitch_Reset();
}

void COrbitCam::SetLockOn(OBJECT_HANDLE handle)
{
    if (!handle.isValid()) return;

    if (!LockOn_IsActive())
    {
        LockOn_Enter(handle, pose.goalDist);
        return;
    }

    if (handle == LockOn_GetHandle()) return;

    LockOn_Switch(handle);
}

void COrbitCam::ClearLockOn()
{
    if (!LockOn_IsActiveOrBlending()) return;

    LockOn_BeginExit();
    AutoYaw_OnManualInput();
}

void COrbitCam::SyncFromCurTransform()
{
    const Vector3 pivot = GetPivotTargetPos();
    pose.targetPivot = pivot;
    pose.curPivot = pivot;

    auto cc = Get_Component<Engine::CCharacterController>();
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
    auto cc = Get_Component<Engine::CCharacterController>();
    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    auto obj = ObjectManager()->Request_Object(targetHandle);
    auto targetCC = obj->Get_Component<Engine::CCharacterController>();

    const Vector4 foot4 = targetCC->Get_FootPosition();
    const Vector3 foot{foot4.x, foot4.y, foot4.z};

    const Vector3 basePivot = foot + Vector3(0.f, targetCC->Get_HalfSize() * 1.5f + profile.offsetY, 0.f);

    Vector3 forward = Vector3::Transform(Vector3(0.f, 0.f, 1.f), camRot);
    forward.Normalize();

    float d = (basePivot - camPos).Length();
    d = clamp(d, profile.minDist, profile.maxDist);

    const Vector3 holdPivotWorld = camPos + forward * d;

    TargetSwitch_Reset();

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

    ClampTargets();

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(holdPivotWorld.x, holdPivotWorld.y, holdPivotWorld.z, 1.f));
}

void COrbitCam::CaptureSnapshot(OrbitCamSnapshot& out) const
{
    out.pose = pose;
    out.lockOn = m_lockState;
    out.lockOnBlend = m_lockBlend;
    out.autoYaw = m_autoYaw;
    out.targetSwitch = m_targetSwitch;
    out.targetHandle = targetHandle;
}

void COrbitCam::RestoreSnapshot(const OrbitCamSnapshot& s)
{
    targetHandle = s.targetHandle;
    pose = s.pose;

    m_lockState = s.lockOn;
    m_lockBlend = s.lockOnBlend;
    m_autoYaw = s.autoYaw;
    m_targetSwitch = s.targetSwitch;

    m_lockFocusPos = {};
    m_lockHasFocusPos = false;

    const Vector3 pivot = GetPivotTargetPos();
    pose.targetPivot = pivot;
    pose.curPivot = pivot;

    const float yawRad = XMConvertToRadians(pose.curRotDeg.x);
    const float pitchRad = XMConvertToRadians(pose.curRotDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 camPos = pivot + backDir * pose.curDist;

    auto cc = Get_Component<Engine::CCharacterController>();
    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    LockOn_UpdateBlend(0.f);

    OrbitLockOnEvalResult lockRes{};
    if (LockOn_IsActiveOrBlending())
        lockRes = LockOn_Evaluate(0.f, pose.curRotDeg.x, pose.curDist);

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

    if (LockOn_IsActiveOrBlending() && !LockOn_GetHandle().isValid())
        ClearLockOn();

    if (TargetSwitch_IsActive())
    {
        const Vector3 basePivotNow = GetBasePivotTargetPos(targetHandle);
        pose.pivotInternalOffset = TargetSwitch_EvaluateInternalOffset(dt, basePivotNow);
    }

    pose.targetPivot = GetPivotTargetPos();

    LockOn_UpdateBlend(dt);

    {
        const float w = LockOn_GetWeight();
        const OrbitInputEvalResult inRes = Input_Evaluate(dt, w);

        pose.targetRotDeg.x += inRes.yawDeltaDeg;
        pose.targetRotDeg.y += inRes.pitchDeltaDeg;
        pose.wantDist -= inRes.zoomDelta;
    }

    OrbitLockOnEvalResult lockRes{};
    if (LockOn_IsActiveOrBlending())
    {
        lockRes = LockOn_Evaluate(dt, pose.targetRotDeg.x, pose.wantDist);

        pose.targetRotDeg.x += lockRes.yawAddDeg;
        if (lockRes.hasDist) pose.wantDist = lockRes.dist;
    }

    if (lockRes.weight <= 0.f)
    {
        const Vector3 foot = GetTargetFootPos();
        const Vector3 camLook = m_pTransform->Dir(STATE::LOOK);
        const Vector3 camRight = m_pTransform->Dir(STATE::RIGHT);

        pose.targetRotDeg.x += AutoYaw_EvaluateYawAddDeg(dt, foot, camLook, camRight, pose.targetRotDeg.x);
    }

    ClampTargets();

    {
        auto cc = Get_Component<Engine::CCharacterController>();
        auto scene = PhysicsSystem()->Get_Scene();

        const Vector3 pivot = pose.targetPivot;

        const OrbitCollisionDistEvalResult distRes =
            CollisionDist_Evaluate(dt, profile, scene, cc, pivot, pose.wantDist, pose.curRotDeg, pose.targetRotDeg, pose.goalDist);

        m_distConstrained = distRes.constrained;

        m_curMaxYawSpeedDeg = distRes.maxYawSpeedDeg;
        m_curMaxPitchSpeedDeg = distRes.maxPitchSpeedDeg;
        pose.goalDist = distRes.goalDist;
    }

    PoseSmoother_Smooth(dt);
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

    auto cc = Get_Component<Engine::CCharacterController>();
    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    Vector3 lookAt = pivot;
    if (lockRes.weight > 0.f) lookAt = Vector3::Lerp(pivot, lockRes.focusPos, lockRes.weight);

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(lookAt.x, lookAt.y, lookAt.z, 1.f));
}

Vector3 COrbitCam::GetTargetFootPos() const
{
    auto obj = ObjectManager()->Request_Object(targetHandle);
    auto cc = obj->Get_Component<Engine::CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    return Vector3(foot4.x, foot4.y, foot4.z);
}

Vector3 COrbitCam::GetBasePivotTargetPos(OBJECT_HANDLE handle) const
{
    auto obj = ObjectManager()->Request_Object(handle);
    auto cc = obj->Get_Component<Engine::CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    const Vector3 foot{foot4.x, foot4.y, foot4.z};

    return foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + profile.offsetY, 0.f);
}

void COrbitCam::AutoYaw_OnTargetChanged()
{
    m_autoYaw.holdTimer = profile.autoYawFollowDelay;
    m_autoYaw.hasPrevFoot = false;
    m_autoYaw.prevFoot = {};
}

void COrbitCam::AutoYaw_OnManualInput()
{
    m_autoYaw.holdTimer = profile.autoYawFollowDelay;
}

_float COrbitCam::AutoYaw_EvaluateYawAddDeg(_float dt, const Vector3& footWorld, const Vector3& camLookWorld, const Vector3& camRightWorld, _float curTargetYawDeg)
{
    if (!profile.useAutoYawFollow) return 0.f;

    if (m_autoYaw.holdTimer > 0.f)
    {
        m_autoYaw.holdTimer -= dt;
        return 0.f;
    }

    if (!m_autoYaw.hasPrevFoot)
    {
        m_autoYaw.prevFoot = footWorld;
        m_autoYaw.hasPrevFoot = true;
        return 0.f;
    }

    Vector3 delta = footWorld - m_autoYaw.prevFoot;
    m_autoYaw.prevFoot = footWorld;

    delta.y = 0.f;

    const float len = delta.Length();
    if (len == 0.f) return 0.f;

    delta /= len;

    Vector3 camLook = camLookWorld;
    Vector3 camRight = camRightWorld;

    camLook.y = 0.f;
    camRight.y = 0.f;

    camLook.Normalize();
    camRight.Normalize();

    const float localZ = delta.Dot(camLook);
    const float localX = delta.Dot(camRight);

    if (localZ < 0.f && fabsf(localZ) > fabsf(localX)) return 0.f;

    const float desiredYawDeg = XMConvertToDegrees(atan2f(delta.x, delta.z));
    const float deltaYawDeg = Math::WrapDeg(desiredYawDeg - curTargetYawDeg);

    float a = 1.f - expf(-profile.autoYawFollowSpeed * dt);
    a = clamp(a, 0.f, 1.f);

    return deltaYawDeg * a;
}

void COrbitCam::TargetSwitch_Reset()
{
    m_targetSwitch = {};
}

void COrbitCam::TargetSwitch_BeginSwitch(const Vector3& holdPivotWorld)
{
    m_targetSwitch.active = true;
    m_targetSwitch.elapsed = 0.f;
    m_targetSwitch.holdPivotWorld = holdPivotWorld;
}

Vector3 COrbitCam::TargetSwitch_EvaluateInternalOffset(_float dt, const Vector3& basePivotNow)
{
    if (!m_targetSwitch.active) return Vector3::Zero;

    m_targetSwitch.elapsed += dt;

    _float t = m_targetSwitch.elapsed / profile.targetSwitchBlendSec;
    if (t >= 1.f)
    {
        m_targetSwitch.active = false;
        return Vector3::Zero;
    }

    t = clamp(t, 0.f, 1.f);
    t = Math::ApplyEase(profile.targetSwitchEase, t);

    const Vector3 keepOffsetNow = m_targetSwitch.holdPivotWorld - basePivotNow;
    return Vector3::Lerp(keepOffsetNow, Vector3::Zero, t);
}

OrbitInputEvalResult COrbitCam::Input_Evaluate(_float dt, _float lockOnWeight)
{
    OrbitInputEvalResult out{};

    if (m_inputLocked) return out;

#ifdef _USING_GUI
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered()) return out;
#endif

    const float dx = InputDevice()->Mouse_DeltaX();
    const float dy = InputDevice()->Mouse_DeltaY();

    if (lockOnWeight <= 0.f) out.yawDeltaDeg = dx * input.sensitivityX;
    out.pitchDeltaDeg = dy * input.sensitivityY;

    const float wheel = InputDevice()->Mouse_DeltaW() * 0.5f;
    if (wheel != 0.f) out.zoomDelta = wheel * input.zoomSpeed;

    if (dx != 0.f || dy != 0.f || wheel != 0.f) AutoYaw_OnManualInput();

    const float maxYawThisFrame = m_curMaxYawSpeedDeg * dt;
    const float maxPitchThisFrame = m_curMaxPitchSpeedDeg * dt;

    out.yawDeltaDeg = clamp(out.yawDeltaDeg, -maxYawThisFrame, maxYawThisFrame);
    out.pitchDeltaDeg = clamp(out.pitchDeltaDeg, -maxPitchThisFrame, maxPitchThisFrame);

    return out;
}

OrbitCollisionDistEvalResult COrbitCam::CollisionDist_Evaluate(_float dt, const OrbitCamProfile& profile, PxScene* scene, Engine::CCharacterController* camCC,
    const Vector3& pivotWorld, _float wantDist, const Vector2& curRotDeg, const Vector2& targetRotDeg, _float curGoalDist)
{
    OrbitCollisionDistEvalResult out{};

    const _float allowed = CollisionDist_ComputeAllowedDist(profile, scene, camCC, pivotWorld, wantDist, curRotDeg, targetRotDeg);

    out.allowedDist = allowed;
    out.constrained = (allowed < wantDist - 0.001f);

    out.maxYawSpeedDeg = out.constrained ? profile.maxYawSpeedDegWhenColliding : profile.maxYawSpeedDeg;
    out.maxPitchSpeedDeg = out.constrained ? profile.maxPitchSpeedDegWhenColliding : profile.maxPitchSpeedDeg;

    const _float targetDist = min(wantDist, allowed);
    const _float zoomSpeed = (targetDist < curGoalDist) ? profile.collisionZoomInSpeed : profile.collisionZoomOutSpeed;

    _float nextGoal = Math::MoveTowards(curGoalDist, targetDist, zoomSpeed * dt);
    nextGoal = clamp(nextGoal, profile.minDist, profile.maxDist);

    out.goalDist = nextGoal;
    return out;
}

_float COrbitCam::CollisionDist_ComputeAllowedDist(const OrbitCamProfile& profile, PxScene* scene, Engine::CCharacterController* camCC,
    const Vector3& pivotWorld, _float wantDist, const Vector2& curRotDeg, const Vector2& targetRotDeg)
{
    const _float camRadius = camCC->Get_Radius();
    const _float padding = 0.1f;
    const _float stepDeg = 4.f;

    if (!scene) return wantDist;

    const _float startYaw = curRotDeg.x;
    const _float startPitch = curRotDeg.y;

    const _float endYaw = targetRotDeg.x;
    const _float endPitch = targetRotDeg.y;

    const _float deltaYaw = Math::WrapDeg(endYaw - startYaw);
    const _float deltaPitch = endPitch - startPitch;

    const _float maxAbs = max(fabsf(deltaYaw), fabsf(deltaPitch));
    _int steps = (_int)ceilf(maxAbs / stepDeg);
    if (steps < 1)  steps = 1;
    if (steps > 12) steps = 12;

    PxSphereGeometry geom(camRadius);

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER;

    CRaycastFilterCallback filterCallback(camCC->Get_CollisionMask(), false);

    _float minAllowed = wantDist;

    for (_int i = 1; i <= steps; ++i)
    {
        const _float t = (_float)i / (_float)steps;

        const _float yawRad = XMConvertToRadians(startYaw + deltaYaw * t);
        const _float pitchRad = XMConvertToRadians(startPitch + deltaPitch * t);

        const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

        Vector3 dir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
        dir.Normalize();

        PxTransform posePx(PxVec3(pivotWorld.x, pivotWorld.y, pivotWorld.z));

        PxSweepBuffer hit;
        PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

        const _float sweepDist = wantDist + padding;

        const _bool ok = scene->sweep(geom, posePx, PxVec3(dir.x, dir.y, dir.z), sweepDist, hit, hitFlags, filterData, &filterCallback);
        if (!ok || !hit.hasBlock) continue;

        _float allowed = hit.block.distance - padding;
        allowed = clamp(allowed, profile.minDist, wantDist);

        if (allowed < minAllowed) minAllowed = allowed;
    }

    return minAllowed;
}

void COrbitCam::PoseSmoother_Smooth(_float dt)
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

void COrbitCam::LockOn_Reset()
{
    m_lockState = {};
    m_lockBlend = {};
    m_lockFocusPos = {};
    m_lockHasFocusPos = false;
}

void COrbitCam::LockOn_Enter(OBJECT_HANDLE handle, _float curTargetDist)
{
    m_lockState.active = true;
    m_lockState.handle = handle;
    m_lockState.savedTargetDist = curTargetDist;
    LockOn_StartBlend(true);
}

void COrbitCam::LockOn_BeginExit()
{
    if (!LockOn_IsActiveOrBlending()) return;

    if (!m_lockBlend.active)
    {
        LockOn_StartBlend(false);
        return;
    }

    if (m_lockBlend.entering) LockOn_StartBlend(false);
}

void COrbitCam::LockOn_ForceClear()
{
    LockOn_Reset();
}

void COrbitCam::LockOn_UpdateBlend(_float dt)
{
    if (!m_lockBlend.active)
    {
        if (m_lockState.active) m_lockBlend.weight = 1.f;
        else m_lockBlend.weight = 0.f;
        return;
    }

    m_lockBlend.elapsed += dt;

    _float t = m_lockBlend.elapsed / m_lockBlend.duration;
    if (t >= 1.f)
    {
        m_lockBlend.active = false;

        if (m_lockBlend.entering) m_lockBlend.weight = 1.f;
        else
        {
            m_lockBlend.weight = 0.f;
            m_lockState = {};
            m_lockFocusPos = {};
        }
        return;
    }

    t = clamp(t, 0.f, 1.f);
    const _float e = Math::ApplyEase(m_lockBlend.ease, t);

    if (m_lockBlend.entering) m_lockBlend.weight = e;
    else m_lockBlend.weight = 1.f - e;
}

OrbitLockOnEvalResult COrbitCam::LockOn_Evaluate(_float dt, _float curTargetYawDeg, _float curTargetDist)
{
    OrbitLockOnEvalResult out{};

    const _float w = LockOn_GetWeight();
    out.weight = w;

    if (!targetHandle.isValid()) return out;

    const Vector3 playerPivot = GetBasePivotTargetPos(targetHandle);

    if (m_lockHasFocusPos) out.focusPos = m_lockFocusPos;
    else out.focusPos = playerPivot;

    if (m_lockBlend.active && !m_lockBlend.entering)
    {
        out.hasDist = true;
        const _float k = 1.f - w;
        out.dist = curTargetDist + (m_lockState.savedTargetDist - curTargetDist) * k;
    }

    if (w <= 0.f) return out;

    if (!m_lockState.handle.isValid()) return out;

    const Vector3 targetPivot = GetBasePivotTargetPos(m_lockState.handle);

    Vector3 flat = targetPivot - playerPivot;
    flat.y = 0.f;

    const _float len = flat.Length();
    if (len == 0.f) return out;

    flat /= len;

    const _float desiredYawDeg = XMConvertToDegrees(atan2f(flat.x, flat.z));
    const _float deltaYawDeg = Math::WrapDeg(desiredYawDeg - curTargetYawDeg);

    _float a = 1.f - expf(-profile.lockOnYawSpeed * dt);
    a = clamp(a, 0.f, 1.f);

    out.yawAddDeg = deltaYawDeg * a * w;

    _float t = len / (len + profile.lockOnFocusDist);
    t = clamp(t, 0.f, 1.f);

    const _float focusT = profile.lockOnFocusNear + (profile.lockOnFocusFar - profile.lockOnFocusNear) * t;
    out.focusPos = Vector3::Lerp(playerPivot, targetPivot, focusT);

    m_lockFocusPos = out.focusPos;
    m_lockHasFocusPos = true;

    if (profile.lockOnAutoZoom && !out.hasDist)
    {
        const _float wanted = len * profile.lockOnAutoZoomFactor;
        const _float clampedDist = clamp(wanted, profile.minDist, profile.maxDist);

        if (curTargetDist < clampedDist)
        {
            out.hasDist = true;
            out.dist = curTargetDist + (clampedDist - curTargetDist) * w;
        }
    }

    return out;
}

void COrbitCam::LockOn_StartBlend(_bool entering)
{
    m_lockBlend.active = true;
    m_lockBlend.entering = entering;
    m_lockBlend.elapsed = 0.f;

    if (entering)
    {
        m_lockBlend.duration = profile.lockOnBlendInSec;
        m_lockBlend.ease = profile.lockOnBlendInEase;
        m_lockBlend.weight = 0.f;
    }
    else
    {
        m_lockBlend.duration = profile.lockOnBlendOutSec;
        m_lockBlend.ease = profile.lockOnBlendOutEase;
        m_lockBlend.weight = 1.f;
    }

    if (m_lockBlend.duration <= 0.f) m_lockBlend.duration = 0.0001f;
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