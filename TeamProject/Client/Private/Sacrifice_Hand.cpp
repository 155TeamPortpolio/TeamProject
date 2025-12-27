#include "pch.h"
#include "Sacrifice_Hand.h"
#include "GameInstance.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

/* State */
#include "StateMachine.h"

CSacrifice_Hand::CSacrifice_Hand()
{
}

CSacrifice_Hand::CSacrifice_Hand(const CSacrifice_Hand& rhg)
{
}

HRESULT CSacrifice_Hand::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CAnimator3D>();

	auto pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	//pResource->Add_ResourcePath("Monster")

	return S_OK;
}

HRESULT CSacrifice_Hand::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CSacrifice_Hand::Awake()
{
}

void CSacrifice_Hand::Priority_Update(_float dt)
{
}

void CSacrifice_Hand::Update(_float dt)
{
}

void CSacrifice_Hand::Late_Update(_float dt)
{
}

CSacrifice_Hand* CSacrifice_Hand::Create()
{
	CSacrifice_Hand* instance = new CSacrifice_Hand();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CSacrifice_Hand");
	}

	return instance;
}

CGameObject* CSacrifice_Hand::Clone(INIT_DESC* pArg)
{
	CSacrifice_Hand* instance = new CSacrifice_Hand(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSacrifice_Hand");
	}

	return instance;
}

void CSacrifice_Hand::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}

HRESULT CSacrifice_Hand::Initialize_StateMachine()
{
	return S_OK;
}

HRESULT CSacrifice_Hand::Initialize_States()
{
	return S_OK;
}

HRESULT CSacrifice_Hand::Initialize_Transitions()
{
	return S_OK;
}
