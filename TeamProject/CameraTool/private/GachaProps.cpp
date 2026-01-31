#include "pch.h"
#include "GachaProps.h"
#include "GachaBack.h"
#include "GachaTV.h"
#include "GachaStage.h"
// Engine
#include "StaticModel.h"
#include "Material.h"
#include "ObjectContainer.h"

HRESULT CGachaProps::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CGachaProps::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Initialize_GlobalPrototype();

	Add_GachaProps();
	return S_OK;
}

void CGachaProps::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CGachaProps::Update(_float dt)
{
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CGachaProps::Late_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

HRESULT CGachaProps::Initialize_GlobalPrototype()
{
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_GachaBack",  CGachaBack::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_GachaTV",    CGachaTV::Create());
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_GachaStage", CGachaStage::Create());

	return S_OK;
}

void CGachaProps::Add_GachaProps()
{
	auto container  = Get_Component<CObjectContainer>();
	auto gachaBack  = Builder::Create_Object({G_GlobalLevelKey, "Proto_GameObject_GachaBack" }).Build("GachaLayer");
	auto gachaTV    = Builder::Create_Object({G_GlobalLevelKey, "Proto_GameObject_GachaTV"   }).Build("GachaLayer");
	auto gachaStage = Builder::Create_Object({G_GlobalLevelKey, "Proto_GameObject_GachaStage"}).Build("GachaLayer");
	
	container->Add_Child(gachaBack,  true);
	container->Add_Child(gachaTV,    true);
	container->Add_Child(gachaStage, true);
}

CGachaProps* CGachaProps::Create()
{
	auto inst = new CGachaProps();
	if (FAILED(inst->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CGachaProps");
		Safe_Release(inst);
	}
	return inst;
}

CGameObject* CGachaProps::Clone(INIT_DESC* pArg)
{
	auto inst = new CGachaProps(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CGachaProps");
		Safe_Release(inst);
	}
	return inst;
}