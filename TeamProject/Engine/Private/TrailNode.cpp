#include "Engine_Defines.h"
#include "TrailNode.h"

CTrailNode::CTrailNode()
	:CEffectNode()
{
}

CTrailNode::CTrailNode(const CTrailNode& rhs)
	:CEffectNode(rhs)
{
}

HRESULT CTrailNode::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrailNode::Initialize(INIT_DESC* pArg)
{
	return S_OK;
}

void CTrailNode::Awake()
{
}

void CTrailNode::Priority_Update(_float dt)
{
}

void CTrailNode::Update(_float dt)
{
}

void CTrailNode::Late_Update(_float dt)
{
}

CTrailNode* CTrailNode::Create()
{
	CTrailNode* instance = new CTrailNode();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CTrailNode");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CTrailNode::Clone(INIT_DESC* pArg)
{
	CTrailNode* instance = new CTrailNode(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Create Failed : CTrailNode");
		Safe_Release(instance);
	}

	return instance;
}

void CTrailNode::Free()
{
	__super::Free();
}
