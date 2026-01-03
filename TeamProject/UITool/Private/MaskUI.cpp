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

    sprite->Set_Param("MaskPreviewAlpha", {&m_previewAlpha, "float", sizeof(_float)});

    m_strTextureKey = "empty.png";
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, true);

    sprite->ChangePass(m_previewVisible ? "UI_StencilWritePreview" : "UI_StencilWrite");

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

    auto sprite = Get_Component<CSprite2D>();

    if (ImGui::Checkbox("Preview Visible", &m_previewVisible))
        sprite->ChangePass(m_previewVisible ? "UI_StencilWritePreview" : "UI_StencilWrite");

    if (ImGui::DragFloat("Preview Alpha", &m_previewAlpha, 0.01f, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
        sprite->Set_Param("MaskPreviewAlpha", {&m_previewAlpha, "float", sizeof(_float)});

    ImGui::TextDisabled(m_previewVisible ? "Pass: UI_StencilWritePreview" : "Pass: UI_StencilWrite");
}

void CMaskUI::Save(nlohmann::ordered_json& data)
{
    __super::Save(data);

    data["typeTag"] = m_strTypeTag;
    data["mask.textureKey"] = m_strTextureKey;
}

void CMaskUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    m_strTextureKey = data.value("mask.textureKey", "empty.png");

    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, false);

    m_previewVisible = true;
    m_previewAlpha = 0.5f;

    auto sprite = Get_Component<CSprite2D>();
    sprite->Set_Param("MaskPreviewAlpha", {&m_previewAlpha, "float", sizeof(_float)});
    sprite->ChangePass("UI_StencilWritePreview");
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