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

    m_stencilMode = StencilMode::Write;

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
    
    data["stencilMode"] = ENUM(StencilMode::Write);

    data["typeTag"] = m_strTypeTag;
    data["maskTextureKey"] = m_strTextureKey;

    auto sprite = Get_Component<CSprite2D>();
    const string curPass = sprite->Get_PassConstant();

    if (curPass == "UI_StencilWritePreview") data["pass"] = "UI_StencilWrite";
    else                                     data["pass"] = curPass;
}

void CMaskUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    m_strTextureKey = data.value("maskTextureKey", "empty.png");
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, false);

    m_previewVisible = true;
    m_previewAlpha = 0.5f;

    auto sprite = Get_Component<CSprite2D>();
    sprite->Set_Param("MaskPreviewAlpha", {&m_previewAlpha, "float", sizeof(_float)});
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