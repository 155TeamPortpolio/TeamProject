#include "pch.h"
#include "UI_SpecialAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"

const string CUI_SpecialAction::INSTANCENAMES[ENUM(Child::END)] = { "attack" };

HRESULT CUI_SpecialAction::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();
	Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CUI_SpecialAction::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
	const string& filePath = pResourceMgr->Get_ResourcePath("hud_battle_evadeAction.json");
	Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

	for (_int i = 0; i < ENUM(Child::END); ++i)
		m_handles[i] = Get_DescendantHandle(INSTANCENAMES[i]);
	
	return S_OK;
}

void CUI_SpecialAction::Update(_float dt)
{
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_SpecialAction::UI_Active(void* pArg)
{
	Set_Active(true);
}

void CUI_SpecialAction::UI_DeActive(void* pArg)
{
	Set_Active(false);
}

void CUI_SpecialAction::Set_Active(_bool isActive)
{
}

CGameObject* CUI_SpecialAction::Create()
{
	CUI_SpecialAction* pInstance = new CUI_SpecialAction();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_SpecialAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_SpecialAction::Clone(INIT_DESC* pArg)
{
	CUI_SpecialAction* pInstance = new CUI_SpecialAction(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_SpecialAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}