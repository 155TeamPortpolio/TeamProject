#include "pch.h"
#include "SacrificeHand.h"
#include "GameInstance.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

/* State */
#include "StateMachine.h"

CSacrificeHand::CSacrificeHand()
{
}

CSacrificeHand::CSacrificeHand(const CSacrificeHand& rhg)
{
}

HRESULT CSacrificeHand::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CAnimator3D>();

	auto pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	//pResource->Add_ResourcePath("Monster")

	return S_OK;
}

HRESULT CSacrificeHand::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

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

HRESULT CSacrificeHand::Initialize_StateMachine()
{
	return S_OK;
}

HRESULT CSacrificeHand::Initialize_States()
{
	return S_OK;
}

HRESULT CSacrificeHand::Initialize_Transitions()
{
	return S_OK;
}
