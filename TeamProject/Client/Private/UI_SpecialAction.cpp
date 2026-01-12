#include "pch.h"
#include "UI_SpecialAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "UI_SwitchAction.h"

const string CUI_SpecialAction::INSTANCENAMES[ENUM(CHILD::END)] = { "bg", "icon", "group", "mask", "uv", "active", "blink", "e" };

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
	const string& filePath = pResourceMgr->Get_ResourcePath("hud_battle_specialAction.json");
	Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

	for (_int i = 0; i < ENUM(CHILD::END); ++i)
		m_handles[i] = Get_DescendantHandle(INSTANCENAMES[i]);
	
	return S_OK;
}

void CUI_SpecialAction::Update(_float dt)
{
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('M'))
	//	Set_Enabled(true);
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('N'))
	//	Set_Enabled(false);
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('B'))
	//	Set_Ready();
	//
	//if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('V'))
	//	Use();

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_SpecialAction::UI_Active(void* pArg)
{
	Set_Enabled(true);
}

void CUI_SpecialAction::UI_DeActive(void* pArg)
{
	Set_Enabled(false);
}

void CUI_SpecialAction::Set_Enabled(_bool isEnabled)
{
	m_isEnabled = isEnabled;

	if (isEnabled)
	{
		Set_Color(CHILD::BG, UI_GRAY_DARKEST);
		Set_Color(CHILD::ICON, UI_WHITE);
		Set_Color(CHILD::E, UI_WHITE);
	}
	else
	{
		Set_Color(CHILD::BG, UI_GRAY_MEDIUM);
		Set_Color(CHILD::ICON, UI_GRAY_LIGHT);
		Set_Color(CHILD::E, UI_GRAY_LIGHTEST);
	}

	Set_Alive(CHILD::ACTIVE, false);
}

void CUI_SpecialAction::Set_Ready()
{
	if (!m_isEnabled)
		return;

	m_isReady = true;
	Set_Animation(CHILD::GROUP, 0);
	Set_Animation(CHILD::UV, 0);
	Set_Alive(CHILD::ACTIVE, false);
	Set_Alive(CHILD::MASK, true);
	Set_Animation(CHILD::BLINK, 0);
}

void CUI_SpecialAction::Use()
{
	if (!m_isEnabled || !m_isReady)
		return;

	m_isReady = false;
	Set_Alive(CHILD::ACTIVE, true);
	Set_Alive(CHILD::MASK, false);
	Set_Animation(CHILD::BLINK, 0);
}

void CUI_SpecialAction::Set_Alive(CHILD child, _bool isAlive)
{
	ForChild(child, [isAlive](CUI_Object* ui) { ui->Set_Alive(isAlive); });
}

void CUI_SpecialAction::Set_Animation(CHILD child, _int iIndex)
{
	ForChild(child, [iIndex](CUI_Object* ui) { ui->Set_Animation(iIndex); });
}

void CUI_SpecialAction::Set_Color(CHILD child, _float4 vColor)
{
	ForChild(child, [vColor](CUI_Object* ui) { ui->Set_Color(vColor); });
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