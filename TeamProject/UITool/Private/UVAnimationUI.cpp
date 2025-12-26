#include "UITool_Defines.h"
#include "UVAnimationUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
#include "Texture.h"
#include "UITool_Level.h"

_uint CUVAnimationUI::m_iCount = {};
const string CUVAnimationUI::m_strTypeTag = "UVAnimation";

CUVAnimationUI::CUVAnimationUI()
{
}

CUVAnimationUI::CUVAnimationUI(const CUVAnimationUI& rhs)
	: CUIObject_Tool(rhs)
{
}

HRESULT CUVAnimationUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUVAnimationUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->ChangePass("UVAnimation");

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    if (szTextureKeys.size())
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    m_iCount++;

	return S_OK;
}

void CUVAnimationUI::Priority_Update(_float dt)
{
}

void CUVAnimationUI::Update(_float dt)
{
    if (!m_isAlive)
        return;

    Play_Animation(dt);

    m_vUVOffset.x += m_vUVOffsetSpeed.x * dt;
    m_vUVOffset.y += m_vUVOffsetSpeed.y * dt;
    Get_Component<CSprite2D>()->Set_Param("UVOffset", { &m_vUVOffset,"float2",sizeof(_float2) });
}

void CUVAnimationUI::Late_Update(_float dt)
{
}

void CUVAnimationUI::Render_GUI()
{
    __super::Render_GUI();
     

    // 텍스쳐
    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    ImGui::SeparatorText(u8"이미지"); 
    ImGui::SetNextWindowSizeConstraints(ImVec2(300.f, 0), ImVec2(300.f, 200.f));
    if (ImGui::Combo(u8"이미지##메인", &m_iTextureKeyIndex, szTextureKeys.data(), szTextureKeys.size()))
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    // UV 애니메이션
    ImGui::SeparatorText(u8"UV 애니메이션");
    ImGui::DragFloat2(u8"u", reinterpret_cast<_float*>(&m_vUVOffsetSpeed), 0.01f);
}

void CUVAnimationUI::SavePrefab(json& data)
{
    __super::SavePrefab(data);

    data["typeTag"] = m_strTypeTag;

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    data["textureTag"] = szTextureKeys[m_iTextureKeyIndex];

    data["uvOffsetSpeed"]["x"] = m_vUVOffsetSpeed.x;
    data["uvOffsetSpeed"]["y"] = m_vUVOffsetSpeed.y;
}

void CUVAnimationUI::LoadPrefab(const json& data)
{
    __super::LoadPrefab(data);

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    m_iTextureKeyIndex = Find_TextureIndex(szTextureKeys, data["textureTag"]);
    if (-1 != m_iTextureKeyIndex)
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    m_vUVOffsetSpeed.x = data["uvOffsetSpeed"]["x"].get<_float>();
    m_vUVOffsetSpeed.y = data["uvOffsetSpeed"]["y"].get<_float>();
}

CGameObject* CUVAnimationUI::Create()
{
    CUVAnimationUI* pInstance = new CUVAnimationUI();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUVAnimationUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUVAnimationUI::Clone(INIT_DESC* pArg)
{
    CUVAnimationUI* pInstance = new CUVAnimationUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUVAnimationUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUVAnimationUI::Free()
{
    __super::Free();
}