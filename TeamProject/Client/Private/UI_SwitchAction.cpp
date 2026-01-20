#include "pch.h"
#include "UI_SwitchAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "GaugeUI.h"

const string CUI_SwitchAction::INSTANCENAMES[ENUM(CHILD::END)] = { "group", "bg", "gaugeBg", "gauge", "iconBg", "icon", "outline", "space" };

HRESULT CUI_SwitchAction::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();
	Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CUI_SwitchAction::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
	const string& filePath = pResourceMgr->Get_ResourcePath("hud_battle_switchAction.json");
	Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

	for (_int i = 0; i < ENUM(CHILD::END); ++i)
		m_handles[i] = Get_DescendantHandle(INSTANCENAMES[i]);

	// 모드 변경 이벤트
	Get_Component<CEventListener>()->Add_Listener<UI_ACTION_PRIMARY_DESC>([&](const UI_ACTION_PRIMARY_DESC& desc)
		{
			switch (desc.eMode)
			{
			case UI_ACTION_PRIMARY_MODE::ATTACK:
				Set_InteractState(INTERACT_STATE::ENABLE);
				break;
			case UI_ACTION_PRIMARY_MODE::INTERACT:
				Set_InteractState(INTERACT_STATE::DISABLE);
				break;
			}

			m_eMode = desc.eMode;
		});

	// 액션 이벤트
	Get_Component<CEventListener>()->Add_Listener<UI_ACTION_DESC>([&](const UI_ACTION_DESC& desc)
		{
			if (m_eMode == UI_ACTION_PRIMARY_MODE::INTERACT || desc.eType != UI_ACTION_TYPE::SWITCH)
				return;

			if (desc.eState == UI_ACTION_STATE::DISABLE)
				Set_InteractState(INTERACT_STATE::DISABLE);
			else if (desc.eState == UI_ACTION_STATE::ENABLE)
				Set_InteractState(INTERACT_STATE::ENABLE);
			else if (desc.eState == UI_ACTION_STATE::AVAILABLE)	// Executing 애니메이션 실행 중에 들어오면 애니메이션이 덮어써버리는 문제 있음
				Set_InteractState(INTERACT_STATE::AVAILABLE);
			else if (desc.eState == UI_ACTION_STATE::EXECUTING)
			{
				Set_InteractState(INTERACT_STATE::ENABLE);
				Execute(EXECUTE_MODE::ANIM);
				Set_FillAmount(desc.fFillAmount);
			}
		});

	return S_OK;
}

void CUI_SwitchAction::Update(_float dt)
{
	if (!m_isVisualInitialized)
		m_isVisualInitialized = Apply_DisableVisual();

	// 이벤트 테스트 코드
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('M'))
	{
		UI_ACTION_PRIMARY_DESC desc = {};
		desc.eMode = UI_ACTION_PRIMARY_MODE::INTERACT;
	    EventSystem()->Broadcast<UI_ACTION_PRIMARY_DESC>({ desc });
	}

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('N'))
	{
		UI_ACTION_PRIMARY_DESC desc = {};
		desc.eMode = UI_ACTION_PRIMARY_MODE::ATTACK;
		EventSystem()->Broadcast<UI_ACTION_PRIMARY_DESC>({ desc });
	}
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('M'))
	//{
	//    UI_ACTION_DESC desc = {};
	//    desc.eType = UI_ACTION_TYPE::SWITCH;
	//    desc.eState = UI_ACTION_STATE::DISABLE;
	//    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
	//}
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('N'))
	//{
	//    UI_ACTION_DESC desc = {};
	//    desc.eType = UI_ACTION_TYPE::SWITCH;
	//    desc.eState = UI_ACTION_STATE::ENABLE;
	//    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
	//}
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('B'))
	//{
	//    UI_ACTION_DESC desc = {};
	//    desc.eType = UI_ACTION_TYPE::SWITCH;
	//    desc.eState = UI_ACTION_STATE::AVAILABLE;
	//    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
	//}
	// 
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('V'))
	//{
	//	UI_ACTION_DESC desc = {};
	//	desc.eType = UI_ACTION_TYPE::SWITCH;
	//	desc.eState = UI_ACTION_STATE::EXECUTING;
	//	desc.fFillAmount = 0.75f;
	//	EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
	//}

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_SwitchAction::UI_Active(void* pArg)
{
	Set_InteractState(INTERACT_STATE::DISABLE);
}

