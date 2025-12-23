#include "pch.h"
#include "Unagi.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"

HRESULT CUnagi::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CUnagi::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void CUnagi::Awake()
{
	const string levelName = "First_Level";
	const string fileName  = "Avatar_Female_Size02_Unagi";

	const fs::path baseDir = fs::path("..") / "bin" / "Resources" / "Model";

	const string modelKey  = fileName + ".model";
	const string matKey    = fileName + ".mat";

	const string modelPath = (baseDir / modelKey).string();
	const string matPath   = (baseDir / matKey).string();

	RES->Add_ResourcePath(modelKey, modelPath);
	RES->Add_ResourcePath(matKey, matPath);

	Get_Component<CModel>()->Link_Model(levelName, modelKey);
	Get_Component<CMaterial>()->Link_Material(levelName, matKey);
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

void CUnagi::Free()
{
	__super::Free();
}