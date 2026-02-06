#include "pch.h"
#include "MapToolCore.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Helper_MapTool.h"

#include "PlacedObject.h"
#include "EntityObject.h"
#include "Layer.h"
#include "BattleObject.h"
#include "MapToolGui.h"
#include "LightPoint.h"

IMPLEMENT_SINGLETON(CMapToolCore)

CMapToolCore::CMapToolCore()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

LOADED_DATA CMapToolCore::Load_MapData()
{
	filesystem::path OpenPath = Helper::OpenFile_Dialogue();

	if (OpenPath.empty())
		return {};

	if (OpenPath.extension().string() != ".json") {
		MSG_BOX("[MapTool] Load Map Data Failed.\nJson 파일이 아닙니다.");
		return {};
	}

	LOADED_DATA	LoadedData = {};

	/* MapData일 때 */
	if (OpenPath.string().find("MapData") != string::npos) 
	{
		MapData_Header mapdata = Helper::LoadJson<MapData_Header>(OpenPath.string());

		if ("Base" != mapdata.TagDataFormat)
		{
			MSG_BOX("[MapTool] Load Map Data Failed.\nBaseData가 아닙니다.");
			return {};
		}

		Clear_Layer(MAPOBJ_TYPE::PLACED);
		Clear_Layer(MAPOBJ_TYPE::TRIGGER);

		LoadedData.tagDataFormat = "MapData";
		//if (mapdata.iVersion != m_tMapToolContext.iVersion) {
		//	MSG_BOX("[MapTool] Load Map Data Failed.\n잘못된 버전입니다.");
		//	return vector<LOADED_OBJECT>();
		//}
		m_tMapToolContext.iVersion = mapdata.iVersion;
		m_tMapToolContext.TagArea = mapdata.TagArea;

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
					Place_TriggerObjectFromLoadData(&objectdata);
					break;
				}
				LOADED_OBJECT Desc = {};
				Desc.iObjIdx = objectdata.iObjID;
				Desc.TagModelKey = objectdata.TagModelResourceKey;

				LoadedData.LoadedObjects.push_back(Desc);
			}
		}
	}
	else if (OpenPath.string().find("EntityData") != string::npos)
	{
		Entity_Header EntityHeader = Helper::LoadJson<Entity_Header>(OpenPath.string());
	
		if ("Base" != EntityHeader.TagDataFormat)
		{
			MSG_BOX("[MapTool] Load Entity Data Failed.\nBaseData가 아닙니다.");
			return {};
		}

		Clear_Layer(MAPOBJ_TYPE::ENTITY);

		LoadedData.tagDataFormat = "EntityData";
		m_tMapToolContext.iVersion = EntityHeader.iVersion;
		m_tMapToolContext.TagArea = EntityHeader.TagArea;

		for (auto& EntityData : EntityHeader.Entities)
		{
			Place_EntityObjectFromLoadData(&EntityData);
			LOADED_OBJECT Desc = {};
			Desc.iObjIdx = EntityData.iEntityID;
			Desc.TagModelKey = EntityData.tagName;

			LoadedData.LoadedObjects.push_back(Desc);
		}
	}
	else if (OpenPath.string().find("BattleData") != string::npos)
	{
		m_pMapToolGui->Load_BattleData(OpenPath.string());
	}
	else if (OpenPath.string().find("LightData") != string::npos)
	{
		Light_Header LightHeader = Helper::LoadJson<Light_Header>(OpenPath.string());

		if ("Base" != LightHeader.TagDataFormat)
		{
			MSG_BOX("[MapTool] Load Entity Data Failed.\nBaseData가 아닙니다.");
			return {};
		}

		Clear_Layer(MAPOBJ_TYPE::LIGHT);

		LoadedData.tagDataFormat = "LightData";
		m_tMapToolContext.iVersion = LightHeader.iVersion;
		m_tMapToolContext.TagArea = LightHeader.TagArea;

		for (auto& LightData : LightHeader.Lights)
		{
			Place_LightPointFromLoadData(&LightData);
			LOADED_OBJECT Desc = {};
			Desc.iObjIdx = LightData.iIndex;
			Desc.TagModelKey = LoadedData.tagDataFormat + to_string(LightData.iIndex);

			LoadedData.LoadedObjects.push_back(Desc);
		}
	}
	else
	{
		MSG_BOX("[MapTool] Load Data Failed.\n일치하는 DataFormat이 없습니다.");
		return {};
	}

	return LoadedData;
}

