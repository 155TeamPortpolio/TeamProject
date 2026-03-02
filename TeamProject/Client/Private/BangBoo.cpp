#include "pch.h"
#include "BangBoo.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

CBangBoo::CBangBoo()
{
}

CBangBoo::CBangBoo(const CBangBoo& rhs)
    :CNpc(rhs)
{
}

HRESULT CBangBoo::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

	Add_Component<CObjectContainer>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CAnimator3D>();
	Add_Component<CCharacterController>();

    return S_OK;
}

HRESULT CBangBoo::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CBangBoo::Awake()
{
	//__super::Awake();
}

void CBangBoo::Priority_Update(_float dt)
{
}

void CBangBoo::Update(_float dt)
{
	if(Get_Component<CAnimator3D>())
		Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CBangBoo::Late_Update(_float dt)
{
}

void CBangBoo::Set_BangBoo_Model(const string& BangBooTag)
{
	if (BangBooTag.empty())
		return;

	string tagModel{}, tagMaterial{}, tagMeta{};
	_bool bExist = false;

	if (BangBooTag == "AgentBoo") {
		tagModel = "Bangboo_AgentGulliver_Modelout.model";
		tagMaterial ="Bangboo_AgentGulliver_Modelout.mat";
		tagMeta = "Bangboo_AgentGulliver_Meta.json";
		bExist = true;
	}
	else if (BangBooTag == "BagBoo") {
		tagModel = "Bangboo_Bagboo_Modelout.model";
		tagMaterial = "Bangboo_Bagboo_Modelout.mat";
		tagMeta = "Bangboo_Bagboo_Meta.json";
		bExist = true;
	}
	else if (BangBooTag == "DevilBoo") {
		tagModel = "Bangboo_Devilboo_Modelout.model";
		tagMaterial = "Bangboo_Devilboo_Modelout.mat";
		tagMeta = "Bangboo_Devilboo_Meta.json";
		bExist = true;
	}
	else if (BangBooTag == "EousBoo") {
		tagModel = "Bangboo_Eous001_Modelout.model";
		tagMaterial = "Bangboo_Eous001_Modelout.mat";
		tagMeta = "Bangboo_Eous001_Meta.json";
		bExist = true;
	}
	else if (BangBooTag == "ExploreBoo") {
		tagModel = "Exploreboo.model";
		tagMaterial = "Exploreboo.mat";
		tagMeta = "NPC_Bangboo_Exploreboo_Meta.json";
		bExist = true;
	}
	else if (BangBooTag == "PenguinBoo") {
		tagModel = "Bangboo_Penguinboo_Model_WithRootKeeperout.model";
		tagMaterial = "Bangboo_Penguinboo_Model_WithRootKeeperout.mat";
		tagMeta = "NPC_Bangboo_Penguinboo_Meta.json";
		bExist = true;
	}
	else if (BangBooTag == "RocketBoo") {
		tagModel = "Bangboo_Rocketboo_Modelout.model";
		tagMaterial = "Bangboo_Rocketboo_Modelout.mat";
		tagMeta = "Bangboo_Rocketboo_Meta.json";
		bExist = true;
	}
	else if (BangBooTag == "SafetyBoo") {
		tagModel = "Bangboo_Safety_Modelout.model";
		tagMaterial = "Bangboo_Safety_Modelout.mat";
		tagMeta = "Bangboo_Safety_Meta.json";
		bExist = true;
	}
	else if (BangBooTag == "SumoBoo") {
		tagModel = "Sumoboo.model";
		tagMaterial = "Sumoboo.mat";
		tagMeta = "NPC_Bangboo_Sumoboo_Meta.json";
		bExist = true;
	}

	if (!bExist) return;

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Sumoboo.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Sumoboo.mat");
	Get_Component<CSkeletalModel>()->Link_Model(G_GlobalLevelKey, tagModel);
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, tagMaterial);
	Get_Component<CAnimator3D>()->LinkAnimate_Model(G_GlobalLevelKey, tagModel);
	Get_Component<CAnimator3D>()->Link_MetaData(G_GlobalLevelKey, tagMeta);
	Get_Component<CAnimator3D>()->Set_Animation(0)
		.Loop(true)
		.Apply();

	__super::Awake();
}

void CBangBoo::Set_BangBoo_Animation(const string& AnimationTag)
{
	if (!Get_Component<CAnimator3D>())
		return;

	Get_Component<CAnimator3D>()->Set_Animation(AnimationTag)
		.Loop(true)
		.Apply();
}

void CBangBoo::Set_BangBoo_Name(const wstring& NameTag)
{
	m_strName = NameTag;
	Add_NameIndicator();
}

void CBangBoo::Set_BangBoo_Speech(const wstring& SpeechString)
{
	m_strSpeech = SpeechString;
	Add_SpeechBubble();
}

CBangBoo* CBangBoo::Create()
{
	CBangBoo* instance = new CBangBoo();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CBangBoo");
	}

	return instance;
}

CGameObject* CBangBoo::Clone(INIT_DESC* pArg)
{
	CBangBoo* instance = new CBangBoo(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CBangBoo");
	}
	return instance;
}

void CBangBoo::Free()
{
	__super::Free();
}