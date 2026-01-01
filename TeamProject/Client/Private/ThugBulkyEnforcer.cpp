#include "pch.h"
#include "ThugBulkyEnforcer.h"

#include "GameInstance.h"
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "CharacterController.h"
#include "Helper_Func.h"

/* States */
#include "StateMachine.h"
#include "ThugBulkyEnforcer_Idle.h"
#include "ThugBulkyEnforcer_Born.h"
#include "ThugBulkyEnforcer_Attack.h"



CThugBulkyEnforcer::CThugBulkyEnforcer()
	: CEnemy()
{
}

CThugBulkyEnforcer::CThugBulkyEnforcer(const CThugBulkyEnforcer& rhg)
	: CEnemy(rhg)
{
}

HRESULT CThugBulkyEnforcer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("Monster_ThugBulkyEnforcer.mat", "../Bin/Resources/Model/skeletal/Enemy/ThugBulkyEnforcer/Monster_ThugBulkyEnforcer.mat");
	pResourceMgr->Add_ResourcePath("Monster_ThugBulkyEnforcer.model", "../Bin/Resources/Model/skeletal/Enemy/ThugBulkyEnforcer/Monster_ThugBulkyEnforcer.model");
	pResourceMgr->Add_ResourcePath("ThugBulkyEnforcer_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/ThugBulkyEnforcer/ThugBulkyEnforcer_Meta.json");

	return S_OK;
}

HRESULT CThugBulkyEnforcer::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Monster_ThugBulkyEnforcer.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Monster_ThugBulkyEnforcer.mat");

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Monster_ThugBulkyEnforcer.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "ThugBulkyEnforcer_Meta.json");
	pAnimator->Set_MotionBone(3);	//Bip001
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	

	return S_OK;
}

void CThugBulkyEnforcer::Awake()
{
}

void CThugBulkyEnforcer::Priority_Update(_float dt)
{
}

void CThugBulkyEnforcer::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);

	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CThugBulkyEnforcer::Late_Update(_float dt)
{
}

CThugBulkyEnforcer* CThugBulkyEnforcer::Create()
{
	CThugBulkyEnforcer* instance = new CThugBulkyEnforcer();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CThugBulkyEnforcer");
	}

	return instance;
}

CGameObject* CThugBulkyEnforcer::Clone(INIT_DESC* pArg)
{
	CThugBulkyEnforcer* instance = new CThugBulkyEnforcer(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSacrificeHand");
	}

	return instance;
}

void CThugBulkyEnforcer::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}

/* For.State Machine */
HRESULT CThugBulkyEnforcer::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();
	if (nullptr == m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Born");
	m_pStateMachine->Initialize(this);

	Get_Component<CAnimator3D>()->Set_Animation("ThugBulkyEnforcer_Ani_Born")
		.Apply();

	return S_OK;
}

HRESULT CThugBulkyEnforcer::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CThugBulkyEnforcer_Born::Create());
	m_pStateMachine->Register_State("Idle", CThugBulkyEnforcer_Idle::Create());
	m_pStateMachine->Register_State("Attack", CThugBulkyEnforcer_Attack::Create());


	return S_OK;
}

HRESULT CThugBulkyEnforcer::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_ANIMATION_END);

	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_ANIMATION_END);

	return S_OK;
}

void CThugBulkyEnforcer::Update_States(_float dt)
{

}
