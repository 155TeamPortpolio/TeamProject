#include "pch.h"
#include "MapToolCore.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Helper_MapTool.h"

#include "PlacedObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CMapToolCore)

CMapToolCore::CMapToolCore()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

vector<LOADED_OBJECT> CMapToolCore::Load_MapData()
{
	filesystem::path OpenPath = Helper::OpenFile_Dialogue();

	if (OpenPath.empty())
		return vector<LOADED_OBJECT>();

	if (OpenPath.extension().string() != ".json") {
		MSG_BOX("[MapTool] Load Map Data Failed.\nJson 파일이 아닙니다.");
		return vector<LOADED_OBJECT>();
	}


	MapData_Header mapdata = Helper::LoadJson<MapData_Header>(OpenPath.string());

	if (mapdata.iVersion != m_tMapToolContext.iVersion) {
		MSG_BOX("[MapTool] Load Map Data Failed.\n잘못된 버전입니다.");
		return vector<LOADED_OBJECT>();
	}

	m_tMapToolContext.TagArea = mapdata.TagArea;

	vector<LOADED_OBJECT>	LodedObjects;

	for (auto& layerdata : mapdata.Layers) {
		// 레이어 태그 무결성 검사
		MAPOBJ_TYPE eType = Check_LayerTag(layerdata.TagLayer);
		if (MAPOBJ_TYPE::END == eType)
			continue;

		for (auto& objectdata : layerdata.Objects) {
			switch (eType)
			{
			case MAPOBJ_TYPE::PLACED:
				Place_PlacedObjectFromLoadData(&objectdata);
				break;
			case MAPOBJ_TYPE::TRIGGER:
				break;
			}
			LOADED_OBJECT Desc = {};
			Desc.iObjIdx = objectdata.iObjID;
			Desc.TagModelKey = objectdata.TagModelResourceKey;

			LodedObjects.push_back(Desc);
		}
	}

	return LodedObjects;
}

void CMapToolCore::Clear_Layer(MAPOBJ_TYPE eObjType)
{
	_uint i = ENUM(eObjType);

	if (m_tMapToolContext.TagLayers[ENUM(eObjType)] == "All_Layer") {
		for (_int j = 0; j < m_tMapToolContext.TagLayers.size(); j++)
		{
			CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, m_tMapToolContext.TagLayers[j] });
			if (nullptr == pLayer)
				continue;
			pLayer->Clear_Layer();
		}
	}
	else {
		CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, m_tMapToolContext.TagLayers[ENUM(eObjType)] });
		if (nullptr == pLayer) {
			ImGui::PopID();
			return;
		}
		pLayer->Clear_Layer();
	}
	m_pGameInstance->Get_GUISystem()->Get_Context()->pSelectedObject = { nullptr };

}

MAPOBJ_TYPE CMapToolCore::Check_LayerTag(const string& TagLayer)
{
	MAPOBJ_TYPE eType = {};
	if (m_tMapToolContext.TagLayers[ENUM(MAPOBJ_TYPE::PLACED)] == TagLayer)
		eType = MAPOBJ_TYPE::PLACED;
	else if (m_tMapToolContext.TagLayers[ENUM(MAPOBJ_TYPE::TRIGGER)] == TagLayer)
		eType = MAPOBJ_TYPE::TRIGGER;
	else
		eType = MAPOBJ_TYPE::END;

	return eType;
}

void CMapToolCore::Place_PlacedObjectFromLoadData(MapData_Object* pData)
{
	if (nullptr == pData)
		return;

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();

	CPlacedObject::MAPTOOL_OBJECT_DESC* PlacedObjDesc = new CPlacedObject::MAPTOOL_OBJECT_DESC;
	PlacedObjDesc->iObjectIndex = pData->iObjID;
	PlacedObjDesc->TagModelKey = pData->TagModelResourceKey;
	PlacedObjDesc->TagMaterialKey = pData->TagMaterialResourceKey;

	COLLIDER_DESC ColDesc = {};
	//ColDesc.bCooking = true;
	ColDesc.bAutoFit = true;
	ColDesc.strModelKey = pData->TagModelResourceKey;

	_float3 vScl{}, vRot{}, vTrans{};
	_float4 vRotQ{};

	// collider에 위치정보 구우려면 빌더단계에서ㄱㄱ
	_float4x4 matWorld = {
		pData->vRight[0], pData->vRight[1] , pData->vRight[2] , pData->vRight[3],
		pData->vUp[0],  pData->vUp[1] , pData->vUp[2] , pData->vUp[3],
		pData->vLook[0], pData->vLook[1] , pData->vLook[2] , pData->vLook[3],
		pData->vPos[0], pData->vPos[1] , pData->vPos[2] , pData->vPos[3] };

	if (true == HelperMT::ExtractSRT(XMLoadFloat4x4(&matWorld), vScl, vRotQ, vTrans))
		vRot = HelperMT::QuaternionToEuler(vRotQ);

	string fileName = Helper::GetFileNameWithOutExtension(pData->TagModelResourceKey);

	CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,"Proto_GameObject_PlacedObject" })
		.Add_ObjDesc(PlacedObjDesc)
		.Scale(vScl)
		.Rotate(vRot)
		.Position(vTrans)
		.Collider(ColDesc)
		.Build(fileName);
 
#ifdef _DEBUG
	pStaticObject->Get_Component<CCollider>()->Set_DebugRender(m_tMapToolContext.isAllDebugRender);
#endif // _DEBUG

	pObjMgr->Add_Object(pStaticObject, { g_TagMapToolLevel, m_tMapToolContext.TagLayers[ENUM(MAPOBJ_TYPE::PLACED)] });


}

#ifdef _DEBUG

void CMapToolCore::Set_AllObjectDebugRender(_bool is)
{
	m_tMapToolContext.isAllDebugRender = is;
	for (auto& Pair : m_pGameInstance->Get_ObjectMgr()->Get_LevelLayer(g_TagMapToolLevel)) {
		for (auto& pObject : Pair.second->Get_AllObject()) {
			if (nullptr != pObject && pObject->Get_Component<CCollider>()) {
				pObject->Get_Component<CCollider>()->Set_DebugRender(is);
			}
		}
	}
}

#endif // _DEBUG

void CMapToolCore::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
