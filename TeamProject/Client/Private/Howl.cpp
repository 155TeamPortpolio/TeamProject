#include "pch.h"
#include "Howl.h"

#include "GameInstance.h"
#include "FieldSystem.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"
#include "HowlState_Idle.h"
#include "HowlState_Sleep.h"
#include "HowlState_Wake.h"


CHowl::CHowl()
    :CServiceNpc()
{
}

CHowl::CHowl(const CHowl& rhs)
    :CServiceNpc(rhs)
{
}

void CHowl::Execute()
{
	UI_DIALOGUE_REQUEST_DESC desc;
	desc.strDialogueID = m_DiagloueData.StartDialogueID;
	desc.iSequenceID = m_iNextSequceID;
	EventSystem()->Broadcast<UI_DIALOGUE_REQUEST_DESC>({ desc });
}

HRESULT CHowl::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pResource = ResourceManager();
	pResource->Add_ResourcePath("NPC_Woof.model", "../Bin/Resources/MainCity/NPC/Howl/NPC_Woof.model");
	pResource->Add_ResourcePath("NPC_Woof.mat", "../Bin/Resources/MainCity/NPC/Howl/NPC_Woof.mat");
	pResource->Add_ResourcePath("NPC_Special_Woof_Meta.json", "../Bin/Resources/MainCity/NPC/Howl/NPC_Special_Woof_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "NPC_Woof.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "NPC_Woof.mat");

	return S_OK;
}

HRESULT CHowl::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CHowl::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "NPC_Woof.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_Special_Woof_Meta.json");

	m_strAnimName = "NPC_Special_Woof_Ani_";
	m_strName = L"아우";

	pAnimator->Set_Animation(Get_AnimName() + "Idle01")
		.Loop(true)
		.Apply();

	__super::Awake();

	Add_InteractZone(Get_Position(), _float3(0.f, 0.f ,1.2f));
	//CFieldSystem::GetInstance()->Set_DayPahse(DayPhase::LateNight);
}

void CHowl::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CHowl::Update(_float dt)
{
	__super::Update(dt);
	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CHowl::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

HRESULT CHowl::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CHowl>::Create();
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

HRESULT CHowl::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CHowlState_Idle::Create());
	m_pStateMachine->Register_State("Sleep", CHowlState_Sleep::Create());
	m_pStateMachine->Register_State("Wake", CHowlState_Wake::Create());

	return S_OK;
}

HRESULT CHowl::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Sleep", "Wake",
		CStateMachine<CHowl>::CONDITION_TRIGGER, "ToWake");

	m_pStateMachine->Register_Transition("Wake", "Idle",
		CStateMachine<CHowl>::CONDITION_TRIGGER, "ToIdle");

	m_pStateMachine->Register_Transition("Idle", "Sleep",
		CStateMachine<CHowl>::CONDITION_TRIGGER, "ToSleep");

	m_pStateMachine->Register_Transition("Sleep", "Idle",
		CStateMachine<CHowl>::CONDITION_TRIGGER, "ToIdle");

	return S_OK;
}

void CHowl::Update_States(_float dt)
{
	if (m_pStateMachine->Get_CurrentStateName() != "Sleep" && m_pStateMachine->Get_CurrentStateName() != "Wake")
	{
		if (CFieldSystem::GetInstance()->Get_DayPhase() == DayPhase::LateNight)
			m_pStateMachine->Set_Trigger("ToSleep");
	}
	else
	{
		if (CFieldSystem::GetInstance()->Get_DayPhase() != DayPhase::LateNight)
			m_pStateMachine->Set_Trigger("ToIdle");
	}
}

CHowl* CHowl::Create()
{
	CHowl* instance = new CHowl();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CHowl");
	}

	return instance;
}

CGameObject* CHowl::Clone(INIT_DESC* pArg)
{
	CHowl* instance = new CHowl(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CHowl");
	}
	return instance;
}

void CHowl::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}