void CMapToolCore::Load_WithEntityData()
{
	filesystem::path OpenPath = Helper::OpenFile_Dialogue();

	if (OpenPath.empty())
		return;

	if (OpenPath.extension().string() != ".json") {
		MSG_BOX("[MapTool] Load Map Data Failed.\nJson 파일이 아닙니다.");
		return;
	}

	LOADED_DATA	LoadedData = {};

	if (OpenPath.string().find("MapData") == string::npos) {
		MSG_BOX("맵 데이터를 부르세요 :)");
		return;
	}

	/* MapData 불러오기 */
	MapData_Header mapdata = Helper::LoadJson<MapData_Header>(OpenPath.string());

	if ("Base" != mapdata.TagDataFormat)
	{
		MSG_BOX("[MapTool] Load Map Data Failed.\nBaseData가 아닙니다.");
		return;
	}

	Clear_Layer(MAPOBJ_TYPE::PLACED);
	Clear_Layer(MAPOBJ_TYPE::TRIGGER);

	LoadedData.tagDataFormat = "MapData";
	m_tMapToolContext.iVersion = mapdata.iVersion;
	m_tMapToolContext.TagArea = mapdata.TagArea;

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
				Place_TriggerObjectFromLoadData(&objectdata);
				break;
			}
			LOADED_OBJECT Desc = {};
			Desc.iObjIdx = objectdata.iObjID;
			Desc.TagModelKey = objectdata.TagModelResourceKey;

			LoadedData.LoadedObjects.push_back(Desc);
		}
	}

	LoadEntity(OpenPath, LoadedData);
	LoadBattle(OpenPath, LoadedData);
	LoadLight(OpenPath, LoadedData);
}

void CMapToolCore::LoadEntity(const filesystem::path& BasePath, LOADED_DATA& LoadedData)
{
	Clear_Layer(MAPOBJ_TYPE::ENTITY);

	filesystem::path entityPath = BasePath;
	string filename = entityPath.filename().string();

	size_t pos = filename.find("MapData");

	if (pos != string::npos) {
		filename.replace(pos, strlen("MapData"), "EntityData");
		entityPath.replace_filename(filename);

		if (filesystem::exists(entityPath)) {
			Entity_Header EntityHeader = Helper::LoadJson<Entity_Header>(entityPath.string());
			LoadedData.tagDataFormat = "EntityData";

			m_tMapToolContext.iVersion = EntityHeader.iVersion;
			m_tMapToolContext.TagArea = EntityHeader.TagArea;

			for (auto& EntityData : EntityHeader.Entities)
			{
				Place_EntityObjectFromLoadData(&EntityData);
				LOADED_OBJECT Desc = {};
				Desc.iObjIdx = EntityData.iEntityID;
				Desc.TagModelKey = EntityData.tagName;

				LoadedData.LoadedObjects.push_back(Desc);
			}
		}
	}
}

void CMapToolCore::LoadBattle(const filesystem::path& BasePath, LOADED_DATA& LoadedData)
{
	Clear_Layer(MAPOBJ_TYPE::BATTLE);
	m_pMapToolGui->Clear_BattleData();

	filesystem::path BattlePath = BasePath;
	string filename = BattlePath.filename().string();

	size_t pos = filename.find("MapData");
	if (pos == string::npos)
		return;

	filename.replace(pos, strlen("MapData"), "BattleData");
	BattlePath.replace_filename(filename);

	pos = filename.find(".Base.1");

	filename.replace(pos, strlen(".Base.1"), "");
	BattlePath.replace_filename(filename);

	if (!filesystem::exists(BattlePath))
		return;

	m_pMapToolGui->Load_BattleData(BattlePath.string());
}

void CMapToolCore::LoadLight(const filesystem::path& BasePath, LOADED_DATA& LoadedData)
{
	Clear_Layer(MAPOBJ_TYPE::LIGHT);

	filesystem::path lightPath = BasePath;
	string filename = lightPath.filename().string();

	size_t pos = filename.find("MapData");

	if (pos != string::npos) {
		filename.replace(pos, strlen("MapData"), "LightData");
		lightPath.replace_filename(filename);

		if (filesystem::exists(lightPath)) {
			Light_Header LightHeader = Helper::LoadJson<Light_Header>(lightPath.string());
			LoadedData.tagDataFormat = "LightData";

			m_tMapToolContext.iVersion = LightHeader.iVersion;
			m_tMapToolContext.TagArea = LightHeader.TagArea;

			for (auto& LightData : LightHeader.Lights)
			{
				Place_LightPointFromLoadData(&LightData);
				LOADED_OBJECT Desc = {};
				Desc.iObjIdx = LightData.iIndex;
				Desc.TagModelKey = "Light_Point" + to_string(LightData.iIndex);

				LoadedData.LoadedObjects.push_back(Desc);
			}
		}
	}
}

void CMapToolCore::Clear_Layer(MAPOBJ_TYPE eObjType)
{
	_uint i = ENUM(eObjType);

	if (g_tagMapObjType[ENUM(eObjType)] == "All_Layer") {
		for (_int j = 0; j < ENUM(MAPOBJ_TYPE::END); j++)
		{
			CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagMapObjType[j] });
			if (nullptr == pLayer)
				continue;
			pLayer->Clear_Layer();
		}

		for (_int j = 0; j < ENUM(BATTLE_TYPE::END); j++)
		{
			CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagBattleObjType[j] });
			if (nullptr == pLayer)
				continue;
			pLayer->Clear_Layer();
		}		
	}
	else if (MAPOBJ_TYPE::BATTLE == eObjType) {
		for (_int j = 0; j < ENUM(BATTLE_TYPE::END); j++)
		{
			CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagBattleObjType[j] });
			if (nullptr == pLayer)
				continue;
			pLayer->Clear_Layer();
		}
	}
	else {
		CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagMapObjType[ENUM(eObjType)] });
		if (nullptr == pLayer) {
			//ImGui::PopID();
			return;
		}
		pLayer->Clear_Layer();
	}
	m_pGameInstance->Get_GUISystem()->Get_Context()->pSelectedObject = { nullptr };

}

