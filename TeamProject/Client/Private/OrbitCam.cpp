#include "pch.h"
#include "OrbitCam.h"
#include "GameInstance.h"
#include "Light.h"
#include "CharacterController.h"

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

    SetPreset(m_preset, false, true);

    m_curRotDeg = m_targetRotDeg;
    m_curDist   = m_targetDist;

    return S_OK;
}

HRESULT COrbitCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

void COrbitCam::SetPreset(OrbitPreset preset, _bool keepZoomRatio, _bool snap)
{
    m_preset = preset;

    const _float oldMin = m_minDist;
    const _float oldMax = m_maxDist;

    const OrbitProfile p = OrbitPresets::Get(preset);

    m_minDist = p.minDist;
    m_maxDist = p.maxDist;

    m_pitchMin = p.pitchMin;
    m_pitchMax = p.pitchMax;

    m_rotSmoothSpeed   = p.rotSmoothSpeed;
    m_distSmoothSpeed  = p.distSmoothSpeed;
    m_pivotSmoothSpeed = p.pivotSmoothSpeed;

    m_offsetY = p.offsetY;

    m_usePitchAutoZoom    = p.usePitchAutoZoom;
    m_pitchAutoZoomMax    = p.pitchAutoZoomMax;
    m_pitchAutoZoomStartN = p.pitchAutoZoomStartN;
    m_pitchAutoZoomSmooth = p.pitchAutoZoomSmooth;

    if (keepZoomRatio)
    {
        _float n = (m_targetDist - oldMin) / (oldMax - oldMin);
        n = clamp(n, 0.f, 1.f);

        const _float newDist = m_minDist + (m_maxDist - m_minDist) * n;
        m_targetDist = newDist;
        if (snap) m_curDist = newDist;
    }

    ClampTargets();

    if (snap)
    {
        m_targetRotDeg = m_curRotDeg;
        m_targetDist = m_curDist;
        m_targetPivot = m_curPivot;

        m_targetPitchZoomOffset = 0.f;
        m_curPitchZoomOffset = 0.f;
    }
}

void COrbitCam::SetTarget(CGameObject* obj)
{
    m_targetHandle = obj->Get_Handle();

    if (m_firstSnap)
    {
        const OrbitProfile p = OrbitPresets::Get(m_preset);
        SetTargetFrontView(obj, p.startDistance, p.startPitchDeg, p.startHeightOffset);
        m_firstSnap = false;
        return;
    }

    const Vector3 pivotTarget = GetPivotTargetPos();
    m_targetPivot = pivotTarget;
    m_curPivot = pivotTarget;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c.x, (float)c.y, (float)c.z);

    Vector3 toPivot = pivotTarget - curPos;
    float dist = toPivot.Length();
    if (dist == 0.f) dist = m_curDist;

    m_curDist = dist;
    m_targetDist = dist;

    toPivot.Normalize();

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    m_curRotDeg.x = XMConvertToDegrees(yawRad);
    m_curRotDeg.y = XMConvertToDegrees(pitchRad);

    m_targetRotDeg = m_curRotDeg;

    ClampTargets();
}

void COrbitCam::SyncFromCurTransform()
{
    const Vector3 pivotTarget = GetPivotTargetPos();
    m_targetPivot = pivotTarget;
    m_curPivot = pivotTarget;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c.x, (float)c.y, (float)c.z);

    Vector3 toPivot = pivotTarget - curPos;
    float dist = toPivot.Length();
    if (dist == 0.f) dist = m_curDist;

    m_curDist = dist;
    m_targetDist = dist;

    toPivot.Normalize();

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    m_curRotDeg.x = XMConvertToDegrees(yawRad);
    m_curRotDeg.y = XMConvertToDegrees(pitchRad);

    m_targetRotDeg = m_curRotDeg;

    m_targetPitchZoomOffset = 0.f;
    m_curPitchZoomOffset = 0.f;

    ClampTargets();

    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivotTarget.x, pivotTarget.y, pivotTarget.z, 1.f));
}

void COrbitCam::SetTargetFrontView(CGameObject* obj, float distance, float pitchDeg, float heightOffset)
{
    m_targetHandle = obj->Get_Handle();

    const Vector3 pivot = GetPivotTargetPos();
    m_targetPivot = pivot;
    m_curPivot = pivot;

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
    if (dist == 0.f) dist = m_curDist;

    m_curDist = dist;
    m_targetDist = dist;

    toPivot.Normalize();

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    m_curRotDeg.x = XMConvertToDegrees(yawRad);
    m_curRotDeg.y = pitchDeg;

    m_targetRotDeg = m_curRotDeg;

    m_targetPitchZoomOffset = 0.f;
    m_curPitchZoomOffset = 0.f;

    ClampTargets();

    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}


void COrbitCam::Priority_Update(_float dt)
{
    m_targetPivot = GetPivotTargetPos();

    UpdateInput(dt);
    ClampTargets();
    SmoothStates(dt);
    ApplyOrbitPose();
}

void COrbitCam::UpdateInput(_float dt)
{
    if (KEY->Mouse_Down(MOUSE_BTN::RB))
    {
        m_targetRotDeg.x += KEY->Mouse_DeltaX() * m_sensitivityX;
        m_targetRotDeg.y += KEY->Mouse_DeltaY() * m_sensitivityY;
    }

    const float zoomDelta = m_zoomSpeed * dt;
    if (KEY->Key_Down('Q')) m_targetDist += zoomDelta;
    if (KEY->Key_Down('E')) m_targetDist -= zoomDelta;

    if (!m_usePitchAutoZoom) { m_targetPitchZoomOffset = 0.f; return; }

    const float pitchAbs = fabsf(m_targetRotDeg.y);
    const float pitchLimit = max(fabsf(m_pitchMin), fabsf(m_pitchMax));

    float n = clamp(pitchAbs / pitchLimit, 0.f, 1.f);

    float k = 0.f;
    if (n > m_pitchAutoZoomStartN) k = (n - m_pitchAutoZoomStartN) / (1.f - m_pitchAutoZoomStartN);

    k = clamp(k, 0.f, 1.f);
    k = k * k * (3.f - 2.f * k);

    m_targetPitchZoomOffset = -m_pitchAutoZoomMax * k;
}

