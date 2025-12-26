#include "pch.h"
#include "ImageUI.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
#include "UITool_Level.h"
#include "Engine_Math.h"

_uint CImageUI::m_iCount = {};
const string CImageUI::m_strTypeTag = "Image";

CImageUI::CImageUI()
{
}

CImageUI::CImageUI(const CImageUI& rhs)
    : CUIObject_Tool(rhs)
{
}

HRESULT CImageUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CImageUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "empty.png");

    m_iCount++;

    return S_OK;
}

void CImageUI::Priority_Update(_float dt)
{
}

void CImageUI::Update(_float dt)
{
    if (!m_isAlive)
        return;

    Play_Animation(dt);
}

void CImageUI::Late_Update(_float dt)
{
}

void CImageUI::Render_GUI()
{
    __super::Render_GUI();

    // �ؽ���
    ImGui::SeparatorText(u8"�̹���");
    if (ImGui::Button(u8"����"))
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
}

void CImageUI::FillElementData(UI_ELEMENT_DATA& data)
{
    __super::FillElementData(data);

    data.strTypeTag = m_strTypeTag;

    data["textureTag"] = m_strTextureKey;
}

void CImageUI::ReadElementData(const UI_ELEMENT_DATA& data)
{
    __super::ReadElementData(data);

    m_strTextureKey = data["textureTag"];
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

void CImageUI::Free()
{
    __super::Free();
}