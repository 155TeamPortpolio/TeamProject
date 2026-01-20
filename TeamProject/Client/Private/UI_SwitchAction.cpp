#include "pch.h"
#include "UI_SwitchAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "GaugeUI.h"

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

	Load_Json("hud_battle_switchAction.json");
	Cache_Children();
	Bind_EventListener();

	Apply_DisableVisual();

	return S_OK;
}

void CUI_SwitchAction::Update(_float dt)
{
	__super::Update(dt);

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

void CUI_SwitchAction::Load_Json(const string& resourceKey)
{
	// json 로드
	auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
	const string& filePath = pResourceMgr->Get_ResourcePath(resourceKey);
	Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));
}

void CUI_SwitchAction::Cache_Children()
{
	auto pContainer = Get_Component<CObjectContainer>();

	// 자식 UI 오브젝트 포인터를 배열에 캐싱
	for (_int i = 0; i < ENUM(CHILD::END); ++i)
	{
		const string& strInstanceName = INSTANCENAMES[i];
		if (strInstanceName.empty())
			continue;

		auto pObj = pContainer->Find_Descendant(strInstanceName);
		if (!pObj)
			continue;

		m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
	}

	// 캐스팅이 필요한 자식, 컴포넌트는 별도로 캐싱
	m_pGauge = dynamic_cast<CGaugeUI*>(m_pChildren[ENUM(CHILD::GAUGE)]);
}

void CUI_SwitchAction::Bind_EventListener()
{
	// 모드 변경 이벤트
	Get_Component<CEventListener>()->Add_Listener<UI_ACTION_PRIMARY_DESC>([&](const UI_ACTION_PRIMARY_DESC& desc)
		{
			m_eMode = desc.eMode;
			switch (desc.eMode)
			{
			case UI_ACTION_PRIMARY_MODE::ATTACK:
				Set_InteractState(INTERACT_STATE::ENABLE);
				break;
			case UI_ACTION_PRIMARY_MODE::INTERACT:
				Set_InteractState(INTERACT_STATE::DISABLE);
			}
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
				Set_Gauge(desc.fFillAmount);
			}
		});
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
	Set_ChildAnimation(CHILD::GROUP, 0);
	Set_ChildAnimation(CHILD::ICON, 0); 
}

void CUI_SwitchAction::Set_Gauge(_float fFillAmount)
{
	if (!m_pGauge)
		return;

	m_pGauge->Set_FillAmount(fFillAmount);
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

void CUI_SwitchAction::Apply_DisableVisual()
{
	Set_ChildColor(CHILD::BG, UI_GRAY_MEDIUM);
	Set_ChildColor(CHILD::GAUGEBG, UI_GRAY_DARK);
	Set_ChildColor(CHILD::GAUGE, UI_GRAY_DARK);
	Set_ChildColor(CHILD::ICONBG, UI_GRAY_DARK);
	Set_ChildColor(CHILD::ICON, UI_GRAY_DARK);
	Set_ChildColor(CHILD::SPACE, UI_GRAY_LIGHTEST);
	Set_ChildAlive(CHILD::OUTLINE, true);
}

void CUI_SwitchAction::Apply_EnableVisual()
{
	Set_ChildColor(CHILD::BG, UI_GRAY_LIGHT);
	Set_ChildColor(CHILD::GAUGEBG, UI_GRAY_DARK);
	Set_ChildColor(CHILD::GAUGE, UI_GRAY_LIGHTEST);
	Set_ChildColor(CHILD::ICONBG, UI_GRAY_DARK);
	Set_ChildColor(CHILD::ICON, UI_GRAY_DARK);
	Set_ChildColor(CHILD::SPACE, UI_GRAY_LIGHTEST);
	Set_ChildAlive(CHILD::OUTLINE, false);
}

void CUI_SwitchAction::Apply_AvailableVisual()
{
	Set_ChildColor(CHILD::BG, UI_GRAY_DARKEST);
	Set_ChildColor(CHILD::GAUGEBG, UI_GRAY_MEDIUM);
	Set_ChildColor(CHILD::GAUGE, UI_SWITCH_YELLOW);
	Set_ChildColor(CHILD::ICONBG, UI_GRAY_MEDIUM);
	Set_ChildColor(CHILD::ICON, UI_SWITCH_YELLOW);
	Set_ChildColor(CHILD::SPACE, UI_WHITE);
	Set_ChildAlive(CHILD::OUTLINE, false);
}

void CUI_SwitchAction::Set_ChildAlive(CHILD child, _bool isAlive)
{
	if (!m_pChildren[ENUM(child)])
		return;

	m_pChildren[ENUM(child)]->Set_Alive(isAlive);
}

void CUI_SwitchAction::Set_ChildColor(CHILD child, _float4 vColor)
{
	if (!m_pChildren[ENUM(child)])
		return;

	m_pChildren[ENUM(child)]->Set_Color(vColor);
}

void CUI_SwitchAction::Set_ChildAnimation(CHILD child, _int iIndex)
{
	if (!m_pChildren[ENUM(child)])
		return;

	m_pChildren[ENUM(child)]->Set_Animation(iIndex);
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