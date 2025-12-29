#include "pch.h"
#include "Sacrifice.h"
#include "GameInstance.h"
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "CharacterController.h"

/* States */
#include "StateMachine.h"
#include "SacrificeState_Idle.h"
#include "SacrificeState_Walk.h"
#include "SacrificeState_Attack.h"
#include "SacrificeState_Born.h"
#include "SacrificeState_Hit.h"

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
	pResource->Add_ResourcePath("SacrificeBringer_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Body/Anim/SacrificeBringer_Meta.json");

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
	pAnimator->Link_MetaData(G_GlobalLevelKey, "SacrificeBringer_Meta.json");
	pAnimator->Set_ExtractBoneMovement(3, false, true, false); //Bip001

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	m_PartMeshIndices.resize(ENUM(PARTS::END));
	m_PartMeshIndices[ENUM(PARTS::ICE)] = 7;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_AXE)] = 11;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_SWORD)] = 12;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_ROAD)] = 13;
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
	if (m_RequestIdle)
	{
		m_pStateMachine->Change_State("Idle");
		m_RequestIdle = false;
	}
	if (m_RequestAttack)
	{
		m_pStateMachine->Change_State("Attack");
		m_RequestAttack = false;
	}
	if (m_RequestWalk)
	{
		m_pStateMachine->Change_State("Walk");
		m_RequestWalk = false;
	}

	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);
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

	return S_OK;
}

HRESULT CSacrifice::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CSacrifice>::CONDITION_ANIMATION_END);
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CSacrifice>::CONDITION_TIME_GREATER, "", 2.3f);
	//m_pStateMachine->Register_Transition("Attack", "Idle",
	//	CStateMachine<CSacrifice>::CONDITION_ANIMATION_END);

	return S_OK;
}