void CMapToolCore::RegisterGuiPanel(CMapToolGui* pGUIPanel)
{
	m_pMapToolGui = pGUIPanel;
}

MAPOBJ_TYPE CMapToolCore::Check_LayerTag(const string& TagLayer)
{
	MAPOBJ_TYPE eType = {};
	if (g_tagMapObjType[ENUM(MAPOBJ_TYPE::PLACED)] == TagLayer)
		eType = MAPOBJ_TYPE::PLACED;
	else if (g_tagMapObjType[ENUM(MAPOBJ_TYPE::TRIGGER)] == TagLayer)
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

	// Load된 PlacedObject(통맵기준)는 배치가 끝났다 가정.(Trigger나 그 외 요소 편하게 붙이기 위해 cooking)
	COLLIDER_DESC ColDesc = {};
	ColDesc.bCooking = true;
	//ColDesc.bAutoFit = true;
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
 
	pStaticObject->Get_Component<CCollider>()->Set_DebugRender(m_tMapToolContext.isAllDebugRender);

	pObjMgr->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::PLACED)] });


}

void CMapToolCore::Place_TriggerObjectFromLoadData(MapData_Object* pData)
{
	if (nullptr == pData)
		return;

	COLLIDER_TYPE eType = {};
	if ("BOX" == pData->TagMaterialResourceKey)
		eType = COLLIDER_TYPE::BOX;
	else if ("SPHERE" == pData->TagMaterialResourceKey)
		eType = COLLIDER_TYPE::SPHERE;
	else if ("CAPSULE" == pData->TagMaterialResourceKey)
		eType = COLLIDER_TYPE::CAPSULE;
	else
		return;


	COLLIDER_DESC ColDesc = {};
	ColDesc.eType = eType;
	ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
	ColDesc.vCenter = { pData->vRight[0], pData->vRight[1], pData->vRight[2] };
	ColDesc.vSize = { pData->vUp[0], pData->vUp[1], pData->vUp[2] };
	ColDesc.vRotation = { XMConvertToRadians(pData->vLook[0]),
						  XMConvertToRadians(pData->vLook[1]),
						  XMConvertToRadians(pData->vLook[2]) };

	CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,"Proto_GameObject_TriggerObject" })
		.Collider(ColDesc)
		.Position({ pData->vPos[0], pData->vPos[1], pData->vPos[2] })
		.Build(pData->TagModelResourceKey);

	pStaticObject->Get_Component<CCollider>()->Set_DebugRender(m_tMapToolContext.isAllDebugRender);

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	pObjMgr->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::TRIGGER)] });
}

void CMapToolCore::Place_EntityObjectFromLoadData(ENTITY* pData)
{
	COLLIDER_DESC ColDesc = {};
	ColDesc.eType = COLLIDER_TYPE::BOX;
	ColDesc.bAutoFit = false;
	ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
	ColDesc.vSize = { pData->vColSize[0], pData->vColSize[1], pData->vColSize[2] };
	CEntityObject::ENTITY_INIT_DESC* pDesc = new CEntityObject::ENTITY_INIT_DESC();
	pDesc->iType = pData->iType;

	string TagInstanceName = pData->tagName;
	CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,"Proto_GameObject_EntityObject" })
		.Add_ObjDesc(pDesc)
		.Collider(ColDesc)
		.Rotate({ pData->vRotation[0], pData->vRotation[1], pData->vRotation[2] })
		.Scale({ pData->vScale[0], pData->vScale[1], pData->vScale[2] })
		.Position({ pData->vTranslation[0], pData->vTranslation[1], pData->vTranslation[2] })
		.Build(TagInstanceName);

	pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

	ObjectManager()->Add_Object(pStaticObject, {g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::ENTITY)]});
}

void CMapToolCore::Place_LightPointFromLoadData(MAP_LIGHT* pData)
{
	COLLIDER_DESC ColDesc = {};
	ColDesc.eType = COLLIDER_TYPE::SPHERE;
	ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거

	CLightPoint::LIGHT_INIT_DESC* pDesc = new CLightPoint::LIGHT_INIT_DESC();
	
	string Name = "LightPoint" + to_string(pData->iIndex);
	pDesc->DescJson = pData->LightDesc;

	_quaternion Rot = pDesc->DescJson.vLightDirection;

	CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,"Proto_GameObject_LightPoint" })
		.Add_ObjDesc(pDesc)
		.Collider(ColDesc)
		.Position({ pData->vTranslation[0], pData->vTranslation[1], pData->vTranslation[2] })
		.Rotate(Rot.ToEuler())
		.Build(Name);

	pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

	ObjectManager()->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::LIGHT)] });
}

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

void CMapToolCore::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
