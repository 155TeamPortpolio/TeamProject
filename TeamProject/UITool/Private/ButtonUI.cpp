#include "pch.h"
#include "ButtonUI.h"

#include "Sprite2D.h"
#include "UITool_Level.h"

_uint CButtonUI::m_iCount = {};

CButtonUI::CButtonUI()
{
}

CButtonUI::CButtonUI(const CButtonUI& rhs)
    : CUIObject_Tool(rhs)
{
}

HRESULT CButtonUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CButtonUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CButtonUI::Priority_Update(_float dt)
{
}

void CButtonUI::Update(_float dt)
{
}

void CButtonUI::Late_Update(_float dt)
{
}

void CButtonUI::Render_GUI()
{
}

void CButtonUI::ToJson(json& data)
{
}

void CButtonUI::FromJson(const json& data)
{
}

CGameObject* CButtonUI::Create()
{
    CButtonUI* pInstance = new CButtonUI();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CButtonUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CButtonUI::Clone(INIT_DESC* pArg)
{
    CButtonUI* pInstance = new CButtonUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CButtonUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButtonUI::Free()
{
    __super::Free();
}