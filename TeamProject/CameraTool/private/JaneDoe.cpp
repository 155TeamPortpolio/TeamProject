#include "pch.h"
#include "JaneDoe.h"

// Components
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "ObjectContainer.h"

HRESULT JaneDoe::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	Add_Component<CAnimator3D>();
	return S_OK;
}

HRESULT JaneDoe::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void JaneDoe::Awake()
{
	__super::Awake();

	const string modelKey  = "JaneDoeModel.model";
	const string matKey    = "JaneDoe.mat";
	const string metaKey   = "JaneDoe_Meta.json";
	const string idleClip  = "JaneDoe_Ani_Idle";

	RES->Add_ResourcePath(modelKey, "../bin/Resources/JaneDoe/JaneDoeModel.model");
	RES->Add_ResourcePath(matKey,   "../bin/Resources/JaneDoe/JaneDoe.mat");
	RES->Add_ResourcePath(metaKey,  "../bin/Resources/JaneDoe/JaneDoe_Meta.json");

	Get_Component<CSkeletalModel>()->Link_Model(G_GlobalLevelKey, modelKey);
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, matKey);

	auto anim = Get_Component<CAnimator3D>();
	anim->LinkAnimate_Model(G_GlobalLevelKey, modelKey);
	anim->Link_MetaData(G_GlobalLevelKey, metaKey);

	anim->Set_Pause(false, 0);
	anim->Set_Animation(0, idleClip).Loop(true).Apply();
	anim->Update_Animation(0.f);
}

void JaneDoe::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
}

JaneDoe* JaneDoe::Create()
{
	auto inst = new JaneDoe();
	if (FAILED(inst->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : JaneDoe");
		Safe_Release(inst);
	}
	return inst;
}

CGameObject* JaneDoe::Clone(INIT_DESC* pArg)
{
	auto inst = new JaneDoe(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : JaneDoe");
		Safe_Release(inst);
	}
	return inst;
}
