#include "pch.h"
#include "BangBooDeliver.h"

#include "GameInstance.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"
#include "BangBooDeliverState_Idle.h"

CBangBooDeliver::CBangBooDeliver()
	:CServiceNpc()
{
}

CBangBooDeliver::CBangBooDeliver(const CBangBooDeliver& rhs)
	:CServiceNpc(rhs)
{
}

HRESULT CBangBooDeliver::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pResource = ResourceManager();
	pResource->Add_ResourcePath("CashierBangbooDeliver.model", "../Bin/Resources/Global/NPC/141BangBoo/Deliver/CashierBangbooDeliver.model");
	pResource->Add_ResourcePath("CashierBangbooDeliver.mat", "../Bin/Resources/Global/NPC/141BangBoo/Deliver/CashierBangbooDeliver.mat");
	pResource->Add_ResourcePath("NPC_Special_CashierBangboo_Deliver_Meta.json", "../Bin/Resources/Globall/NPC/141BangBoo/Deliver/NPC_Special_CashierBangboo_Deliver_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "CashierBangbooDeliver.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "CashierBangbooDeliver.mat");

	return S_OK;
}

HRESULT CBangBooDeliver::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CBangBooDeliver::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "CashierBangbooDeliver.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_Special_CashierBangboo_Deliver_Meta.json");
	pAnimator->Set_MotionBone(13); //Bip001

	m_strAnimName = "NPC_CashierBangbooDeliver_Ani_";
	m_strName = "Deliver";

	pAnimator->Set_Animation(Get_AnimName() + "Idle_Start")
		.Loop(false)
		.Apply();
}

void CBangBooDeliver::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CBangBooDeliver::Update(_float dt)
{
	__super::Update(dt);
	m_pStateMachine->Update(dt);
}

void CBangBooDeliver::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

HRESULT CBangBooDeliver::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CBangBooDeliver>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Idle");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CBangBooDeliver::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CBangBooDeliverState_Idle::Create());
	return S_OK;
}

CBangBooDeliver* CBangBooDeliver::Create()
{
	CBangBooDeliver* instance = new CBangBooDeliver();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CBangBooDeliver");
	}

	return instance;
}

CGameObject* CBangBooDeliver::Clone(INIT_DESC* pArg)
{
	CBangBooDeliver* instance = new CBangBooDeliver(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CBangBooDeliver");
	}
	return instance;
}

void CBangBooDeliver::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}
