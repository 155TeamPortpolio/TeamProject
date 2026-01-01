#include "pch.h"
#include "Sacrifice.h"
#include "GameInstance.h"
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "CharacterController.h"
#include "Helper_Func.h"

/* States */
#include "StateMachine.h"
#include "SacrificeState_Idle.h"
#include "SacrificeState_Walk.h"
#include "SacrificeState_Attack.h"
#include "SacrificeState_Born.h"
#include "SacrificeState_Hit.h"
#include "SacrificeState_Evade.h"
#include "SacrificeState_Death.h"
#include "SacrificeState_ChangePhase.h"
#include "SacrificeState_Parry.h"

CSacrifice::CSacrifice()
	:CEnemy()
{
}

CSacrifice::CSacrifice(const CSacrifice& rhg)
	:CEnemy(rhg)
{
}

HRESULT CSacrifice::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResource->Add_ResourcePath("Monster_SacrificeBringer.model", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Body/Monster_SacrificeBringer.model");
	pResource->Add_ResourcePath("Monster_SacrificeBringer.mat", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Body/Monster_SacrificeBringer.mat");
	pResource->Add_ResourcePath("Monster_SacrificeBringer_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Body/Monster_SacrificeBringer_Meta.json");

	return S_OK;
}

HRESULT CSacrifice::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Monster_SacrificeBringer.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Monster_SacrificeBringer.mat");

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Monster_SacrificeBringer.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Monster_SacrificeBringer_Meta.json");
	pAnimator->Set_MotionBone(3); //Bip001
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);

	auto pCCT = Get_Component<CCharacterController>();
	pCCT->Set_GravityEnabled(false);

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	m_PartMeshIndices.resize(ENUM(PARTS::END));
	m_PartMeshIndices[ENUM(PARTS::ICE)] = 7;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_AXE)] = 11;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_SWORD)] = 12;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_WHIP)] = 13;
	for (_uint i = 0; i < m_PartMeshIndices.size(); ++i)
		pModel->SetDrawable(m_PartMeshIndices[i], false);

	return S_OK;
}

void CSacrifice::Awake()
{
}

void CSacrifice::Priority_Update(_float dt)
{

}

void CSacrifice::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);
	
	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CSacrifice::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

CSacrifice* CSacrifice::Create()
{
	CSacrifice* instance = new CSacrifice();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CSacrifice");
	}

	return instance;
}

CGameObject* CSacrifice::Clone(INIT_DESC* pArg)
{
	CSacrifice* instance = new CSacrifice(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSacrifice");
	}

	return instance;
}

void CSacrifice::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}

void CSacrifice::ActiveSword()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_SWORD)], true);
}

void CSacrifice::DeactiveSword()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_SWORD)], false);
}

void CSacrifice::ActiveAxe()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_AXE)], true);
}

void CSacrifice::DeactiveAxe()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_AXE)], false);
}

void CSacrifice::ActiveWhip()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_WHIP)], true);
}

void CSacrifice::DeactiveWhip()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_WHIP)], false);
}

void CSacrifice::ChangePhase()
{
	if (PHASE::PHASE1 == m_eCurrPhase)
		m_pStateMachine->Change_State("ChangePhase");
}

HRESULT CSacrifice::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CSacrifice>::Create();
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

HRESULT CSacrifice::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CSacrificeState_Idle::Create());
	m_pStateMachine->Register_State("Walk", CSacrificeState_Walk::Create());
	m_pStateMachine->Register_State("Attack",CSacrificeState_Attack::Create());
	m_pStateMachine->Register_State("Born",CSacrificeState_Born::Create());
	m_pStateMachine->Register_State("Hit",CSacrificeState_Hit::Create());
	m_pStateMachine->Register_State("Evade", CSacrificeState_Evade::Create());
	m_pStateMachine->Register_State("Death", CSacrificeState_Death::Create());
	m_pStateMachine->Register_State("ChangePhase", CSacrificeState_ChangePhase::Create());
	m_pStateMachine->Register_State("Parry", CSacrificeState_Parry::Create());

	return S_OK;
}

HRESULT CSacrifice::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CSacrifice>::CONDITION_ANIMATION_END);

	/* From Idle */
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CSacrifice>::CONDITION_TRIGGER, "Idle_To_Attack");
	m_pStateMachine->Register_Transition("Idle", "Evade",
		CStateMachine<CSacrifice>::CONDITION_TRIGGER, "Idle_To_Evade");
	m_pStateMachine->Register_Transition("Idle", "Walk",
		CStateMachine<CSacrifice>::CONDITION_TRIGGER, "Idle_To_Walk");

	/* From Death */
	m_pStateMachine->Register_Transition("Death", "ChangePhase",
		CStateMachine<CSacrifice>::CONDITION_ANIMATION_END);

	/* From Change Phase */
	m_pStateMachine->Register_Transition("ChangePhase", "Idle",
		CStateMachine<CSacrifice>::CONDITION_ANIMATION_END);

	/* From Parry */
	m_pStateMachine->Register_Transition("Parry", "Idle",
		CStateMachine<CSacrifice>::CONDITION_ANIMATION_END);

	return S_OK;
}

void CSacrifice::Update_States(_float dt)
{
	if (m_RequestIdle)
	{
		m_pStateMachine->Change_State("Idle");
		m_pStateMachine->Reset_Trigger("Idle_To_Attack");
		m_pStateMachine->Reset_Trigger("Idle_To_Evade");
		m_pStateMachine->Reset_Trigger("Idle_To_Walk");
		m_RequestIdle = false;
	}

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Tap(VK_SPACE))
		m_pStateMachine->Change_State("Parry");

	/* Idle */
	if ("Idle" == m_pStateMachine->Get_CurrentStateName())
	{
		m_fIdleElasedTime += dt;
		if (m_fIdleElasedTime >= m_fIdleDuration)
		{
			_uint iRandIndex = Helper::Get_Random_Int(0, 3);
			iRandIndex = 0;
			if (0 == iRandIndex)
			{
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			}
			else if (1 == iRandIndex)
			{
				m_pStateMachine->Set_Trigger("Idle_To_Walk");
			}
			else
			{
				m_pStateMachine->Set_Trigger("Idle_To_Evade");
			}
	
			m_fIdleElasedTime = 0.f;
		}
	}
	
}
