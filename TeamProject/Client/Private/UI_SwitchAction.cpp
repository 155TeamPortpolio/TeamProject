#include "pch.h"
#include "UI_SwitchAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "UI_SwitchAction.h"

const string CUI_SwitchAction::INSTANCENAMES[ENUM(Child::END)] = { "bg" };

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
	const string& filePath = pResourceMgr->Get_ResourcePath("hud_battle_specialAction.json");
	Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

	for (_int i = 0; i < ENUM(Child::END); ++i)
		m_handles[i] = Get_DescendantHandle(INSTANCENAMES[i]);

	return S_OK;
}

void CUI_SwitchAction::Update(_float dt)
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

void CUI_SwitchAction::UI_Active(void* pArg)
{
	Set_Enabled(true);
}

void CUI_SwitchAction::UI_DeActive(void* pArg)
{
	Set_Enabled(false);
}

void CUI_SwitchAction::Set_Enabled(_bool isEnabled)
{
	m_isEnabled = isEnabled;
}

void CUI_SwitchAction::Set_Ready()
{
	if (!m_isEnabled)
		return;
}

void CUI_SwitchAction::Use()
{
	if (!m_isEnabled || !m_isReady)
		return;
}

void CUI_SwitchAction::Set_Alive(Child child, _bool isAlive)
{
	ForChild(child, [isAlive](CUI_Object* ui) { ui->Set_Alive(isAlive); });
}

void CUI_SwitchAction::Set_Animation(Child child, _int iIndex)
{
	ForChild(child, [iIndex](CUI_Object* ui) { ui->Set_Animation(iIndex); });
}

void CUI_SwitchAction::Set_Color(Child child, _float4 vColor)
{
	ForChild(child, [vColor](CUI_Object* ui) { ui->Set_Color(vColor); });
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