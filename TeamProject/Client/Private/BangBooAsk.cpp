#include "pch.h"
#include "BangBooAsk.h"

#include "GameInstance.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"
#include "BangBooAskState_Idle.h"

CBangBooAsk::CBangBooAsk()
	:CServiceNpc()
{
}

CBangBooAsk::CBangBooAsk(const CBangBooAsk& rhs)
	:CServiceNpc(rhs)
{
}

HRESULT CBangBooAsk::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pResource = ResourceManager();
	pResource->Add_ResourcePath("CashierBangbooAsk.model", "../Bin/Resources/Global/NPC/141BangBoo/Ask/CashierBangbooAsk.model");
	pResource->Add_ResourcePath("CashierBangbooAsk.mat", "../Bin/Resources/Global/NPC/141BangBoo/Ask/CashierBangbooAsk.mat");
	pResource->Add_ResourcePath("NPC_CashierBangbooAsk_Meta.json", "../Bin/Resources/Global/NPC/141BangBoo/Ask/NPC_CashierBangbooAsk_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "CashierBangbooAsk.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "CashierBangbooAsk.mat");

	return S_OK;
}

HRESULT CBangBooAsk::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CBangBooAsk::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "CashierBangbooAsk.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_CashierBangbooAsk_Meta.json");
	pAnimator->Set_MotionBone(13); //Bip001

	m_strAnimName = "NPC_CashierBangbooAsk_Ani_";
	m_strName = L"문의";

	pAnimator->Set_Animation(Get_AnimName() + "Idle_Start")
		.Loop(false)
		.Apply();
}

void CBangBooAsk::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CBangBooAsk::Update(_float dt)
{
	__super::Update(dt);
	m_pStateMachine->Update(dt);
}

void CBangBooAsk::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

HRESULT CBangBooAsk::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CBangBooAsk>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Idle");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CBangBooAsk::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CBangBooAskState_Idle::Create());
	return S_OK;
}

CBangBooAsk* CBangBooAsk::Create()
{
	CBangBooAsk* instance = new CBangBooAsk();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CBangBooAsk");
	}

	return instance;
}

CGameObject* CBangBooAsk::Clone(INIT_DESC* pArg)
{
	CBangBooAsk* instance = new CBangBooAsk(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CBangBooAsk");
	}
	return instance;
}

void CBangBooAsk::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}
