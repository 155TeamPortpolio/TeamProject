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

    Play_Animation(dt);
}

void CImageUI::Render_GUI()
{
    __super::Render_GUI();

    // ¿ÃπÃ¡ˆ
    Render_GUI_Image(m_strTextureKey);

    // flip
    _bool isFlip = {};
    if (ImGui::Checkbox("flip X", &m_isFlipX))
    {
        m_vFlip.x = (m_isFlipX) ? 1.f : 0.f;
        isFlip = true;
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("flip Y", &m_isFlipY))
    {
        m_vFlip.y = (m_isFlipY) ? 1.f : 0.f;
        isFlip = true;
    }
    if (isFlip)
        Get_Component<CSprite2D>()->Set_Param("vFlip", { &m_vFlip, "float2", sizeof(_float2) });
}

void CImageUI::Save(nlohmann::ordered_json& data)
{
    __super::Save(data);

    data["typeTag"] = m_strTypeTag;
    data["textureTag"] = m_strTextureKey;
}

void CImageUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    m_strTextureKey = data.value("textureTag", "");
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, false);
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