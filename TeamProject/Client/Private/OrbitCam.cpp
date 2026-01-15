#include "pch.h"
#include "OrbitCam.h"
#include "GameInstance.h"
#include "CharacterController.h"
#include "Character.h"
#include "GUIUtil.h"
#include "Helper_Func.h"
#include "EventListener.h"

void COrbitCam::Awake()
{
    auto cc = Get_Component<CCharacterController>();

    cc->Resize(0.2f, 0.2f);
    cc->Set_GravityEnabled(false);
    cc->Set_StepOffset(0.f);
    cc->Set_SlopeLimit(89.f);
    cc->Set_MinMoveDist(0.001f);
    cc->Set_ContactOffset(0.001f);
    cc->Set_RestOffset(0.f);
}

HRESULT COrbitCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CCharacterController>();

    pose.targetRotDeg = Vector2(0.f, profile.startPitchDeg);
    pose.curRotDeg = pose.targetRotDeg;

    pose.targetDist = profile.startDistance;
    pose.curDist = pose.targetDist;

    pose.targetPivot = Vector3::Zero;
    pose.curPivot = pose.targetPivot;

    pose.pivotOverrideOffset = Vector3::Zero;

    ClampTargets();
    return S_OK;
}

HRESULT COrbitCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

void COrbitCam::SetTarget(CGameObject* obj)
{
    const float keepTargetDist = pose.targetDist;
    const _bool hadTarget = targetHandle.isValid();

    autoYawHoldTimer = profile.autoYawFollowDelay;
    hasPrevTargetFoot = false;

    if (!hadTarget)
    {
        targetHandle = obj->Get_Handle();
        targetSwitch = {};
        pose.pivotOverrideOffset = Vector3::Zero;
        SetTargetFrontView(obj, profile.startDistance, profile.startPitchDeg, profile.startHeightOffset);
        return;
    }

    const Vector3 holdPivotWorld = pose.curPivot;

    targetHandle = obj->Get_Handle();

    targetSwitch.active = true;
    targetSwitch.elapsed = 0.f;
    targetSwitch.holdPivotWorld = holdPivotWorld;

    pose.targetRotDeg = pose.curRotDeg;

    pose.targetDist = clamp(keepTargetDist, profile.minDist, profile.maxDist);
    pose.curDist = clamp(pose.curDist, profile.minDist, profile.maxDist);

    ClampTargets();
}

void COrbitCam::SetTarget(OBJECT_HANDLE handle)
{
    SetTarget(ObjectManager()->Request_Object(handle));
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
    pose.targetDist = rawDist;

    ClampTargets();

    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::SetTargetFrontView(CGameObject* obj, float distance, float pitchDeg, float heightOffset)
{
    targetHandle = obj->Get_Handle();

    const Vector3 pivot = GetPivotTargetPos();
    pose.targetPivot = pivot;
    pose.curPivot = pivot;

    auto targetTf = obj->Get_Component<CTransform>();
    Vector3 forward = targetTf->Dir(STATE::LOOK);
    forward.y = 0.f;
    forward.Normalize();

    const float horizontalDist = sqrtf(distance * distance - heightOffset * heightOffset);

    Vector3 camPos = pivot + forward * horizontalDist;
    camPos.y += heightOffset;

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(XMVectorSet(camPos.x, camPos.y, camPos.z, 1.f));

    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c.x, (float)c.y, (float)c.z);

    Vector3 toPivot = pivot - curPos;
    float dist = toPivot.Length();

    pose.curDist = dist;
    pose.targetDist = dist;

    toPivot /= dist;

    const float yawRad = atan2f(toPivot.x, toPivot.z);

    pose.curRotDeg.x = XMConvertToDegrees(yawRad);
    pose.curRotDeg.y = pitchDeg;

    pose.targetRotDeg = pose.curRotDeg;

    ClampTargets();

    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::SnapFromCamPose(const Vector3& camPos, const Quaternion& camRot)
{
    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(XMVectorSet(camPos.x, camPos.y, camPos.z, 1.f));

    auto obj = ObjectManager()->Request_Object(targetHandle);
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

    pose.pivotOverrideOffset = desiredPivot - basePivot;

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
    pose.targetDist = rawDist;

    ClampTargets();

    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(desiredPivot.x, desiredPivot.y, desiredPivot.z, 1.f));
}

