#include "pch.h"
#include "MaskUI.h"

#include "Sprite2D.h"

HRESULT CMaskUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CMaskUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Set_OriginTexSize(true);

    auto sprite = Get_Component<CSprite2D>();
    sprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    sprite->ChangePass("UI_StencilWrite");
    sprite->Set_Param("MaskThreshold", {&m_maskThreshold, "float", sizeof(_float)});

    m_strTextureKey = "empty.png";
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, true);

    Set_Priority(0);

    m_iCount++;
    return S_OK;
}

void CMaskUI::Update(_float dt)
{
    if (!m_isAlive) return;
    Play_Animation(dt);
}

void CMaskUI::Render_GUI()
{
    __super::Render_GUI();

    Render_GUI_Image(m_strTextureKey);

    ImGui::SeparatorText("Mask");
    ImGui::DragFloat("MaskThreshold", &m_maskThreshold, 0.01f, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::TextDisabled("Pass: UI_StencilWrite (Alpha clip -> Stencil)");
}

void CMaskUI::Save(nlohmann::ordered_json& data)
{
    __super::Save(data);
}

void CMaskUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);
}

CGameObject* CMaskUI::Create()
{
    CMaskUI* pInstance = new CMaskUI();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CMaskUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMaskUI::Clone(INIT_DESC* pArg)
{
    CMaskUI* pInstance = new CMaskUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CMaskUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}