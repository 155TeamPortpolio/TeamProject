#include "pch.h"
#include "SacrificeHand.h"
#include "GameInstance.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

/* State */
#include "StateMachine.h"
#include "SacrificeHandState_Attack.h"

CSacrificeHand::CSacrificeHand()
	:CEnemy()
{
}

CSacrificeHand::CSacrificeHand(const CSacrificeHand& rhg)
	:CEnemy(rhg)
{
}

HRESULT CSacrificeHand::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CAnimator3D>();

	auto pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResource->Add_ResourcePath("Monster_SacrificeBringerHand.model", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Hand/Monster_SacrificeBringerHand.model");
	pResource->Add_ResourcePath("Monster_SacrificeBringerHand.mat", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Hand/Monster_SacrificeBringerHand.mat");
	pResource->Add_ResourcePath("SacrificeBringerHand_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Hand/Anim/SacrificeBringerHand_Meta.json");

	return S_OK;
}

HRESULT CSacrificeHand::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Monster_SacrificeBringerHand.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Monster_SacrificeBringerHand.mat");

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Monster_SacrificeBringerHand.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "SacrificeBringerHand_Meta.json");
	//pAnimator->Set_MotionBone(43);
	//pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	SetActive(false);

	return S_OK;
}

void CSacrificeHand::Awake()
{
}

void CSacrificeHand::Priority_Update(_float dt)
{
}

void CSacrificeHand::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);

	m_pStateMachine->Update(dt);
}

void CSacrificeHand::Late_Update(_float dt)
{
}

CSacrificeHand* CSacrificeHand::Create()
{
	CSacrificeHand* instance = new CSacrificeHand();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CSacrificeHand");
	}

	return instance;
}

CGameObject* CSacrificeHand::Clone(INIT_DESC* pArg)
{
	CSacrificeHand* instance = new CSacrificeHand(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSacrificeHand");
	}

	return instance;
}

void CSacrificeHand::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}

void CSacrificeHand::SetActive(_bool isActive)
{
	auto pModel = Get_Component<CSkeletalModel>();
	_uint iMeshCount = pModel->Get_MeshCount();

	if (isActive)
	{
		for (_uint i = 0; i < iMeshCount; ++i)
			pModel->SetDrawable(i, true);
	}
	else
	{
		for (_uint i = 0; i < iMeshCount; ++i)
			pModel->SetDrawable(i, false);
	}
}

HRESULT CSacrificeHand::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CSacrificeHand>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Attack");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CSacrificeHand::Initialize_States()
{
	m_pStateMachine->Register_State("Attack", CSacrificeHandState_Attack::Create());

	return S_OK;
}

HRESULT CSacrificeHand::Initialize_Transitions()
{
	return S_OK;
}
