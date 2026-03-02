#include "pch.h"
#include "ToolModel.h"
#include "GameInstance.h"
#include "Helper_Func.h"

/* Component */
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

#include "MaterialInstance.h"
#include "MaterialData.h"

CToolModel::CToolModel()
	:CGameObject()
{
}

CToolModel::CToolModel(const CToolModel& rhs)
	:CGameObject(rhs)
{
}

HRESULT CToolModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CAnimator3D>();
	return S_OK;
}

HRESULT CToolModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CToolModel::Awake()
{
}

void CToolModel::Priority_Update(_float dt)
{
}

void CToolModel::Update(_float dt)
{
	if (m_IsLoaded)
		Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CToolModel::Late_Update(_float dt)
{
}

void CToolModel::Render_GUI()
{
	if(m_IsLoaded)
		__super::Render_GUI();
	LoadModel();
}

CToolModel* CToolModel::Create()
{
	CToolModel* instance = new CToolModel();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Clone Failed : CToolModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CToolModel::Clone(INIT_DESC* pArg)
{
	CToolModel* instance = new CToolModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CToolModel");
		Safe_Release(instance);
	}

	return instance;
}

void CToolModel::Free()
{
	__super::Free();
}

void CToolModel::LoadModel()
{
	if (ImGui::Button("Load Once"))
	{
		vector<string> files = Helper::OpenMultiFiles();
		if (files.size() != 3)
			return;
		
		string baseName;
		bool bModel = false, bMat = false, bJson = false;
		
		for (const auto& pathStr : files)
		{
			std::filesystem::path p(pathStr);
			string ext = p.extension().string();
			string stem = p.stem().string();
		
			if (ext == ".model")
			{
				if (bModel) return;
				bModel = true;
			}
			else if (ext == ".mat")
			{
				if (bMat) return;
				bMat = true;
			}
			else if (ext == ".json")
			{
				if (bJson) return;
				bJson = true;
			}
			else
				return;
		}
		
		if (!(bModel && bMat && bJson))
			return;
		
		string Model, Material, Meta;
		for (const auto& path : files)
		{
			string ext = std::filesystem::path(path).extension().string();
			string name = std::filesystem::path(path).stem().string() + ext;
		
			if (".model" == ext) {
				if (SUCCEEDED(ResourceManager()->Add_ResourcePath(name, path))) {
					Model = name;
				}
			}
			else if (".mat" == ext) {
				if (SUCCEEDED(ResourceManager()->Add_ResourcePath(name, path))) {
					Material = name;
				}
			}
			else if (".json" == ext) {
				if (SUCCEEDED(ResourceManager()->Add_ResourcePath(name, path))) {
					Meta = name;
				}
			}
		}
		
		auto pModel = Get_Component<CSkeletalModel>();
		auto pMaterial = Get_Component<CMaterial>();
		auto pAnimator = Get_Component<CAnimator3D>();
		//
		//string Model = "JaneDoeModel.model";
		//string Material = "JaneDoe.mat";
		//string Meta = "JaneDoeModel_Meata.json";
		//
		//ResourceManager()->Add_ResourcePath(Model, "D:/TeamPortpolio/TeamProject/TeamProject/Client/Bin/Resources/Model/skeletal/JaneDoe/JaneDoeModel.model");
		//ResourceManager()->Add_ResourcePath(Material,"D:/TeamPortpolio/TeamProject/TeamProject/Client/Bin/Resources/Model/skeletal/JaneDoe/JaneDoe.mat" );
		//ResourceManager()->Add_ResourcePath(Meta, "D:/TeamPortpolio/TeamProject/TeamProject/Client/Bin/Resources/Model/skeletal/JaneDoe/JaneDoeModel_Meta.json");

		pModel->Link_Model(G_GlobalLevelKey, Model);
		pMaterial->Link_Material(G_GlobalLevelKey, Material);
		pAnimator->LinkAnimate_Model(G_GlobalLevelKey, Model);
		pAnimator->Link_MetaData(G_GlobalLevelKey, Meta);

		m_IsLoaded = true;
	}
}
