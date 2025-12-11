#include "Engine_Defines.h"
#include "SpriteNode.h"

CSpriteNode::CSpriteNode()
	:CEffectNode()
{
}

CSpriteNode::CSpriteNode(const CSpriteNode& rhs)
	:CEffectNode(rhs)
{
}

HRESULT CSpriteNode::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CSpriteNode::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CSpriteNode::Awake()
{
}

void CSpriteNode::Priority_Update(_float dt)
{
}

void CSpriteNode::Update(_float dt)
{
}

void CSpriteNode::Late_Update(_float dt)
{
}

CSpriteNode* CSpriteNode::Create()
{
	CSpriteNode* instance = new CSpriteNode();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CSpriteNode");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CSpriteNode::Clone(INIT_DESC* pArg)
{
	CSpriteNode* instance = new CSpriteNode(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Create Failed : CSpriteNode");
		Safe_Release(instance);
	}

	return instance;
}

void CSpriteNode::Free()
{
	__super::Free();
}
