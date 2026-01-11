#include "pch.h"
#include "UI_PrimaryAction.h"

HRESULT CUI_PrimaryAction::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUI_PrimaryAction::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CUI_PrimaryAction::Update(_float dt)
{
}

CGameObject* CUI_PrimaryAction::Create()
{
    CUI_PrimaryAction* pInstance = new CUI_PrimaryAction();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_PrimaryAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_PrimaryAction::Clone(INIT_DESC* pArg)
{
    CUI_PrimaryAction* pInstance = new CUI_PrimaryAction(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_PrimaryAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}