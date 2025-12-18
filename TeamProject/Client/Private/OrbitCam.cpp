#include "pch.h"
#include "OrbitCam.h"
#include "GameInstance.h"

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

    m_eCamType = CamType::GamePlay;
    m_eRigType = CamRigType::Free;

    m_rotDegCur = m_rotDegTarget;
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
    if (!m_targetHandle.isValid()) return;

    const Vector3 pivot = GetPivotPos();

    const Vector4 curPos4 = m_pTransform->Get_Pos();
    const Vector3 curPos{ curPos4.x, curPos4.y, curPos4.z };

    Vector3 toPivot = pivot - curPos;
    float dist = toPivot.Length();

    if (dist < 1e-6f)
        dist = m_distanceCur;

    m_distanceCur = dist;
    m_distanceTarget = dist;

    if (toPivot.LengthSquared() > 1e-8f)
        toPivot.Normalize();
    else
        toPivot = Vector3(0.f, 0.f, 1.f);

    const float yawRad = atan2f(toPivot.x, toPivot.z);

    float y = -toPivot.y;
    y = clamp(y, -1.f, 1.f);
    const float pitchRad = asinf(y);

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
    if (!m_targetHandle.isValid()) return;

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
        const float dx = input->Mouse_DeltaX();
        const float dy = input->Mouse_DeltaY();

        m_rotDegTarget.x += dx * m_sensitivityX;
        m_rotDegTarget.y += dy * m_sensitivityY;
    }

    const float zoomDelta = m_zoomSpeed * dt;

    if (input->Key_Down('Q')) m_distanceTarget += zoomDelta;
    if (input->Key_Down('E')) m_distanceTarget -= zoomDelta;
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
}

Vector3 COrbitCam::GetPivotPos() const
{
    auto obj = GAME->Get_ObjectMgr()->Request_Object(m_targetHandle);
    if (!obj) return {};

    const Vector4 tpos4 = obj->Get_Component<CTransform>()->Get_Pos();
    const Vector3 tpos{ tpos4.x, tpos4.y, tpos4.z };

    return tpos + Vector3(0.f, m_offsetY, 0.f);
}

float COrbitCam::GetEffectiveDistance() const
{
    float dist = m_distanceCur;

    if (!m_usePitchDolly)
        return clamp(dist, m_distanceMin, m_distanceMax);

    const float pitchAbs = fabsf(m_rotDegCur.y);
    const float pitchLimit = max(fabsf(m_pitchMin), fabsf(m_pitchMax));

    if (pitchLimit <= 1e-6f)
        return clamp(dist, m_distanceMin, m_distanceMax);

    float n = pitchAbs / pitchLimit;
    n = clamp(n, 0.f, 1.f);

    if (n <= m_pitchDollyStartN)
        return clamp(dist, m_distanceMin, m_distanceMax);

    float k = (n - m_pitchDollyStartN) / (1.f - m_pitchDollyStartN);
    k = clamp(k, 0.f, 1.f);

    k = k * k * (3.f - 2.f * k);

    float mul = 1.f - m_pitchDollyStrength * k;
    mul = clamp(mul, 0.1f, 1.f);

    dist *= mul;

    return clamp(dist, m_distanceMin, m_distanceMax);
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

    const Vector4 camPos4{ camPos.x, camPos.y, camPos.z, 1.f };
    const Vector4 pivot4{ pivot.x, pivot.y, pivot.z, 1.f };

    m_pTransform->Set_Pos(camPos4);
    m_pTransform->LookAt(pivot4);
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
