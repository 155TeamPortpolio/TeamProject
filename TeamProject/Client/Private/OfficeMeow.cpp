#include "pch.h"
#include "OfficeMeow.h"

#include "GameInstance.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"
#include "OfficeMeowState_Idle.h"
#include "OfficeMeowState_Think.h"
#include "OfficeMeowState_Welcome.h"

COfficeMeow::COfficeMeow()
	:CServiceNpc()
{
}

COfficeMeow::COfficeMeow(const COfficeMeow& rhs)
	:CServiceNpc(rhs)
{
}

HRESULT COfficeMeow::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pResource = ResourceManager();
	pResource->Add_ResourcePath("NPC_OfficerMeowji.model", "../Bin/Resources/Model/skeletal/NPC/OfficeMeowji/NPC_OfficerMeowji.model");
	pResource->Add_ResourcePath("NPC_OfficerMeowji.mat", "../Bin/Resources/Model/skeletal/NPC/OfficeMeowji/NPC_OfficerMeowji.mat");
	pResource->Add_ResourcePath("NPC_Male_OfficerMeowji_Meta.json", "../Bin/Resources/Model/skeletal/NPC/OfficeMeowji/NPC_Male_OfficerMeowji_Meta.json");
	
	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "NPC_OfficerMeowji.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "NPC_OfficerMeowji.mat");

	return S_OK;
}

HRESULT COfficeMeow::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void COfficeMeow::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "NPC_OfficerMeowji.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_Male_OfficerMeowji_Meta.json");
	pAnimator->Set_MotionBone(13); //Bip001

	m_strAnimName = "NPC_Male_OfficerMeowji_Ani_MainCity_Stand_";
	m_strName = "OfficerMeow";

	pAnimator->Set_Animation(Get_AnimName() + "Idle01")
		.Loop(true)
		.Apply();
}

void COfficeMeow::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void COfficeMeow::Update(_float dt)
{
	__super::Update(dt);
	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void COfficeMeow::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

HRESULT COfficeMeow::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<COfficeMeow>::Create();
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

HRESULT COfficeMeow::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", COfficeMeowState_Idle::Create());
	m_pStateMachine->Register_State("Think", COfficeMeowState_Think::Create());
	m_pStateMachine->Register_State("Welcome", COfficeMeowState_Welcome::Create());

	return S_OK;
}

HRESULT COfficeMeow::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Idle", "Think",
		CStateMachine<COfficeMeow>::CONDITION_TRIGGER, "ToThink");

	m_pStateMachine->Register_Transition("Think", "Idle",
		CStateMachine<COfficeMeow>::CONDITION_TRIGGER, "ToIdle");

	m_pStateMachine->Register_AnyStateTransition("Welcome",
		CStateMachine<COfficeMeow>::CONDITION_TRIGGER, "Welcome");

	m_pStateMachine->Register_Transition("Welcome", "Idle",
		CStateMachine<COfficeMeow>::CONDITION_TRIGGER, "ToIdle");

	return S_OK;
}

void COfficeMeow::Update_States(_float dt)
{
	m_fWelcomeCurrentCoolDown += dt;
	if (m_fWelcomeCurrentCoolDown > 15.f)
	{
		m_pStateMachine->Set_Trigger("Welcome");
		m_fWelcomeCurrentCoolDown = 0.f;
	}
}

COfficeMeow* COfficeMeow::Create()
{
	COfficeMeow* instance = new COfficeMeow();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : COfficeMeow");
	}

	return instance;
}

CGameObject* COfficeMeow::Clone(INIT_DESC* pArg)
{
	COfficeMeow* instance = new COfficeMeow(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : COfficeMeow");
	}
	return instance;
}

void COfficeMeow::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}
