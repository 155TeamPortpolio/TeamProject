#include "pch.h"
#include "SpriteNode_Edit.h"

CSpriteNode_Edit::CSpriteNode_Edit()
	:CSpriteNode()
{
}

CSpriteNode_Edit::CSpriteNode_Edit(const CSpriteNode_Edit& rhs)
	:CSpriteNode(rhs)
{
}

HRESULT CSpriteNode_Edit::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CSpriteNode_Edit::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CSpriteNode_Edit::Awake()
{
}

void CSpriteNode_Edit::Priority_Update(_float dt)
{
}

void CSpriteNode_Edit::Update(_float dt)
{
	__super::Update(dt);
}

void CSpriteNode_Edit::Late_Update(_float dt)
{
}

void CSpriteNode_Edit::Render_GUI()
{
}

CSpriteNode_Edit* CSpriteNode_Edit::Create()
{
	CSpriteNode_Edit* instance = new CSpriteNode_Edit();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Clone Failed : CSpriteNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CSpriteNode_Edit::Clone(INIT_DESC* pArg)
{
	CSpriteNode_Edit* instance = new CSpriteNode_Edit(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CSpriteNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}

void CSpriteNode_Edit::Free()
{
	__super::Free();
}
