#include "pch.h"
#include "Corin.h"

// Components
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "ObjectContainer.h"

HRESULT Corin::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	Add_Component<CAnimator3D>();
	return S_OK;
}

HRESULT Corin::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void Corin::Awake()
{
	__super::Awake();

	const string modelKey = "Corin.model";
	const string matKey   = "Corin.mat";
	const string metaKey  = "Avatar_Female_Size01_Corin_Meta.json";
	const string idleClip = "Avatar_Female_Size01_Corin_Ani_QuestStart";

	RES->Add_ResourcePath(modelKey, "../bin/Resources/Global/BattleCharacter/Corin/Corin.model");
	RES->Add_ResourcePath(matKey,   "../bin/Resources/Global/BattleCharacter/Corin/Corin.mat");
	RES->Add_ResourcePath(metaKey,  "Avatar_Female_Size01_Corin_Meta.json");

	Get_Component<CSkeletalModel>()->Link_Model(G_GlobalLevelKey, modelKey);
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, matKey);

	auto anim = Get_Component<CAnimator3D>();
	anim->LinkAnimate_Model(G_GlobalLevelKey, modelKey);
	anim->Link_MetaData(G_GlobalLevelKey, metaKey);

	anim->Set_Pause(false, 0);
	anim->Set_Animation(0, idleClip).Loop(true).Apply();
	anim->Update_Animation(0.f);
}

void Corin::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
}

Corin* Corin::Create()
{
	auto inst = new Corin();
	if (FAILED(inst->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : Corin");
		Safe_Release(inst);
	}
	return inst;
}

CGameObject* Corin::Clone(INIT_DESC* pArg)
{
	auto inst = new Corin(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : Corin");
		Safe_Release(inst);
	}
	return inst;
}
