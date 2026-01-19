#include "pch.h"
#include "Belle.h"

#include "GameInstance.h"
#include "Material.h"
#include "Animator3D.h"
#include "StateMachine.h"
#include "CharacterController.h"
#include "SkeletalModel.h"

#include "BelleState_Idle.h"
#include "BelleState_Move.h"

CBelle::CBelle()
{
}

CBelle::CBelle(const CBelle& rhs)
	:CFieldCharacter(rhs)
{
}

HRESULT CBelle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Belle.model",
		"../Bin/Resources/Model/skeletal/FieldCharacter/Belle/Belle.model");
	pRcsMgr->Add_ResourcePath("Belle.mat",
		"../Bin/Resources/Model/skeletal/FieldCharacter/Belle/Belle.mat");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size02_Belle_Suibianguan_Meta.json",
		"../Bin/Resources/Model/skeletal/FieldCharacter/Belle/Avatar_Female_Size02_Belle_Suibianguan_Meta.json");

	Get_Component<CModel>()->Link_Model(G_GlobalLevelKey, "Belle.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Belle.mat");
	return S_OK;
}

HRESULT CBelle::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CBelle::Awake()
{
	__super::Awake();

	m_pAnimator->LinkAnimate_Model("Test_Level", "Belle.model");
	m_pAnimator->Link_MetaData("Test_Level", "Avatar_Female_Size02_Belle_Suibianguan_Meta.json");
	m_strAnimName = "Avatar_Female_Size02_Belle_";
	m_strName = "Belle";

	m_pAnimator->Set_Animation(Get_AnimName() + "Ani_MainCity_Idle")
		.Loop(true)
		.Apply();
	m_pCCT->Set_GravityEnabled(true);

	//Get_Component<CModel>()->ShadowCast(false);
}

void CBelle::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CBelle::Update(_float dt)
{
	Update_States();
	m_pStateMachine->Update(dt);
	__super::Update(dt);
}

void CBelle::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CBelle::Render_GUI()
{
	__super::Render_GUI();
	if (m_pStateMachine)
	{
		ImGui::Separator();
		ImGui::Text("StateMachine: %s", m_pStateMachine->Get_CurrentStateName().c_str());

		if (ImGui::Button("Open StateMachine"))
			m_pStateMachine->Set_ShowWindow(true);

		m_pStateMachine->Render_GUI();
	}
}

HRESULT CBelle::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CBelle>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Idle");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CBelle::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CBelleState_Idle::Create());
	m_pStateMachine->Register_State("Move", CBelleState_Move::Create());
	return S_OK;
}

HRESULT CBelle::Initialize_Transitions()
{
	// Idle -> Move
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CBelle>::CONDITION_BOOL_TRUE, "IsMove");

	// Move -> Idle
	m_pStateMachine->Register_Transition("Move", "Idle",
		CStateMachine<CBelle>::CONDITION_TRIGGER, "ToIdle");
	return S_OK;
}

void CBelle::Update_States()
{
	m_pStateMachine->Set_Bool("IsMove", Is_Move_Buffer());

	Process_EndState(m_pStateMachine->Get_CurrentStateName());
}

void CBelle::Process_EndState(const string& strCurrentState)
{

}

CBelle* CBelle::Create()
{
	CBelle* Instance = new CBelle();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CBelle::Clone(INIT_DESC* pArg)
{
	CBelle* Instance = new CBelle(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CBelle::Free()
{
	Safe_Release(m_pStateMachine);
	__super::Free();
}