void COrbitCam::ClampTargets()
{
    m_targetRotDeg.y = clamp(m_targetRotDeg.y, m_pitchMin, m_pitchMax);
    m_targetDist = clamp(m_targetDist, m_minDist, m_maxDist);
}

void COrbitCam::SmoothStates(_float dt)
{
    float aRot = 1.f - expf(-m_rotSmoothSpeed * dt);
    aRot = clamp(aRot, 0.f, 1.f);
    m_curRotDeg = m_curRotDeg + (m_targetRotDeg - m_curRotDeg) * aRot;

    float aDist = 1.f - expf(-m_distSmoothSpeed * dt);
    aDist = clamp(aDist, 0.f, 1.f);
    m_curDist = m_curDist + (m_targetDist - m_curDist) * aDist;

    float aZoom = 1.f - expf(-m_pitchAutoZoomSmooth * dt);
    aZoom = clamp(aZoom, 0.f, 1.f);
    m_curPitchZoomOffset = m_curPitchZoomOffset + (m_targetPitchZoomOffset - m_curPitchZoomOffset) * aZoom;

    float aPivot = 1.f - expf(-m_pivotSmoothSpeed * dt);
    aPivot = clamp(aPivot, 0.f, 1.f);
    m_curPivot = m_curPivot + (m_targetPivot - m_curPivot) * aPivot;
}

Vector3 COrbitCam::GetPivotTargetPos() const
{
    auto obj = OBJ->Request_Object(m_targetHandle);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    const Vector3 foot{foot4.x, foot4.y, foot4.z};

    return foot + Vector3(0.f, cc->Get_HalfSize() * 2.f, 0.f);
}

float COrbitCam::GetEffectiveDist() const
{
    return clamp(m_curDist + m_curPitchZoomOffset, m_minDist, m_maxDist);
}

void COrbitCam::ApplyOrbitPose()
{
    const Vector3 pivot = GetPivotPos();

    const float yawRad = XMConvertToRadians(m_curRotDeg.x);
    const float pitchRad = XMConvertToRadians(m_curRotDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const float dist = GetEffectiveDist();
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 desiredPos = pivot + backDir * dist;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c0 = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c0.x, (float)c0.y, (float)c0.z);

    const Vector3 disp = desiredPos - curPos;
    cc->Move_Displacement(XMVectorSet(disp.x, disp.y, disp.z, 0.f), 1.f);

    const PxExtendedVec3& c1 = cc->Get_Controller()->getPosition();
    m_pTransform->Set_WorldPos(XMVectorSet((float)c1.x, (float)c1.y, (float)c1.z, 1.f));

    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::Render_GUI()
{
    __super::Render_GUI();

    if (ImGui::CollapsingHeader(u8"OrbitCam", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("OrbitCam_RenderGUI");

        auto myCam = Get_Component<CCamera>();
        bool isMain = (CAM->Get_BaseCam() == myCam);

        if (ImGui::Checkbox(u8"MainCam", &isMain))
        {
            if (isMain)
                CAM->Set_MainCam(myCam);
        }

        ImGui::DragFloat(u8"OffsetY", &m_offsetY, 0.01f, -10.f, 10.f);

        ImGui::Separator();

        ImGui::DragFloat2(u8"Rot Target (Deg)", &m_targetRotDeg.x, 0.05f);
        ImGui::DragFloat2(u8"Rot Cur (Deg)", &m_curRotDeg.x, 0.05f);

        ImGui::DragFloat(u8"Distance Target", &m_targetDist, 0.01f, 0.1f, 100.f);
        ImGui::DragFloat(u8"Distance Cur", &m_curDist, 0.01f, 0.1f, 100.f);

        ImGui::Separator();

        ImGui::DragFloat(u8"Pitch Min", &m_pitchMin, 0.1f, -89.f, 0.f);
        ImGui::DragFloat(u8"Pitch Max", &m_pitchMax, 0.1f, 0.f, 89.f);

        ImGui::DragFloat(u8"Dist Min", &m_minDist, 0.01f, 0.1f, 100.f);
        ImGui::DragFloat(u8"Dist Max", &m_maxDist, 0.01f, 0.1f, 200.f);

        ImGui::Separator();

        ImGui::DragFloat(u8"Sens X", &m_sensitivityX, 0.001f, 0.f, 5.f);
        ImGui::DragFloat(u8"Sens Y", &m_sensitivityY, 0.001f, 0.f, 5.f);
        ImGui::DragFloat(u8"Zoom Speed", &m_zoomSpeed, 0.01f, 0.f, 30.f);

        ImGui::DragFloat(u8"Rot Smooth", &m_rotSmoothSpeed, 0.1f, 0.f, 60.f);
        ImGui::DragFloat(u8"Dist Smooth", &m_distSmoothSpeed, 0.1f, 0.f, 60.f);

        ImGui::Separator();

        ImGui::Checkbox(u8"Pitch Dolly", &m_usePitchDolly);
        ImGui::DragFloat(u8"Dolly Strength", &m_pitchDollyStrength, 0.01f, 0.f, 0.8f);
        ImGui::DragFloat(u8"Dolly StartN", &m_pitchDollyStartN, 0.01f, 0.f, 0.95f);

        ImGui::PopID();
    }
}