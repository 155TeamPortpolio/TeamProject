#include "pch.h"
#include "FreeCam.h"
#include "GameInstance.h"

HRESULT CFreeCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CFreeCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    SyncRotation();
    return S_OK;
}

void CFreeCam::Priority_Update(_float dt)
{
    if (GUISystem()->UsingUI())
        return;
    auto input = GAME->Get_InputDev();

    if (input->Mouse_Down(MOUSE_BTN::RB))
    {
        const float deltaX = input->Mouse_DeltaX();
        const float deltaY = input->Mouse_DeltaY();

        m_vRotDegTarget.x += deltaX * m_fSensitivity;
        m_vRotDegTarget.y += deltaY * m_fSensitivity;
        m_vRotDegTarget.y = clamp(m_vRotDegTarget.y, -89.f, 89.f);
    }

    ApplyRotation(dt);

    _vector4 look4 = m_pTransform->Dir(STATE::LOOK);
    _vector4 right4 = m_pTransform->Dir(STATE::RIGHT);

    _vector3 look{ look4.x, look4.y, look4.z };
    _vector3 right{ right4.x, right4.y, right4.z };

    const float speed = m_fSpeed * dt;

    _vector3 move{};

    if (input->Key_Down('W')) move += look * speed;
    if (input->Key_Down('S')) move += look * -speed;
    if (input->Key_Down('D')) move += right * speed;
    if (input->Key_Down('A')) move += right * -speed;

    if (move.LengthSquared() > 1e-8f)
        m_pTransform->Translate({ move.x, move.y, move.z, 0.f });
}

void CFreeCam::ApplyRotation(_float dt)
{
    const float yawRad = XMConvertToRadians(m_vRotDegTarget.x);
    const float pitchRad = XMConvertToRadians(m_vRotDegTarget.y);

    m_qRotTarget = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    float alpha = 1.f - expf(-m_fRotSmoothSpeed * dt);
    alpha = clamp(alpha, 0.f, 1.f);

    m_qCurRot = Quaternion::Slerp(m_qCurRot, m_qRotTarget, alpha);
    m_qCurRot.Normalize();

    const _vector4 q4{ m_qCurRot.x, m_qCurRot.y, m_qCurRot.z, m_qCurRot.w };
    m_pTransform->Set_Quaternion(q4);
}

void CFreeCam::SyncRotation()
{
    _vector4 look4 = m_pTransform->Dir(STATE::LOOK);
    _vector3 forward{ look4.x, look4.y, look4.z };

    const float yawRad = atan2f(forward.x, forward.z);

    const float y = clamp(-forward.y, -1.f, 1.f);
    const float pitchRad = asinf(y);

    m_vRotDegTarget.x = XMConvertToDegrees(yawRad);
    m_vRotDegTarget.y = XMConvertToDegrees(pitchRad);
    m_vRotDegTarget.y = clamp(m_vRotDegTarget.y, -89.f, 89.f);

    m_qRotTarget = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);
    m_qCurRot = m_qRotTarget;
}

CFreeCam* CFreeCam::Create()
{
    auto inst = new CFreeCam();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Object Create Failed : CFreeCam");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CFreeCam::Clone(INIT_DESC* pArg)
{
    auto inst = new CFreeCam(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Object Clone Failed : CFreeCam");
        Safe_Release(inst);
    }
    return inst;
}

void CFreeCam::Render_GUI()
{
    __super::Render_GUI();

    if (ImGui::CollapsingHeader(u8"FreeCam", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("FreeCam_RenderGUI");

        ImGui::DragFloat(u8"이동 속도", &m_fSpeed, 0.1f, 0.f, 5000.f);
        ImGui::DragFloat(u8"마우스 감도", &m_fSensitivity, 0.001f, 0.f, 5.f);
        ImGui::DragFloat(u8"회전 스무딩 속도", &m_fRotSmoothSpeed, 0.1f, 0.f, 60.f);

        ImGui::Separator();

        ImGui::DragFloat2(u8"회전 목표(도)", &m_vRotDegTarget.x, 0.05f);
        m_vRotDegTarget.y = clamp(m_vRotDegTarget.y, -89.f, 89.f);

        ImGui::PopID();
    }
}