#include "pch.h"
#include "OrbitCam.h"
#include "GameInstance.h"
#include "Light.h"

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

void COrbitCam::Free()
{
   __super::Free(); 
}

HRESULT COrbitCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    m_rotDegCur   = m_rotDegTarget;
    m_distanceCur = m_distanceTarget;

    return S_OK;
}

HRESULT COrbitCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

void COrbitCam::SetTarget(CGameObject* obj)
{
    m_targetHandle = obj->Get_Handle();

    const Vector3 pivotTarget = GetPivotTargetPos();
    m_pivotTarget = pivotTarget;
    m_pivotCur = pivotTarget;

    const Vector4 curPos4 = m_pTransform->Get_Pos();
    const Vector3 curPos{ curPos4.x, curPos4.y, curPos4.z };

    Vector3 toPivot = pivotTarget - curPos;
    float dist = toPivot.Length();
    if (dist == 0.f) dist = m_distanceCur;

    m_distanceCur = dist;
    m_distanceTarget = dist;

    toPivot.Normalize();

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    m_rotDegCur.x = XMConvertToDegrees(yawRad);
    m_rotDegCur.y = XMConvertToDegrees(pitchRad);

    m_rotDegTarget = m_rotDegCur;

    ClampTargets();
}

void COrbitCam::ClearTarget()
{
    m_targetHandle.Reset();
}

void COrbitCam::Priority_Update(_float dt)
{
    m_pivotTarget = GetPivotTargetPos();

    UpdateInput(dt);
    ClampTargets();
    SmoothStates(dt);
    ApplyOrbitPose();
}

void COrbitCam::UpdateInput(_float dt)
{
    auto input = GAME->Get_InputDev();

    if (input->Mouse_Down(MOUSE_BTN::RB))
    {
        m_rotDegTarget.x += input->Mouse_DeltaX() * m_sensitivityX;
        m_rotDegTarget.y += input->Mouse_DeltaY() * m_sensitivityY;
    }

    const float zoomDelta = m_zoomSpeed * dt;
    if (input->Key_Down('Q')) m_distanceTarget += zoomDelta;
    if (input->Key_Down('E')) m_distanceTarget -= zoomDelta;

    if (!m_usePitchAutoZoom) { m_pitchZoomOffsetTarget = 0.f; return; }

    const float pitchAbs = fabsf(m_rotDegTarget.y);
    const float pitchLimit = max(fabsf(m_pitchMin), fabsf(m_pitchMax));

    float n = clamp(pitchAbs / pitchLimit, 0.f, 1.f);

    float k = 0.f;
    if (n > m_pitchAutoZoomStartN)
        k = (n - m_pitchAutoZoomStartN) / (1.f - m_pitchAutoZoomStartN);

    k = clamp(k, 0.f, 1.f);
    k = k * k * (3.f - 2.f * k);

    m_pitchZoomOffsetTarget = -m_pitchAutoZoomMax * k;
}

void COrbitCam::ClampTargets()
{
    m_rotDegTarget.y = clamp(m_rotDegTarget.y, m_pitchMin, m_pitchMax);
    m_distanceTarget = clamp(m_distanceTarget, m_distanceMin, m_distanceMax);
}

void COrbitCam::SmoothStates(_float dt)
{
    float aRot = 1.f - expf(-m_rotSmoothSpeed * dt);
    aRot = clamp(aRot, 0.f, 1.f);
    m_rotDegCur = m_rotDegCur + (m_rotDegTarget - m_rotDegCur) * aRot;

    float aDist = 1.f - expf(-m_distSmoothSpeed * dt);
    aDist = clamp(aDist, 0.f, 1.f);
    m_distanceCur = m_distanceCur + (m_distanceTarget - m_distanceCur) * aDist;

    float aZoom = 1.f - expf(-m_pitchAutoZoomSmooth * dt);
    aZoom = clamp(aZoom, 0.f, 1.f);
    m_pitchZoomOffsetCur = m_pitchZoomOffsetCur + (m_pitchZoomOffsetTarget - m_pitchZoomOffsetCur) * aZoom;

    float aPivot = 1.f - expf(-m_pivotSmoothSpeed * dt);
    aPivot = clamp(aPivot, 0.f, 1.f);
    m_pivotCur = m_pivotCur + (m_pivotTarget - m_pivotCur) * aPivot;
}

Vector3 COrbitCam::GetPivotPos() const
{
    return m_pivotCur;
}

Vector3 COrbitCam::GetPivotTargetPos() const
{
    auto obj = OBJ->Request_Object(m_targetHandle);
    const Vector4 p4 = obj->Get_Component<CTransform>()->Get_Pos();
    return Vector3(p4.x, p4.y, p4.z) + Vector3(0.f, m_offsetY, 0.f);
}

float COrbitCam::GetEffectiveDistance() const
{
    return clamp(m_distanceCur + m_pitchZoomOffsetCur, m_distanceMin, m_distanceMax);
}

void COrbitCam::ApplyOrbitPose()
{
    const Vector3 pivot = GetPivotPos();

    const float yawRad = XMConvertToRadians(m_rotDegCur.x);
    const float pitchRad = XMConvertToRadians(m_rotDegCur.y);

    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const float dist = GetEffectiveDistance();
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 camPos = pivot + backDir * dist;

    m_pTransform->Set_Pos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::Render_GUI()
{
    __super::Render_GUI();

    if (ImGui::CollapsingHeader(u8"OrbitCam", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("OrbitCam_RenderGUI");

        ImGui::DragFloat(u8"OffsetY", &m_offsetY, 0.01f, -10.f, 10.f);

        ImGui::Separator();

        ImGui::DragFloat2(u8"Rot Target (Deg)", &m_rotDegTarget.x, 0.05f);
        ImGui::DragFloat2(u8"Rot Cur (Deg)", &m_rotDegCur.x, 0.05f);

        ImGui::DragFloat(u8"Distance Target", &m_distanceTarget, 0.01f, 0.1f, 100.f);
        ImGui::DragFloat(u8"Distance Cur", &m_distanceCur, 0.01f, 0.1f, 100.f);

        ImGui::Separator();

        ImGui::DragFloat(u8"Pitch Min", &m_pitchMin, 0.1f, -89.f, 0.f);
        ImGui::DragFloat(u8"Pitch Max", &m_pitchMax, 0.1f, 0.f, 89.f);

        ImGui::DragFloat(u8"Dist Min", &m_distanceMin, 0.01f, 0.1f, 100.f);
        ImGui::DragFloat(u8"Dist Max", &m_distanceMax, 0.01f, 0.1f, 200.f);

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
