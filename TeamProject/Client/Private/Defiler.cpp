#include "pch.h"
#include "Defiler.h"
#include "GameInstance.h"

#include "SkeletalModel.h"
#include "Animator3D.h"
#include "Material.h"
#include "CharacterController.h"

#include "StateMachine.h"
#include "DefilerState.h"

CDefiler::CDefiler()
	:CEnemy()
{
}

CDefiler::CDefiler(const CDefiler& rhg)
	:CEnemy(rhg)
{
}

HRESULT CDefiler::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	Get_Component<CSkeletalModel>()->Link_Model(G_GlobalLevelKey, "Defiler_Isolde.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Defiler_Isolde.mat");

	return S_OK;
}

HRESULT CDefiler::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	m_eEnemyClass = ENEMY_CLASS::BOSS;
	vector<_uint> ProMeshes = Get_Component<CSkeletalModel>()->Hide_MehsByName("Pro");

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Defiler_Isolde.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Monster_IsoldetheDefiler_Meta.json");
	pAnimator->Resize_Layer(3);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 2);

	auto pCCT = Get_Component<CCharacterController>();

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;
	
	//if (FAILED(Create_Colliders()))
	//	return E_FAIL;


	Create_UIEnemyStatus("Bip001_Spine2");
	Create_UIBossHUD();

	return S_OK;
}

void CDefiler::Awake()
{
}

void CDefiler::Priority_Update(_float dt)
{
}

void CDefiler::Update(_float dt)
{
	__super::Update(dt);

	Update_States(dt);
	m_pStateMachine->Update(dt);

	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);
}

void CDefiler::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CDefiler::Render_GUI()
{
	__super::Render_GUI();

	Render_GUI_ForTargetInfo();
	m_pStateMachine->Render_GUI();
	ImGui::Text("Current State : %s", m_pStateMachine->Get_CurrentStateName().c_str());
}

CDefiler* CDefiler::Create()
{
	CDefiler* instance = new CDefiler();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CDefiler");
	}

	return instance;
}

void CDefiler::Update_States(_float dt)
{
	
}

CGameObject* CDefiler::Clone(INIT_DESC* pArg)
{
	CDefiler* instance = new CDefiler(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CDefiler");
	}

	return instance;
}

void CDefiler::Free()
{
	__super::Free();
	Safe_Release(m_pStateMachine);
}

HRESULT CDefiler::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CDefiler>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Born");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CDefiler::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CDefilerState_Born::Create());
	//m_pStateMachine->Register_State("Idle", CSacrificeState_Idle::Create());
	//m_pStateMachine->Register_State("Walk", CSacrificeState_Walk::Create());
	//m_pStateMachine->Register_State("Attack", CSacrificeState_Attack::Create());
	//m_pStateMachine->Register_State("Hit", CSacrificeState_Hit::Create());
	//m_pStateMachine->Register_State("Evade", CSacrificeState_Evade::Create());
	//m_pStateMachine->Register_State("Death", CSacrificeState_Death::Create());
	//m_pStateMachine->Register_State("ChangePhase", CSacrificeState_ChangePhase::Create());
	//m_pStateMachine->Register_State("Parry", CSacrificeState_Parry::Create());
	//m_pStateMachine->Register_State("Groggy", CSacrificeState_Groggy::Create());

	return S_OK;
}

HRESULT CDefiler::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CDefiler>::CONDITION_ANIMATION_END);

	/* From Idle */
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CDefiler>::CONDITION_TRIGGER, "Idle_To_Attack");
	m_pStateMachine->Register_Transition("Idle", "Walk",
		CStateMachine<CDefiler>::CONDITION_TRIGGER, "Idle_To_Walk");

	/* From Death */
	vector<CStateMachine<CDefiler>::CONDITION_INFO> conditions;
	conditions.push_back({ CStateMachine<CDefiler>::CONDITION_ANIMATION_END });
	conditions.push_back({ CStateMachine<CDefiler>::CONDITION_TRIGGER,"Change_Phase" });
	m_pStateMachine->Register_Transition("Death", "ChangePhase", conditions);

	/* From Change Phase */
	m_pStateMachine->Register_Transition("ChangePhase", "Idle",
		CStateMachine<CDefiler>::CONDITION_ANIMATION_END);

	/* From Parry */
	m_pStateMachine->Register_Transition("Parry", "Idle",
		CStateMachine<CDefiler>::CONDITION_ANIMATION_END);

	return S_OK;
}