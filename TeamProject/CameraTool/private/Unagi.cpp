#include "pch.h"
#include "Unagi.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

namespace
{
	struct PlayerAssetDesc
	{
		const char* folder;
		const char* fileName;
	};

	const PlayerAssetDesc& GetPlayerAssetDesc(Player v)
	{
		static const PlayerAssetDesc table[] =
		{
			{"Unagi",  "Avatar_Female_Size02_Unagi" },
			{"Qingyi", "Avatar_Female_Size01_QingYi"},
		};
		return table[(int)v];
	};

	fs::path GetPlayerBaseDir(Player v)
	{
		const auto& d = GetPlayerAssetDesc(v);
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
	const Player player = Player::QingYi;

	const string levelName = "First_Level";

	const auto& d = GetPlayerAssetDesc(player);
	const string fileName = d.fileName;

	const fs::path baseDir = GetPlayerBaseDir(player);

	const string modelKey  = fileName + ".model";
	const string matKey    = fileName + ".mat";
	const string jsonKey   = fileName + "_Meta.json";

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

void CUnagi::Update(_float dt)
{
	//Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CUnagi::Render_GUI()
{
	__super::Render_GUI();
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