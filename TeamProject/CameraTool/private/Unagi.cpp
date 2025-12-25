#include "pch.h"
#include "Unagi.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

namespace
{
	struct AvatarAssetDesc
	{
		const char* folder;
		const char* fileName;
	};

	const AvatarAssetDesc& GetAvatarAssetDesc(Avatar v)
	{
		static const AvatarAssetDesc table[] =
		{
			{"Unagi",   "Avatar_Female_Size02_Unagi"          },
			{"Qingyi",  "Avatar_Female_Size01_QingYi"         },
			{"Corin",   "Avatar_Female_Size01_Corin"          },
			{"Belle",   "Avatar_Female_Size02_Belle_MainCity" },
			{"Alice",   "Avatar_Female_Size02_Alice"          },
			{"Astra",   "Avatar_Female_Size03_Astra"          },
			{"Burnice", "Avatar_Female_Size02_Burnice"        },
			{"Yixuan",  "Avatar_Female_Size03_YiXuan"         },
			{"Yuzuha",  "Avatar_Female_Size02_Yuzuha"         },
		};
		return table[(int)v];
	};

	fs::path GetPlayerBaseDir(Avatar v)
	{
		const auto& d = GetAvatarAssetDesc(v);
		return fs::path("..") / "bin" / "Resources" / "Model" / d.folder / "Anim";
	}
}

HRESULT CUnagi::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
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
	//Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CUnagi::ApplyAvatar(Avatar avatar)
{
	m_avatar = avatar;

	const string levelName = "First_Level";
	const auto& desc = GetAvatarAssetDesc(avatar);
	const string fileName = desc.fileName;

	const fs::path baseDir = GetPlayerBaseDir(avatar);

	const string modelKey = fileName + ".model";
	const string matKey   = fileName + ".mat";
	const string jsonKey  = fileName + "_Meta.json";

	const string modelPath = (baseDir / modelKey).string();
	const string matPath   = (baseDir / matKey).string();
	const string jsonPath  = (baseDir / jsonKey).string();

	RES->Add_ResourcePath(modelKey, modelPath);
	RES->Add_ResourcePath(matKey, matPath);
	RES->Add_ResourcePath(jsonKey, jsonPath);

	Get_Component<CModel>()->Link_Model(levelName, modelKey);
	Get_Component<CMaterial>()->Link_Material(levelName, matKey);
	Get_Component<CAnimator3D>()->LinkAnimate_Model(levelName, modelKey);
	Get_Component<CAnimator3D>()->Link_MetaData(levelName, jsonKey);
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