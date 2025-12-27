#include "pch.h"
#include "Sacrifice.h"
#include "GameInstance.h"
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"

/* States */
#include "StateMachine.h"
#include "SacrificeState_Idle.h"

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

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CSacrifice::Post_EngineUpdate(_float dt)
{
	__super::Post_EngineUpdate(dt);
}

void CSacrifice::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Monster_SacrificeBringer.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "SacrificeBringer_Meta.json");
}

void CSacrifice::Priority_Update(_float dt)
{

}

void CSacrifice::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CSacrifice::Late_Update(_float dt)
{

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

HRESULT CSacrifice::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CSacrifice>::Create();
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

HRESULT CSacrifice::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CSacrificeState_Idle::Create());

	return S_OK;
}

HRESULT CSacrifice::Initialize_Transitions()
{
	return S_OK;
}
