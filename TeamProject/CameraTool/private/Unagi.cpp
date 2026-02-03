#include "pch.h"
#include "Unagi.h"

// Components
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "ObjectContainer.h"

HRESULT Unagi::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	Add_Component<CAnimator3D>();
	return S_OK;
}

HRESULT Unagi::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void Unagi::Awake()
{
	__super::Awake();

	const string modelKey = "Miyabi.model";
	const string matKey   = "Miyabi.mat";
	const string metaKey  = "Miyabi_Meta.json";
	const string idleClip = "_Unagi_Ani_Idle";

	RES->Add_ResourcePath(modelKey, "../bin/Resources/Global/BattleCharacter/Miyabi/Miyabi.model");
	RES->Add_ResourcePath(matKey,   "../bin/Resources/Global/BattleCharacter/Miyabi/Miyabi.mat");
	RES->Add_ResourcePath(metaKey,  "Miyabi_Meta.json");

	Get_Component<CSkeletalModel>()->Link_Model(G_GlobalLevelKey, modelKey);
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey,   matKey);

	auto anim = Get_Component<CAnimator3D>();
	anim->LinkAnimate_Model(G_GlobalLevelKey, modelKey);
	anim->Link_MetaData(G_GlobalLevelKey,     metaKey);

	anim->Set_Pause(false, 0);
	anim->Set_Animation(0, idleClip).Loop(true).Apply();
	anim->Update_Animation(0.f);
}

void Unagi::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
}

Unagi* Unagi::Create()
{
	auto inst = new Unagi();
	if (FAILED(inst->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : Unagi");
		Safe_Release(inst);
	}
	return inst;
}

CGameObject* Unagi::Clone(INIT_DESC* pArg)
{
	auto inst = new Unagi(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : Unagi");
		Safe_Release(inst);
	}
	return inst;
}
