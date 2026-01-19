#include "pch.h"
#include "Unagi.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

#include "ObjectContainer.h"

namespace fs = filesystem;

namespace
{
	struct AvatarAssetDesc
	{
		const char* folder;

		const char* modelKey;
		const char* modelFile;

		const char* matKey;
		const char* matFile;

		const char* metaKey;
		const char* metaFile;

		_float      modelScale;
		const char* idleClip;
	};

	const AvatarAssetDesc& GetAvatarAssetDesc(Avatar v)
	{
		static const AvatarAssetDesc table[] =
		{
			{
				"Corin",
				"Corin.model", "Corin.model",
				"Corin.mat", "Corin.mat",
				"Corin_Meta.json", "Corin_Meta.json",
				1.f,
				"Avatar_Female_Size01_Corin_Ani_Idle"
			},
			{
				"JaneDoe",
				"JaneDoe.model", "JaneDoeModel.model",
				"JaneDoe.mat", "JaneDoe.mat",
				"JaneDoe_Meta.json", "Avatar_Female_Size03_JaneDoe_Meta.json",
				0.01f,
				"Avatar_Female_Size03_JaneDoe_Ani_SwitchIn_Attack_Ex_Start"
				//"Avatar_Female_Size03_JaneDoe_Ani_Idle"
			}
		};
		return table[(int)v];
	}

	fs::path GetPlayerBaseDir(Avatar v)
	{
		const auto& d = GetAvatarAssetDesc(v);
		return fs::path("..") / "bin" / "Resources" / d.folder;
	}
}

HRESULT CUnagi::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	Add_Component<CAnimator3D>();
	return S_OK;
}

HRESULT CUnagi::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void CUnagi::Awake()
{
	ApplyAvatar(m_avatar);
}

void CUnagi::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CUnagi::ApplyAvatar(Avatar avatar)
{
	m_avatar = avatar;

	const string levelName = "First_Level";
	const auto& desc = GetAvatarAssetDesc(avatar);

	const fs::path baseDir = GetPlayerBaseDir(avatar);

	const string modelPath = (baseDir / desc.modelFile).string();
	const string matPath = (baseDir / desc.matFile).string();
	const string metaPath = (baseDir / desc.metaFile).string();

	RES->Add_ResourcePath(desc.modelKey, modelPath);
	RES->Add_ResourcePath(desc.matKey, matPath);
	RES->Add_ResourcePath(desc.metaKey, metaPath);

	Get_Component<CTransform>()->Scale(Vector3{desc.modelScale, desc.modelScale, desc.modelScale});

	auto anim = Get_Component<CAnimator3D>();

	Get_Component<CModel>()->Link_Model(levelName, desc.modelKey);
	Get_Component<CMaterial>()->Link_Material(levelName, desc.matKey);
	anim->LinkAnimate_Model(levelName, desc.modelKey);
	anim->Link_MetaData(levelName, desc.metaKey);

	anim->Set_Pause(false, 0);
	anim->Set_Animation(0, string(desc.idleClip)).Loop(true).Apply();
	anim->Update_Animation(0.f);
}

CUnagi* CUnagi::Create()
{
	CUnagi* instance = new CUnagi();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoModel");
		Safe_Release(instance);
	}
	return instance;
}

CGameObject* CUnagi::Clone(INIT_DESC* pArg)
{
	CUnagi* instance = new CUnagi(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoModel");
		Safe_Release(instance);
	}
	return instance;
}