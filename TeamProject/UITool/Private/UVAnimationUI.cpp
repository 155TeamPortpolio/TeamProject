#include "pch.h"
#include "UVAnimationUI.h"
 
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
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

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "empty.png");

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
    ImGui::SeparatorText(u8"이미지");
    if (ImGui::Button(u8"선택"))
    {
        string filePath = Helper::OpenFile_Dialogue();
        if (!filePath.empty())
        {
            string fileName = Helper::GetFileNameWithExtension(filePath);

            CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(fileName, filePath);
            m_strTextureKey = fileName;
            Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_strTextureKey);
        }
    }
    Get_Component<CSprite2D>()->Render_GUI();

    // UV 애니메이션
    ImGui::SeparatorText(u8"UV애니메이션");
    ImGui::DragFloat2(u8"속도", reinterpret_cast<_float*>(&m_vUVOffsetSpeed), 0.01f);
}

void CUVAnimationUI::FillElementData(UI_ELEMENT_DATA& data)
{
    __super::FillElementData(data);

    data.strTypeTag = m_strTypeTag;

    data.strTextureTag = m_strTextureKey;
}

void CUVAnimationUI::ReadElementData(const UI_ELEMENT_DATA& data)
{
    __super::ReadElementData(data);

    m_strTextureKey = data.strTextureTag;
    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_strTextureKey);

    m_vUVOffsetSpeed.x = data.vUVOffsetSpeed[0];
    m_vUVOffsetSpeed.y = data.vUVOffsetSpeed[1];
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