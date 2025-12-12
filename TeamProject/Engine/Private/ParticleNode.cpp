#include "Engine_Defines.h"
#include "ParticleNode.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

CParticleNode::CParticleNode()
	:CEffectNode()
{
}

CParticleNode::CParticleNode(const CParticleNode& rhs)
	:CEffectNode(rhs)
{
}

HRESULT CParticleNode::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CParticleNode::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CParticleNode::Awake()
{
}

void CParticleNode::Priority_Update(_float dt)
{
}

void CParticleNode::Update(_float dt)
{
}

void CParticleNode::Late_Update(_float dt)
{
}

CParticleNode* CParticleNode::Create()
{
	CParticleNode* instance = new CParticleNode();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CSpriteNode");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CParticleNode::Clone(INIT_DESC* pArg)
{
	CParticleNode* instance = new CParticleNode(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Create Failed : CSpriteNode");
		Safe_Release(instance);
	}

	return instance;
}

void CParticleNode::Free()
{
	__super::Free();
}
