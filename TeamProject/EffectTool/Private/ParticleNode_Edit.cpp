#include "pch.h"
#include "ParticleNode_Edit.h"
#include "ParticleSystem.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

CParticleNode_Edit::CParticleNode_Edit()
	:CParticleNode()
{
}

CParticleNode_Edit::CParticleNode_Edit(const CParticleNode_Edit& rhs)
	:CParticleNode(rhs)
{
}

HRESULT CParticleNode_Edit::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CParticleNode_Edit::Initialize(INIT_DESC* pArg)
{
	CParticleSystem* pParticle = Get_Component<CParticleSystem>();
	pParticle->Link_Model(G_GlobalLevelKey, "Engine_Default_InstancePoint");

	return S_OK;
}

void CParticleNode_Edit::Awake()
{
}

void CParticleNode_Edit::Priority_Update(_float dt)
{
}

void CParticleNode_Edit::Update(_float dt)
{
	__super::Update(dt);
}

void CParticleNode_Edit::Late_Update(_float dt)
{
}

CParticleNode_Edit* CParticleNode_Edit::Create()
{
	CParticleNode_Edit* instance = new CParticleNode_Edit();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Clone Failed : CParticleNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CParticleNode_Edit::Clone(INIT_DESC* pArg)
{
	CParticleNode_Edit* instance = new CParticleNode_Edit(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CParticleNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}

void CParticleNode_Edit::Free()
{
	__super::Free();
}
