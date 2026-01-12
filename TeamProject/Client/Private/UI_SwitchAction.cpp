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

	return S_OK;
}

void CUI_SwitchAction::Update(_float dt)
{
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('M'))
	//	Set_InteractState(INTERACT_STATE::ENABLED);
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('N'))
	//	Set_InteractState(INTERACT_STATE::DISABLED);
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('B'))
	//	Set_ActionState(ACTION_STATE::READY);
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('V'))
	//	Set_ActionState(ACTION_STATE::UNAVAILABLE);
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('C'))
	//	Start_Execute(EXECUTE_MODE::ANIM, 5);
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('X'))
	//	Start_Execute(EXECUTE_MODE::NONANIM, 3);

	if (m_executeState == EXECUTE_STATE::EXECUTING)
		if (Is_AnimFinished(CHILD::ICON))
			Finish_Execute();

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_SwitchAction::UI_Active(void* pArg)
{
	Set_InteractState(INTERACT_STATE::ENABLED);
}

void CUI_SwitchAction::UI_DeActive(void* pArg)
{
	Set_InteractState(INTERACT_STATE::DISABLED);
}

void CUI_SwitchAction::Set_InteractState(INTERACT_STATE state)
{
	m_interactState = state;
	RefreshVisual();
}

void CUI_SwitchAction::Set_ActionState(ACTION_STATE state)
{
	if (m_interactState != INTERACT_STATE::ENABLED)
		return;

	m_actionState = state;
	RefreshVisual();
}

void CUI_SwitchAction::Start_Execute(EXECUTE_MODE mode, _uint iCount)
{
	if (m_interactState != INTERACT_STATE::ENABLED)
		return;

	if (m_actionState != ACTION_STATE::READY)
		return;

	_float fFillamount = iCount / 6.f;
	auto& handle = m_handles[ENUM(CHILD::GAUGE)];
	if (handle.isValid())
		dynamic_cast<CGaugeUI*>(handle.Get())->Set_FillAmount(fFillamount);

	if (EXECUTE_MODE::NONANIM == mode)
		return;

	m_executeState = EXECUTE_STATE::EXECUTING;
	Set_Animation(CHILD::GROUP, 0);
	Set_Animation(CHILD::ICON, 0);
	RefreshVisual();
}

void CUI_SwitchAction::Finish_Execute()
{
	if (m_executeState != EXECUTE_STATE::EXECUTING)
		return;

	m_executeState = EXECUTE_STATE::IDLE;
	RefreshVisual();
}

void CUI_SwitchAction::RefreshVisual()
{
	if (m_interactState == INTERACT_STATE::DISABLED)
	{
		ApplyDisableVisual();
		return;
	}

	if (m_executeState == EXECUTE_STATE::EXECUTING)
	{
		ApplyExecuteVisual();
		return;
	}

	if (m_actionState == ACTION_STATE::READY)
		ApplyReadyVisual();
	else
		ApplyUnavailableVisual();
}

void CUI_SwitchAction::ApplyDisableVisual()
{
	Set_Color(CHILD::BG, UI_GRAY_MEDIUM);
	Set_Color(CHILD::GAUGEBG, UI_GRAY_DARK);
	Set_Color(CHILD::GAUGE, UI_GRAY_DARK);
	Set_Color(CHILD::ICONBG, UI_GRAY_DARK);
	Set_Color(CHILD::ICON, UI_GRAY_DARK);
	Set_Color(CHILD::SPACE, UI_GRAY_LIGHTEST);
	Set_Alive(CHILD::OUTLINE, true);
}

void CUI_SwitchAction::ApplyExecuteVisual()
{
	Set_Color(CHILD::BG, UI_GRAY_LIGHT);
	Set_Color(CHILD::GAUGEBG, UI_GRAY_DARK); 
	Set_Color(CHILD::GAUGE, UI_GRAY_LIGHTEST);
	Set_Color(CHILD::ICONBG, UI_GRAY_DARK);
	Set_Color(CHILD::ICON, UI_GRAY_DARK);
	Set_Color(CHILD::SPACE, UI_GRAY_LIGHTEST);
	Set_Alive(CHILD::OUTLINE, false);
}

void CUI_SwitchAction::ApplyReadyVisual()
{
	Set_Color(CHILD::BG, UI_GRAY_DARKEST);
	Set_Color(CHILD::GAUGEBG, UI_GRAY_MEDIUM); 
	Set_Color(CHILD::GAUGE, UI_SWITCH_YELLOW);
	Set_Color(CHILD::ICONBG, UI_GRAY_MEDIUM);
	Set_Color(CHILD::ICON, UI_SWITCH_YELLOW);
	Set_Color(CHILD::SPACE, UI_WHITE);
	Set_Alive(CHILD::OUTLINE, false);
}

void CUI_SwitchAction::ApplyUnavailableVisual()
{
	Set_Color(CHILD::BG, UI_GRAY_LIGHT);
	Set_Color(CHILD::GAUGEBG, UI_GRAY_DARK); 
	Set_Color(CHILD::GAUGE, UI_GRAY_LIGHTEST);
	Set_Color(CHILD::ICONBG, UI_GRAY_DARK);
	Set_Color(CHILD::ICON, UI_GRAY_DARK);
	Set_Color(CHILD::SPACE, UI_GRAY_LIGHTEST);
	Set_Alive(CHILD::OUTLINE, false);
}

void CUI_SwitchAction::Set_Alive(CHILD child, _bool isAlive)
{
	ForChild(child, [isAlive](CUI_Object* ui) { ui->Set_Alive(isAlive); });
}

void CUI_SwitchAction::Set_Animation(CHILD child, _int iIndex)
{
	ForChild(child, [iIndex](CUI_Object* ui) { ui->Set_Animation(iIndex); });
}

void CUI_SwitchAction::Set_Color(CHILD child, _float4 vColor)
{
	ForChild(child, [vColor](CUI_Object* ui) { ui->Set_Color(vColor); });
}

_bool CUI_SwitchAction::Is_AnimFinished(CHILD child)
{
	_bool isFinished = {};
	ForChild(child, [&isFinished](CUI_Object* ui) { isFinished = ui->Is_AnimFinished(); });
	return isFinished;
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