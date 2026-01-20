#include "pch.h"
#include "Howl.h"

#include "GameInstance.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"
#include "HowlState_Idle.h"
#include "HowlState_Sleep.h"

CHowl::CHowl()
    :CServiceNpc()
{
}

CHowl::CHowl(const CHowl& rhs)
    :CServiceNpc(rhs)
{
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
	m_strName = "Howl";

	pAnimator->Set_Animation(Get_AnimName() + "Idle01")
		.Loop(true)
		.Apply();
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

	return S_OK;
}

HRESULT CHowl::Initialize_Transitions()
{

	return S_OK;
}

void CHowl::Update_States(_float dt)
{
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
