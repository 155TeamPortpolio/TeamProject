#include "pch.h"
#include "BangBooPay.h"

#include "GameInstance.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"
#include "BangBooPayState_Idle.h"

CBangBooPay::CBangBooPay()
    :CServiceNpc()
{
}

CBangBooPay::CBangBooPay(const CBangBooPay& rhs)
    :CServiceNpc(rhs)
{
}

HRESULT CBangBooPay::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pResource = ResourceManager();
	pResource->Add_ResourcePath("NPC_CashierBangbooPay_Modelout.model", "../Bin/Resources/Model/skeletal/NPC/141BangBoo/Pay/NPC_CashierBangbooPay_Modelout.model");
	pResource->Add_ResourcePath("NPC_CashierBangbooPay_Modelout.mat", "../Bin/Resources/Model/skeletal/NPC/141BangBoo/Pay/NPC_CashierBangbooPay_Modelout.mat");
	pResource->Add_ResourcePath("NPC_CashierBangbooPay_Idle_Start_Meta.json", "../Bin/Resources/Model/skeletal/NPC/141BangBoo/Pay/NPC_CashierBangbooPay_Idle_Start_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "NPC_CashierBangbooPay_Modelout.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "NPC_CashierBangbooPay_Modelout.mat");

	return S_OK;
}

HRESULT CBangBooPay::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CBangBooPay::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "NPC_CashierBangbooPay_Modelout.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_CashierBangbooPay_Idle_Start_Meta.json");
	pAnimator->Set_MotionBone(13); //Bip001

	m_strAnimName = "NPC_CashierBangbooPay_";
	m_strName = "Ask";

	pAnimator->Set_Animation(Get_AnimName() + "Idle_Start")
		.Loop(false)
		.Apply();
}

void CBangBooPay::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CBangBooPay::Update(_float dt)
{
	__super::Update(dt);
	m_pStateMachine->Update(dt);
}

void CBangBooPay::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

HRESULT CBangBooPay::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CBangBooPay>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Idle");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CBangBooPay::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CBangBooPayState_Idle::Create());
    return S_OK;
}

CBangBooPay* CBangBooPay::Create()
{
	CBangBooPay* instance = new CBangBooPay();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CBangBooPay");
	}

	return instance;
}

CGameObject* CBangBooPay::Clone(INIT_DESC* pArg)
{
	CBangBooPay* instance = new CBangBooPay(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CBangBooPay");
	}
	return instance;
}

void CBangBooPay::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}
