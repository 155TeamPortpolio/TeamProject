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

    // 마스크 텍스쳐
    ImGui::SeparatorText(u8"마스크 이미지");
    if (ImGui::Checkbox(u8"사용", &m_isUseMask))
    {
        Get_Component<CSprite2D>()->Set_Param("UseMask", { &m_isUseMask,"bool",sizeof(_bool) });
        CTexture* pTexture = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Texture(G_GlobalLevelKey, szTextureKeys[m_iMaskTextureKeyIndex]);
        Get_Component<CSprite2D>()->Set_Param("OpacityTexture", { pTexture->Get_SRV(), "Texture2D", 0 });
    } 
    ImGui::BeginDisabled(!m_isUseMask);
    ImGui::SetNextWindowSizeConstraints(ImVec2(300.f, 0), ImVec2(300.f, 200.f));
    if (ImGui::Combo(u8"이미지##마스크", &m_iMaskTextureKeyIndex, szTextureKeys.data(), szTextureKeys.size()))
    {
        CTexture* pTexture = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Texture(G_GlobalLevelKey, szTextureKeys[m_iMaskTextureKeyIndex]);
        Get_Component<CSprite2D>()->Set_Param("OpacityTexture", { pTexture->Get_SRV(), "Texture2D", 0 });
    } 
    ImGui::EndDisabled();
}

void CUVAnimationUI::FillElementData(UI_ELEMENT_DATA& data)
{
    __super::FillElementData(data);

    data.strTypeTag = m_strTypeTag;

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    data.strTextureTag = szTextureKeys[m_iTextureKeyIndex];

    data.vUVOffsetSpeed = { m_vUVOffsetSpeed.x, m_vUVOffsetSpeed .y };

    // 마스크는 아직
}

void CUVAnimationUI::ReadElementData(const UI_ELEMENT_DATA& data)
{
    __super::ReadElementData(data);

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    m_iTextureKeyIndex = Find_TextureIndex(szTextureKeys, data.strTextureTag);
    if (-1 != m_iTextureKeyIndex)
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    m_vUVOffsetSpeed = _float2(data.vUVOffsetSpeed[0], data.vUVOffsetSpeed[1]);

    // 마스크는 아직
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