void CUI_SwitchAction::UI_DeActive(void* pArg)
{
	Set_InteractState(INTERACT_STATE::DISABLE);
}

void CUI_SwitchAction::Set_InteractState(INTERACT_STATE state)
{
	m_interactState = state;
	Refresh_Visual();
}

void CUI_SwitchAction::Execute(EXECUTE_MODE mode)
{
	//if (m_interactState != INTERACT_STATE::ENABLE)
	//	return;

	if (EXECUTE_MODE::NONANIM == mode)
		return;

	//m_interactState = INTERACT_STATE::ENABLE;
	Refresh_Visual();
	Set_Animation(CHILD::GROUP, 0);
	Set_Animation(CHILD::ICON, 0); 
}

void CUI_SwitchAction::Set_FillAmount(_float fFillAmount)
{
	auto& handle = m_handles[ENUM(CHILD::GAUGE)];
	if (handle.isValid())
		dynamic_cast<CGaugeUI*>(handle.Get())->Set_FillAmount(fFillAmount);
}

void CUI_SwitchAction::Refresh_Visual()
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

_bool CUI_SwitchAction::Apply_DisableVisual()
{
	_bool isApplied = {};

	isApplied |= Set_Color(CHILD::BG, UI_GRAY_MEDIUM);
	isApplied |= Set_Color(CHILD::GAUGEBG, UI_GRAY_DARK);
	isApplied |= Set_Color(CHILD::GAUGE, UI_GRAY_DARK);
	isApplied |= Set_Color(CHILD::ICONBG, UI_GRAY_DARK);
	isApplied |= Set_Color(CHILD::ICON, UI_GRAY_DARK);
	isApplied |= Set_Color(CHILD::SPACE, UI_GRAY_LIGHTEST);
	isApplied |= Set_Alive(CHILD::OUTLINE, true);

	return isApplied;
}

void CUI_SwitchAction::Apply_EnableVisual()
{
	Set_Color(CHILD::BG, UI_GRAY_LIGHT);
	Set_Color(CHILD::GAUGEBG, UI_GRAY_DARK);
	Set_Color(CHILD::GAUGE, UI_GRAY_LIGHTEST);
	Set_Color(CHILD::ICONBG, UI_GRAY_DARK);
	Set_Color(CHILD::ICON, UI_GRAY_DARK);
	Set_Color(CHILD::SPACE, UI_GRAY_LIGHTEST);
	Set_Alive(CHILD::OUTLINE, false);
}

void CUI_SwitchAction::Apply_AvailableVisual()
{
	Set_Color(CHILD::BG, UI_GRAY_DARKEST);
	Set_Color(CHILD::GAUGEBG, UI_GRAY_MEDIUM);
	Set_Color(CHILD::GAUGE, UI_SWITCH_YELLOW);
	Set_Color(CHILD::ICONBG, UI_GRAY_MEDIUM);
	Set_Color(CHILD::ICON, UI_SWITCH_YELLOW);
	Set_Color(CHILD::SPACE, UI_WHITE);
	Set_Alive(CHILD::OUTLINE, false);
}

_bool CUI_SwitchAction::Set_Alive(CHILD child, _bool isAlive)
{
	return ForChild(child, [isAlive](CUI_Object* ui) { ui->Set_Alive(isAlive); });
}

_bool CUI_SwitchAction::Set_Animation(CHILD child, _int iIndex)
{
	return ForChild(child, [iIndex](CUI_Object* ui) { ui->Set_Animation(iIndex); });
}

_bool CUI_SwitchAction::Set_Color(CHILD child, _float4 vColor)
{
	return ForChild(child, [vColor](CUI_Object* ui) { ui->Set_Color(vColor); });
}

CGameObject* CUI_SwitchAction::Create()
{
	CUI_SwitchAction* pInstance = new CUI_SwitchAction();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_SwitchAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_SwitchAction::Clone(INIT_DESC* pArg)
{
	CUI_SwitchAction* pInstance = new CUI_SwitchAction(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_SwitchAction");
		Safe_Release(pInstance);
	}
	return pInstance;
}