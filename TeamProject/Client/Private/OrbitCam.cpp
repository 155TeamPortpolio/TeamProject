#include "pch.h"
#include "OrbitCam.h"
#include "GameInstance.h"
#include "CharacterController.h"
#include "Character.h"

namespace
{
    using Profile = COrbitCam::Profile;

    float WrapDeg(float deg)
    {
        while (deg >  180.f) deg -= 360.f;
        while (deg < -180.f) deg += 360.f;
        return deg;
    }

    Profile FieldPreset()
    {
        Profile p{};
        p.minDist           = 0.6f;
        p.maxDist           = 2.2f;
        p.pitchMin          = -20.f;
        p.pitchMax          = 55.f;
        p.rotSmoothSpeed    = 22.f;
        p.distSmoothSpeed   = 22.f;
        p.pivotSmoothSpeed  = 18.f;
        p.offsetY           = 0.2f;
        p.startDistance     = 2.3f;
        p.startPitchDeg     = -15.f;
        p.startHeightOffset = 0.4f;
        return p;
    }

    Profile BattlePreset()
    {
        Profile p{};
        p.minDist           = 1.2f;
        p.maxDist           = 3.5f;
        p.pitchMin          = -35.f;
        p.pitchMax          = 70.f;
        p.rotSmoothSpeed    = 16.f;
        p.distSmoothSpeed   = 16.f;
        p.pivotSmoothSpeed  = 14.f;
        p.offsetY           = 0.2f;
        p.startDistance     = 3.0f;
        p.startPitchDeg     = -18.f;
        p.startHeightOffset = 0.6f;
        return p;
    }

    Profile GetPreset(OrbitPreset preset)
    {
        if (preset == OrbitPreset::Field) return FieldPreset();
        return BattlePreset();
    }
}

void COrbitCam::Awake()
{
    auto cc = Get_Component<CCharacterController>();
    cc->Resize(0.1f, 0.1f);
    cc->Set_GravityEnabled(false);
}

HRESULT COrbitCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CCharacterController>();
    SetPreset(preset, false, true);
    pose.curRotDeg = pose.targetRotDeg;
    pose.curDist = pose.targetDist;

    return S_OK;
}

HRESULT COrbitCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

void COrbitCam::SetPreset(OrbitPreset nextPreset, _bool keepZoomRatio, _bool snap)
{
    preset = nextPreset;

    const _float oldMin = profile.minDist;
    const _float oldMax = profile.maxDist;

    profile = GetPreset(preset);

    if (keepZoomRatio)
    {
        _float n = (pose.targetDist - oldMin) / (oldMax - oldMin);
        n = clamp(n, 0.f, 1.f);

        const _float newDist = profile.minDist + (profile.maxDist - profile.minDist) * n;
        pose.targetDist = newDist;
        if (snap) pose.curDist = newDist;
    }

    ClampTargets();

    if (snap)
    {
        pose.targetRotDeg = pose.curRotDeg;
        pose.targetDist = pose.curDist;
        pose.targetPivot = pose.curPivot;

        pose.targetPitchZoomOffset = 0.f;
        pose.curPitchZoomOffset = 0.f;
    }
}

void COrbitCam::SetTarget(CGameObject* obj)
{
    targetHandle = obj->Get_Handle();

    if (firstSnap)
    {
        SetTargetFrontView(obj, profile.startDistance, profile.startPitchDeg, profile.startHeightOffset);
        firstSnap = false;
        return;
    }

    const Vector3 pivot = GetPivotTargetPos();
    pose.targetPivot = pivot;
    pose.curPivot = pivot;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    const Vector3 camPos((float)c.x, (float)c.y, (float)c.z);

    Vector3 toPivot = pivot - camPos;
    float dist = toPivot.Length();

    pose.curDist = dist;
    pose.targetDist = dist;

    toPivot /= dist;

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    pose.curRotDeg.x = XMConvertToDegrees(yawRad);
    pose.curRotDeg.y = XMConvertToDegrees(pitchRad);

    pose.targetRotDeg = pose.curRotDeg;

    ClampTargets();
}

