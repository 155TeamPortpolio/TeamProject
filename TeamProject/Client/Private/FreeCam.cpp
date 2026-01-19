#include "pch.h"
#include "FreeCam.h"
#include "GameInstance.h"
#include "Light.h"

HRESULT CFreeCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CFreeCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

void CFreeCam::Priority_Update(_float dt)
{ 
    if (InputDevice()->Mouse_Down(MOUSE_BTN::RB))
    {
        m_vRotDegTarget.x += InputDevice()->Mouse_DeltaX() * m_fSensitivity;
        m_vRotDegTarget.y += InputDevice()->Mouse_DeltaY() * m_fSensitivity;
        m_vRotDegTarget.y = clamp(m_vRotDegTarget.y, -89.f, 89.f);
    }

    ApplyRotation(dt);

    const _vector4 look4 = m_pTransform->Dir(STATE::LOOK);
    const _vector4 right4 = m_pTransform->Dir(STATE::RIGHT);

    const _vector3 look{ look4.x, look4.y, look4.z };
    const _vector3 right{ right4.x, right4.y, right4.z };

    const _vector3 worldUp{0.f, 1.f, 0.f};

    const float speed = m_fSpeed * dt;

    _vector3 move{};
    if (InputDevice()->Key_Down(VK_UP))    move += look    *  speed;
    if (InputDevice()->Key_Down(VK_DOWN))  move += look    * -speed;
    if (InputDevice()->Key_Down(VK_RIGHT)) move += right   *  speed;
    if (InputDevice()->Key_Down(VK_LEFT))  move += right   * -speed;

    if (InputDevice()->Key_Down(VK_CONTROL))
    {
        if (InputDevice()->Key_Down(VK_SPACE)) move += worldUp *  speed;
        if (InputDevice()->Key_Down(VK_SHIFT)) move += worldUp * -speed;
    }

    if (move.LengthSquared() > 0.f)
        m_pTransform->Translate({ move.x, move.y, move.z, 0.f });
}

void CFreeCam::ApplyRotation(_float dt)
{
    const float yawRad = XMConvertToRadians(m_vRotDegTarget.x);
    const float pitchRad = XMConvertToRadians(m_vRotDegTarget.y);

    m_qRotTarget = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    float a = 1.f - expf(-m_fRotSmoothSpeed * dt);
    a = clamp(a, 0.f, 1.f);

    m_qCurRot = Quaternion::Slerp(m_qCurRot, m_qRotTarget, a);
    m_qCurRot.Normalize();

    m_pTransform->Set_Quaternion(_vector4(m_qCurRot.x, m_qCurRot.y, m_qCurRot.z, m_qCurRot.w));
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

        auto myCam = Get_Component<CCamera>();
        bool isMain = (CameraManager()->Get_BaseCam() == myCam);

        if (ImGui::Checkbox(u8"MainCam", &isMain))
        {
            if (isMain)
                CameraManager()->Set_MainCam(myCam);
        }

        ImGui::DragFloat(u8"이동 속도", &m_fSpeed, 0.1f, 0.f, 5000.f);
        ImGui::DragFloat(u8"마우스 감도", &m_fSensitivity, 0.001f, 0.f, 5.f);
        ImGui::DragFloat(u8"회전 스무딩 속도", &m_fRotSmoothSpeed, 0.1f, 0.f, 60.f);

        ImGui::Separator();

        ImGui::DragFloat2(u8"회전 목표(도)", &m_vRotDegTarget.x, 0.05f);
        m_vRotDegTarget.y = clamp(m_vRotDegTarget.y, -89.f, 89.f);

        ImGui::PopID();
    }
}