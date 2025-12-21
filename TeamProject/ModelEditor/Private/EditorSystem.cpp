#include "pch.h"
#include "EditorSystem.h"
#include "GameInstance.h"
#include "EditModel.h"
#include "RayCaster.h"
#include "Layer.h"
#include "AI_SKModel.h"
#include "AI_STModel.h"
#include "AIModelData.h"
#include "AIMesh.h"
#include "GameObject.h"
#include "ModelHelper.h"

IMPLEMENT_SINGLETON(CEditorSystem)

CEditorSystem::CEditorSystem()
{
}

HRESULT CEditorSystem::Initialize()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	pProto->Add_ProtoType("ModelEdit_Level", "Proto_GameObject_EditModel", CEditModel::Create());
	pProto->Add_ProtoType("ModelEdit_Level", "Proto_GameObject_RayCaster", CRayCaster::Create());

	IObjectService* pObjMgr = CGameInstance::GetInstance()->Get_ObjectMgr();
	CGameObject* RayCaster = Builder::Create_Object({ "ModelEdit_Level", "Proto_GameObject_RayCaster" }).Build("RayCaster");
	m_pRayCast = dynamic_cast<CRayCaster*>(RayCaster);

	for (int i = 0; i < 6; ++i)
	{
		string name = "Edit_Model_" + to_string(i);

		CGameObject* EditModel = Builder::Create_Object({ "ModelEdit_Level", "Proto_GameObject_EditModel" }).Position({}).Build(name.c_str());
		pObjMgr->Add_Object(EditModel, { "ModelEdit_Level", "Model_Layer" });
	}

	pObjMgr->Add_Object(RayCaster, { "ModelEdit_Level", "Ray_Layer" });
	m_pModelLayer = CGameInstance::GetInstance()->Get_ObjectMgr()->Get_Layer({ "ModelEdit_Level", "Model_Layer" });

	Load_MaterialMaps();
	Read_MaterialMaps();
	return S_OK;
}

void CEditorSystem::Update()
{
	
}

void CEditorSystem::CheckHittedMesh(CGameObject* pObject)
{
	if (pObject == nullptr)
		return;
	_bool isSkinned = { false };

	CAI_SKModel* pSkeletalModel = pObject->Get_Component<CAI_SKModel>();
	CAI_STModel* pStaticModel = pObject->Get_Component < CAI_STModel>();
	CAIModelData* pData = { nullptr };

	if (pSkeletalModel) {
		pData = pSkeletalModel->Get_AIModelData();
	}
	else if (pStaticModel) {
		pData = pStaticModel->Get_AIModelData();
	}
	else
		return;

	if (!pData)
		return;
	RAY nowRay = m_pRayCast->Get_Ray();
	_float4x4 nowTransform = pObject->Get_Component<CTransform>()->Get_WorldMatrix();
	for (_uint Index : pData->Get_ProxyIndex())
	{
		CAIMesh* pMehs = pData->Get_AIMesh(Index);
		auto result = pMehs->CheckRay(nowRay, nowTransform);
	
	}
}

void CEditorSystem::Load_MaterialMaps()
{
	ModelHelper::Add_ModelPathFromDirectory(m_Directorys, "../../Resource/Dir");
}

HRESULT CEditorSystem::Read_MaterialMaps()
{
	for (size_t i = 0; i < m_Directorys.size(); i++)
	{
		string path = m_Directorys[i];
		ifstream ifs(path);
		if (!ifs.is_open()) {
			string err = path + "파일을 찾을 수 없습니다.";
			MessageBoxA(nullptr, err.c_str(), "Error", MB_OK);
			return E_FAIL;
		}

		json jScene;
		try {
			ifs >> jScene;
		}
		catch (const json::parse_error& e) {
			MessageBoxA(nullptr, e.what(), "JSON Parse Error", MB_OK);
			return E_FAIL;
		}
		ifs.close();

		filesystem::path entry(m_Directorys[i]);
		string fileName = entry.stem().string();
		m_fileMaps.emplace(fileName, jScene);

	}
	return S_OK;
}

void CEditorSystem::Load_TextureMaps()
{
	
}

void CEditorSystem::Free()
{
}
