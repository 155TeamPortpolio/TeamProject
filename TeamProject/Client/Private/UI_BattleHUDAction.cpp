#include "pch.h"
#include "UI_BattleHUDAction.h"

#include "ObjectContainer.h"

HRESULT CUI_BattleHUDAction::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_BattleHUDAction::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Set_Size(_float2(340.f, 224.f));

    Ready_PartObjects();

    return S_OK;
}

void CUI_BattleHUDAction::Update(_float dt)
{
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_BattleHUDAction::Ready_PartObjects()
{
}

CGameObject* CUI_BattleHUDAction::Create()
{
    CUI_BattleHUDAction* pInstance = new CUI_BattleHUDAction();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_BattleHUDAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BattleHUDAction::Clone(INIT_DESC* pArg)
{
    CUI_BattleHUDAction* pInstance = new CUI_BattleHUDAction(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_BattleHUDAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}