void COrbitCam::SyncFromCurTransform()
{
    firstSnap = false;

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

    float zoomOffset = 0.f;
    if (profile.usePitchAutoZoom)
    {
        const float pitchAbs = fabsf(pose.curRotDeg.y);
        const float pitchLimit = max(fabsf(profile.pitchMin), fabsf(profile.pitchMax));
        float n = clamp(pitchAbs / pitchLimit, 0.f, 1.f);

        float k = 0.f;
        if (n > profile.pitchAutoZoomStartN) k = (n - profile.pitchAutoZoomStartN) / (1.f - profile.pitchAutoZoomStartN);

        k = clamp(k, 0.f, 1.f);
        k = k * k * (3.f - 2.f * k);

        zoomOffset = -profile.pitchAutoZoomMax * k;
    }

    pose.curPitchZoomOffset = zoomOffset;
    pose.targetPitchZoomOffset = zoomOffset;

    pose.curDist = rawDist - zoomOffset;
    pose.targetDist = pose.curDist;

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

    Vector3 camPos = pivot + forward * distance;
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

    pose.targetPitchZoomOffset = 0.f;
    pose.curPitchZoomOffset = 0.f;

    ClampTargets();

    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::SnapFromCamPose(const Vector3& camPos, const Quaternion& camRot)
{
    firstSnap = false;

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(XMVectorSet(camPos.x, camPos.y, camPos.z, 1.f));

    auto obj = OBJ->Request_Object(targetHandle);
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

    float zoomOffset = 0.f;
    if (profile.usePitchAutoZoom)
    {
        const float pitchAbs = fabsf(pose.curRotDeg.y);
        const float pitchLimit = max(fabsf(profile.pitchMin), fabsf(profile.pitchMax));
        float n = clamp(pitchAbs / pitchLimit, 0.f, 1.f);

        float k = 0.f;
        if (n > profile.pitchAutoZoomStartN) k = (n - profile.pitchAutoZoomStartN) / (1.f - profile.pitchAutoZoomStartN);

        k = clamp(k, 0.f, 1.f);
        k = k * k * (3.f - 2.f * k);

        zoomOffset = -profile.pitchAutoZoomMax * k;
    }

    pose.curPitchZoomOffset = zoomOffset;
    pose.targetPitchZoomOffset = zoomOffset;

    pose.curDist = rawDist - zoomOffset;
    pose.targetDist = pose.curDist;

    ClampTargets();

    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(desiredPivot.x, desiredPivot.y, desiredPivot.z, 1.f));
}

void COrbitCam::Priority_Update(_float dt)
{
    pose.targetPivot = GetPivotTargetPos();
    
    UpdateInput(dt);
    UpdateAutoYawFollow(dt);
    ClampTargets();
    SmoothStates(dt);
    ApplyOrbitPose(dt);
}

void COrbitCam::UpdateInput(_float dt)
{
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        const float dx = KEY->Mouse_DeltaX();
        const float dy = KEY->Mouse_DeltaY();

        pose.targetRotDeg.x += dx * input.sensitivityX;
        pose.targetRotDeg.y += dy * input.sensitivityY;

        if (dx != 0.f || dy != 0.f) autoYawHoldTimer = profile.autoYawFollowDelay;
    }

    const float zoomDelta = input.zoomSpeed * dt;
    if (KEY->Key_Down('Q')) pose.targetDist += zoomDelta;
    if (KEY->Key_Down('E')) pose.targetDist -= zoomDelta;

    if (!profile.usePitchAutoZoom) { pose.targetPitchZoomOffset = 0.f; return; }

    const float pitchAbs = fabsf(pose.targetRotDeg.y);
    const float pitchLimit = max(fabsf(profile.pitchMin), fabsf(profile.pitchMax));

    float n = clamp(pitchAbs / pitchLimit, 0.f, 1.f);

    float k = 0.f;
    if (n > profile.pitchAutoZoomStartN) k = (n - profile.pitchAutoZoomStartN) / (1.f - profile.pitchAutoZoomStartN);

    k = clamp(k, 0.f, 1.f);
    k = k * k * (3.f - 2.f * k);

    pose.targetPitchZoomOffset = -profile.pitchAutoZoomMax * k;
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

    float zoom = 1.f - expf(-profile.pitchAutoZoomSmooth * dt);
    zoom = clamp(zoom, 0.f, 1.f);
    pose.curPitchZoomOffset = pose.curPitchZoomOffset + (pose.targetPitchZoomOffset - pose.curPitchZoomOffset) * zoom;

    float pivot = 1.f - expf(-profile.pivotSmoothSpeed * dt);
    pivot = clamp(pivot, 0.f, 1.f);
    pose.curPivot = pose.curPivot + (pose.targetPivot - pose.curPivot) * pivot;
}

Vector3 COrbitCam::GetPivotTargetPos() const
{
    auto obj = OBJ->Request_Object(targetHandle);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    const Vector3 foot{foot4.x, foot4.y, foot4.z};

    const Vector3 basePivot = foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + profile.offsetY, 0.f);

    return basePivot + pose.pivotOverrideOffset;
}
 
float COrbitCam::GetEffectiveDist() const
{
    return clamp(pose.curDist + pose.curPitchZoomOffset, profile.minDist, profile.maxDist);
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
    cc->Move_Displacement(XMVectorSet(disp.x, disp.y, disp.z, 0.f), dt);

    const PxExtendedVec3& c1 = cc->Get_Controller()->getPosition();
    m_pTransform->Set_WorldPos(XMVectorSet((float)c1.x, (float)c1.y, (float)c1.z, 1.f));
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
    const float deltaYawDeg = WrapDeg(desiredYawDeg - pose.targetRotDeg.x);

    float a = 1.f - expf(-profile.autoYawFollowSpeed * dt);
    a = clamp(a, 0.f, 1.f);

    pose.targetRotDeg.x += deltaYawDeg * a;
}

Vector3 COrbitCam::GetTargetFootPos() const
{
    auto obj = OBJ->Request_Object(targetHandle);
    auto cc  = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    return Vector3(foot4.x, foot4.y, foot4.z);
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