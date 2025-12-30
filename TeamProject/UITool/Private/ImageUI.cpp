#include "pch.h"
#include "ImageUI.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
#include "Texture.h"
#include "UITool_Level.h"
#include "Engine_Math.h"

_uint CImageUI::m_iCount = {};
const string CImageUI::m_strTypeTag = "Image";

HRESULT CImageUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CImageUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Set_OriginTexSize(true);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    m_strTextureKey = "empty.png";
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, true);

    m_iCount++;

    return S_OK;
}

void CImageUI::Update(_float dt)
{
    if (!m_isAlive) return;

    Get_Component<CSprite2D>()->Set_Param("vFlip", { &m_vFlip, "float2", sizeof(_float2) });

    Play_Animation(dt);
}

void CImageUI::Render_GUI()
{
    __super::Render_GUI();

    // ¿ÃπÃ¡ˆ
    Render_GUI_Image(m_strTextureKey);

    if (ImGui::Checkbox("flip X", &m_isFlipX))
        m_vFlip.x = (m_isFlipX) ? 1.f : 0.f;
    ImGui::SameLine();
    if (ImGui::Checkbox("flip Y", &m_isFlipY))
        m_vFlip.y = (m_isFlipY) ? 1.f : 0.f;
}

void CImageUI::FillElementData(UI_ELEMENT_DATA& data)
{
    __super::FillElementData(data);
    data.strTypeTag    = m_strTypeTag;
    data.strTextureTag = m_strTextureKey;
}

void CImageUI::ReadElementData(const UI_ELEMENT_DATA& data)
{
    __super::ReadElementData(data);

    m_strTextureKey = data.strTextureTag;
    if (m_strTextureKey.empty()) m_strTextureKey = "empty.png";

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_strTextureKey);
}

CGameObject* CImageUI::Create()
{
    CImageUI* pInstance = new CImageUI();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CImageUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CImageUI::Clone(INIT_DESC* pArg)
{
    CImageUI* pInstance = new CImageUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CImageUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}