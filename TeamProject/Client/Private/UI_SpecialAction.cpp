#include "pch.h"
#include "UI_SpecialAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "UI_SwitchAction.h"

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

	Load_Json("hud_battle_specialAction.json");
	Cache_Children();
	Bind_EventListener();

	return S_OK;
}

void CUI_SpecialAction::Update(_float dt)
{
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_SpecialAction::UI_Active(void* pArg)
{
	Set_InteractState(INTERACT_STATE::ENABLE);
}

void CUI_SpecialAction::UI_DeActive(void* pArg)
{
	Set_InteractState(INTERACT_STATE::DISABLE);
}

void CUI_SpecialAction::Load_Json(const string& resourceKey)
{
	// json 로드
	auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
	const string& filePath = pResourceMgr->Get_ResourcePath(resourceKey);
	Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));
}

void CUI_SpecialAction::Cache_Children()
{
	// 자식 UI 오브젝트 포인터를 배열에 캐싱
	for (_int i = 0; i < ENUM(CHILD::END); ++i)
		m_pChildren[i] = dynamic_cast<CUI_Object*>(Get_Component<CObjectContainer>()->Find_Descendant(INSTANCENAMES[i]));
}

void CUI_SpecialAction::Bind_EventListener()
{
	// 모드 변경 이벤트
	Get_Component<CEventListener>()->Add_Listener<UI_ACTION_PRIMARY_DESC>([&](const UI_ACTION_PRIMARY_DESC& desc)
		{
			if (desc.eMode == UI_ACTION_PRIMARY_MODE::INTERACT)
				Set_InteractState(INTERACT_STATE::DISABLE);
			else
				Set_InteractState(INTERACT_STATE::ENABLE);
		});

	// 액션 이벤트
	Get_Component<CEventListener>()->Add_Listener<UI_ACTION_DESC>([&](const UI_ACTION_DESC& desc)
		{
			if (desc.eType != UI_ACTION_TYPE::SPECIAL)
				return;

			if (desc.eState == UI_ACTION_STATE::DISABLE)
				Set_InteractState(INTERACT_STATE::DISABLE);
			else if (desc.eState == UI_ACTION_STATE::ENABLE)
				Set_InteractState(INTERACT_STATE::ENABLE);
			else if (desc.eState == UI_ACTION_STATE::AVAILABLE)
				Set_InteractState(INTERACT_STATE::AVAILABLE);
			else if (desc.eState == UI_ACTION_STATE::EXECUTING)
				Execute();
		});
}

void CUI_SpecialAction::Set_InteractState(INTERACT_STATE state)
{
	m_interactState = state;
	Refresh_Visual();
}

void CUI_SpecialAction::Execute()
{
	//if (m_interactState != INTERACT_STATE::AVAILABLE)
	//    return;

	m_interactState = INTERACT_STATE::AVAILABLE;
	Refresh_Visual();
	Set_Animation(CHILD::GROUP, 0);
	Set_Animation(CHILD::UV, 0);
	Set_Alive(CHILD::ACTIVE, false);
	Set_Alive(CHILD::MASK, true);
}

void CUI_SpecialAction::Refresh_Visual()
{
	if (m_interactState == INTERACT_STATE::DISABLE)
	{
		Apply_DisableVisual();
		return;
	}

	if (m_interactState == INTERACT_STATE::AVAILABLE)
	{
		Apply_AvailableVisual();
		return;
	}

	Apply_EnableVisual();
}

void CUI_SpecialAction::Apply_DisableVisual()
{
	Set_Color(CHILD::BG, UI_GRAY_MEDIUM);
	Set_Color(CHILD::ICON, UI_GRAY_LIGHT);
	Set_Color(CHILD::E, UI_GRAY_LIGHTEST);
	Set_Alive(CHILD::ACTIVE, false);
}

void CUI_SpecialAction::Apply_EnableVisual()
{
	Set_Color(CHILD::BG, UI_GRAY_DARKEST);
	Set_Color(CHILD::ICON, UI_WHITE);
	Set_Color(CHILD::E, UI_GRAY_LIGHTEST);
	Set_Alive(CHILD::ACTIVE, false);
}

void CUI_SpecialAction::Apply_AvailableVisual()
{
	Set_Color(CHILD::BG, UI_GRAY_DARKEST);
	Set_Color(CHILD::ICON, UI_WHITE);
	Set_Color(CHILD::E, UI_WHITE);
	Set_Alive(CHILD::ACTIVE, true);
	Set_Alive(CHILD::MASK, false);
	Set_Animation(CHILD::BLINK, 0);
}

void CUI_SpecialAction::Set_Alive(CHILD child, _bool isAlive)
{
	if (!m_pChildren[ENUM(child)])
		return;

	m_pChildren[ENUM(child)]->Set_Alive(isAlive);
}

void CUI_SpecialAction::Set_Color(CHILD child, _float4 vColor)
{
	if (!m_pChildren[ENUM(child)])
		return;

	m_pChildren[ENUM(child)]->Set_Color(vColor);
}

void CUI_SpecialAction::Set_Animation(CHILD child, _int iIndex)
{
	if (!m_pChildren[ENUM(child)])
		return;

	m_pChildren[ENUM(child)]->Set_Animation(iIndex);
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