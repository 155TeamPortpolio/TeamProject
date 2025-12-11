#include "Engine_Defines.h"
#include "EffectContainer.h"

CEffectContainer::CEffectContainer()
	:CGameObject()
{
}

CEffectContainer::CEffectContainer(const CEffectContainer& rhs)
	:CGameObject(rhs)
{
}

HRESULT CEffectContainer::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CEffectContainer::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CEffectContainer::Awake()
{
}

void CEffectContainer::Priority_Update(_float dt)
{
}

void CEffectContainer::Update(_float dt)
{
}

void CEffectContainer::Late_Update(_float dt)
{
}

CEffectContainer* CEffectContainer::Create()
{
	CEffectContainer* instance = new CEffectContainer();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CEffectContainer");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CEffectContainer::Clone(INIT_DESC* pArg)
{
	CEffectContainer* instance = new CEffectContainer(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CEffectContainer");
		Safe_Release(instance);
	}

	return instance;
}

void CEffectContainer::Free()
{
	__super::Free();
}