void COrbitCam::CaptureSnapshot(OrbitCamSnapshot& out) const
{
    out.pose = pose;
    out.targetSwitch = targetSwitch;
    out.autoYawHoldTimer = autoYawHoldTimer;
    out.prevTargetFoot = prevTargetFoot;
    out.hasPrevTargetFoot = hasPrevTargetFoot;
    out.targetHandle = targetHandle;
}

void COrbitCam::RestoreSnapshot(const OrbitCamSnapshot& s)
{
    targetHandle = s.targetHandle;
    pose = s.pose;
    targetSwitch = s.targetSwitch;
    autoYawHoldTimer = s.autoYawHoldTimer;
    prevTargetFoot = s.prevTargetFoot;
    hasPrevTargetFoot = s.hasPrevTargetFoot;

    const Vector3 pivot = GetPivotTargetPos();
    pose.targetPivot = pivot;
    pose.curPivot = pivot;

    const float yawRad = XMConvertToRadians(pose.curRotDeg.x);
    const float pitchRad = XMConvertToRadians(pose.curRotDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 camPos = pivot + backDir * pose.curDist;

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(XMVectorSet(camPos.x, camPos.y, camPos.z, 1.f));

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::Priority_Update(_float dt)
{
    if (!targetHandle.isValid()) return;
    UpdateTargetSwitch(dt);

    pose.targetPivot = GetPivotTargetPos();
    
    UpdateInput(dt);
    UpdateAutoYawFollow(dt);
    ClampTargets();
    SmoothStates(dt);
    ApplyOrbitPose(dt);
}

void COrbitCam::UpdateInput(_float dt)
{
    auto& io = ImGui::GetIO();

    if (io.WantCaptureMouse || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered()) return;

    const float dx = InputDevice()->Mouse_DeltaX();
    const float dy = InputDevice()->Mouse_DeltaY();

    pose.targetRotDeg.x += dx * input.sensitivityX;
    pose.targetRotDeg.y += dy * input.sensitivityY;

    if (dx != 0.f || dy != 0.f) autoYawHoldTimer = profile.autoYawFollowDelay;

    const float wheel = InputDevice()->Mouse_DeltaW() * 0.5f;
    if (wheel != 0.f) pose.targetDist -= wheel * input.zoomSpeed;
}

void COrbitCam::ClampTargets()
{
    pose.targetRotDeg.y = clamp(pose.targetRotDeg.y, profile.pitchMin, profile.pitchMax);
    pose.targetDist = clamp(pose.targetDist, profile.minDist, profile.maxDist);
}

void COrbitCam::SmoothStates(_float dt)
{
    float rot = 1.f - expf(-profile.rotSmoothSpeed * dt);
    rot = clamp(rot, 0.f, 1.f);
    pose.curRotDeg = pose.curRotDeg + (pose.targetRotDeg - pose.curRotDeg) * rot;

    float dist = 1.f - expf(-profile.distSmoothSpeed * dt);
    dist = clamp(dist, 0.f, 1.f);
    pose.curDist = pose.curDist + (pose.targetDist - pose.curDist) * dist;

    float pivot = 1.f - expf(-profile.pivotSmoothSpeed * dt);
    pivot = clamp(pivot, 0.f, 1.f);
    pose.curPivot = pose.curPivot + (pose.targetPivot - pose.curPivot) * pivot;
}

Vector3 COrbitCam::GetPivotTargetPos() const
{
    const Vector3 basePivot = GetBasePivotTargetPos(targetHandle);
    return basePivot + pose.pivotOverrideOffset;
}
 
float COrbitCam::GetEffectiveDist() const
{
    if (pose.curDist > profile.maxDist) 
        return profile.maxDist;
    return pose.curDist;
}

void COrbitCam::ApplyOrbitPose(_float dt)
{
    const Vector3 pivot = GetPivotPos();

    const float yawRad = XMConvertToRadians(pose.curRotDeg.x);
    const float pitchRad = XMConvertToRadians(pose.curRotDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const float dist = GetEffectiveDist();
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 desiredPos = pivot + backDir * dist;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c0 = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c0.x, (float)c0.y, (float)c0.z);

    const Vector3 disp = desiredPos - curPos;
    cc->Move_Displacement(Vector4(disp.x, disp.y, disp.z, 0.f), dt);

    const PxExtendedVec3& c1 = cc->Get_Controller()->getPosition();
    const Vector3 newPos((float)c1.x, (float)c1.y, (float)c1.z);

    float actualDist = (newPos - pivot).Dot(backDir);
    actualDist = clamp(actualDist, profile.minDist, profile.maxDist);

    float delta = actualDist - pose.curDist;

    float maxStep = 0.f;
    if (delta < 0.f) maxStep = profile.collisionZoomInSpeed * dt;
    else maxStep = profile.collisionZoomOutSpeed * dt;

    delta = clamp(delta, -maxStep, maxStep);
    pose.curDist += delta;

    m_pTransform->Set_WorldPos(Vector4((float)c1.x, (float)c1.y, (float)c1.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::UpdateAutoYawFollow(_float dt)
{
    if (!profile.useAutoYawFollow) return;

    if (autoYawHoldTimer > 0.f)
    {
        autoYawHoldTimer -= dt;
        return;
    }

    const Vector3 foot = GetTargetFootPos();
     
    if (!hasPrevTargetFoot)
    {
        prevTargetFoot    = foot;
        hasPrevTargetFoot = true;
        return;
    }

    Vector3 delta = foot - prevTargetFoot;
    prevTargetFoot = foot;

    delta.y = 0.f;

    const float len = delta.Length();
    if (len == 0.f) return;

    delta /= len;

    Vector3 camLook  = m_pTransform->Dir(STATE::LOOK);
    Vector3 camRight = m_pTransform->Dir(STATE::RIGHT);

    camLook.y  = 0.f;
    camRight.y = 0.f;

    camLook.Normalize();
    camRight.Normalize();

    const float localZ = delta.Dot(camLook);
    const float localX = delta.Dot(camRight);

    if (localZ < 0.f && fabsf(localZ) > fabsf(localX)) return;

    const float desiredYawDeg = XMConvertToDegrees(atan2f(delta.x, delta.z));
    const float deltaYawDeg = Math::WrapDeg(desiredYawDeg - pose.targetRotDeg.x);

    float a = 1.f - expf(-profile.autoYawFollowSpeed * dt);
    a = clamp(a, 0.f, 1.f);

    pose.targetRotDeg.x += deltaYawDeg * a;
}

Vector3 COrbitCam::GetTargetFootPos() const
{
    auto obj = ObjectManager()->Request_Object(targetHandle);
    auto cc  = obj->Get_Component<CCharacterController>();

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
        pose.pivotOverrideOffset = Vector3::Zero;
        return;
    }

    t = clamp(t, 0.f, 1.f);
    t = Math::ApplyEase(profile.targetSwitchEase, t);

    const Vector3 basePivotNow = GetBasePivotTargetPos(targetHandle);
    const Vector3 keepOffsetNow = targetSwitch.holdPivotWorld - basePivotNow;

    pose.pivotOverrideOffset = Vector3::Lerp(keepOffsetNow, Vector3::Zero, t);
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
    __super::Render_GUI();

    if (!ImGui::CollapsingHeader("OrbitCam Debug", ImGuiTreeNodeFlags_DefaultOpen)) return;

    ImGui::Text("Target Valid: %s", targetHandle.isValid() ? "Yes" : "No");
    ImGui::Text("AutoYaw HoldTimer: %.3f", autoYawHoldTimer);

    ImGui::Separator();
    ImGui::Text("Pose");
    ImGui::Text("RotDeg Cur   (Yaw %.3f, Pitch %.3f)", pose.curRotDeg.x, pose.curRotDeg.y);
    ImGui::Text("RotDeg Target(Yaw %.3f, Pitch %.3f)", pose.targetRotDeg.x, pose.targetRotDeg.y);
    ImGui::Text("Dist   Cur   %.4f   Target %.4f", pose.curDist, pose.targetDist);
    ImGui::Text("Pivot  Cur   (%.3f, %.3f, %.3f)", pose.curPivot.x, pose.curPivot.y, pose.curPivot.z);
    ImGui::Text("Pivot  Target(%.3f, %.3f, %.3f)", pose.targetPivot.x, pose.targetPivot.y, pose.targetPivot.z);
    ImGui::Text("Pivot  Offset(%.3f, %.3f, %.3f)", pose.pivotOverrideOffset.x, pose.pivotOverrideOffset.y, pose.pivotOverrideOffset.z);

    ImGui::Separator();
    ImGui::Text("Profile");
    ImGui::Text("Dist Min/Max %.2f / %.2f", profile.minDist, profile.maxDist);
    ImGui::Text("Pitch Min/Max %.2f / %.2f", profile.pitchMin, profile.pitchMax);
    ImGui::Text("Smooth Rot/Dist/Pivot %.2f / %.2f / %.2f", profile.rotSmoothSpeed, profile.distSmoothSpeed, profile.pivotSmoothSpeed);
    ImGui::Text("Collision Zoom In/Out %.2f / %.2f", profile.collisionZoomInSpeed, profile.collisionZoomOutSpeed);

    ImGui::Separator();
    ImGui::Text("CCT");
    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& cpos = cc->Get_Controller()->getPosition();
    const PxExtendedVec3& fpos = cc->Get_Controller()->getFootPosition();
    const Vector3 camPos((float)cpos.x, (float)cpos.y, (float)cpos.z);
    const Vector3 footPos((float)fpos.x, (float)fpos.y, (float)fpos.z);
    const _float3 vel = cc->Get_Velocity();

    ImGui::Text("Pos  (%.3f, %.3f, %.3f)", camPos.x, camPos.y, camPos.z);
    ImGui::Text("Foot (%.3f, %.3f, %.3f)", footPos.x, footPos.y, footPos.z);
    ImGui::Text("Grounded: %s", cc->Is_Grounded() ? "Yes" : "No");
    ImGui::Text("Vel (%.3f, %.3f, %.3f)", vel.x, vel.y, vel.z);
    ImGui::Text("MinMoveDist %.6f", cc->Get_MinMoveDist());
    ImGui::Text("ContactOffset %.6f  RestOffset %.6f", cc->Get_ContactOffset(), cc->Get_RestOffset());

    ImGui::Separator();
    ImGui::Text("Orbit Metrics");
    const Vector3 pivot = GetPivotPos();

    const float yawRad = XMConvertToRadians(pose.curRotDeg.x);
    const float pitchRad = XMConvertToRadians(pose.curRotDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    backDir.Normalize();

    const float distLen = (pivot - camPos).Length();
    const float distProj = (camPos - pivot).Dot(backDir);
    const float distErr = distLen - distProj;

    const Vector3 desiredPos = pivot + backDir * pose.curDist;
    const Vector3 toDesired = desiredPos - camPos;

    ImGui::Text("Pivot (%.3f, %.3f, %.3f)", pivot.x, pivot.y, pivot.z);
    ImGui::Text("BackDir (%.3f, %.3f, %.3f)", backDir.x, backDir.y, backDir.z);
    ImGui::Text("Dist Cur %.4f  Len %.4f  Proj %.4f  (Len-Proj %.6f)", pose.curDist, distLen, distProj, distErr);
    ImGui::Text("DesiredPos (%.3f, %.3f, %.3f)", desiredPos.x, desiredPos.y, desiredPos.z);
    ImGui::Text("ToDesired  (%.3f, %.3f, %.3f)  Len %.4f", toDesired.x, toDesired.y, toDesired.z, toDesired.Length());

    ImGui::Separator();
    ImGui::Text("Target Switch");
    ImGui::Text("Active %s  Elapsed %.3f", targetSwitch.active ? "Yes" : "No", targetSwitch.elapsed);
    ImGui::Text("HoldPivot (%.3f, %.3f, %.3f)", targetSwitch.holdPivotWorld.x, targetSwitch.holdPivotWorld.y, targetSwitch.holdPivotWorld.z);
}
