#include "pch.h"
#include "BangBoo.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

CBangBoo::CBangBoo()
    :CNpc()
{
}

CBangBoo::CBangBoo(const CBangBoo& rhs)
    :CNpc(rhs)
{
}

HRESULT CBangBoo::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBangBoo::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CBangBoo::Awake()
{
}

void CBangBoo::Priority_Update(_float dt)
{
}

void CBangBoo::Update(_float dt)
{
}

void CBangBoo::Late_Update(_float dt)
{
}

CBangBoo* CBangBoo::Create()
{
	CBangBoo* instance = new CBangBoo();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CBangBoo");
	}

	return instance;
}

CGameObject* CBangBoo::Clone(INIT_DESC* pArg)
{
	CBangBoo* instance = new CBangBoo(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CBangBoo");
	}
	return instance;
}

void CBangBoo::Free()
{
	__super::Free();
}