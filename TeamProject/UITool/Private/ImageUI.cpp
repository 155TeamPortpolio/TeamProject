#include "pch.h"
#include "ImageUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

CImageUI::CImageUI()
{
}

CImageUI::CImageUI(const CImageUI& rhs)
    : CUI_Object(rhs)
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
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "Logo.png");

    return S_OK;
}

void CImageUI::Priority_Update(_float dt)
{
}

void CImageUI::Update(_float dt)
{
}

void CImageUI::Late_Update(_float dt)
{
}

void CImageUI::Render_GUI()
{
    CGameObject::Render_GUI();

    ImGui::SeparatorText("Position");
    if (ImGui::Button("Left"))  m_fLocalX -= 0.1f;
    ImGui::SameLine();
    if (ImGui::Button("Right")) m_fLocalX += 0.1f;
    if (ImGui::Button("Up"))			m_fLocalY -= 0.1f;
    ImGui::SameLine();
    if (ImGui::Button("Down"))    m_fLocalY += 0.1f;

    ImGui::SeparatorText("Scale");
    if (ImGui::Button("Sub X"))       m_fSizeX -= 0.1f;
    ImGui::SameLine();
    if (ImGui::Button("Add X"))         m_fSizeX += 0.1f;
    if (ImGui::Button("Sub Y"))			m_fSizeY -= 0.1f;
    ImGui::SameLine();
    if (ImGui::Button("Add Y"))    m_fSizeY += 0.1f;
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