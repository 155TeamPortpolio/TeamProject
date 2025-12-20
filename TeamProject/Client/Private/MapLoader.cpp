#include "pch.h"
#include "MapLoader.h"
#include "MapData_Defines.h"
#include "GameInstance.h"
#include "Helper_Func.h"

#include "MapPlacedObject.h"

CMapLoader::CMapLoader()
    : m_TagLayers{ "PlacedObject_Layer", "FloorObject_Layer", "TriggerObject_Layer", "Navigation_Layer"}
{
}

HRESULT CMapLoader::Initialize(const string TagLevel, const string MapDataPath)
{
    m_TagLevel = TagLevel;

    filesystem::path OpenPath = MapDataPath;

    if (OpenPath.empty())
        return E_FAIL;

    if (OpenPath.extension().string() != ".json") {
        MSG_BOX("[MapTool] Load Map Data Failed.\nJson 파일이 아닙니다.");
        return E_FAIL;
    }

    MapData_Header mapdata = Helper::LoadJson<MapData_Header>(OpenPath.string());
    if (-1 == mapdata.iVersion)
        return E_FAIL;


    if (mapdata.iVersion != m_iVersion) {
        MSG_BOX("[MapTool] Load Map Data Failed.\n잘못된 버전입니다.");
        return E_FAIL;
    }
    
    for (auto& layerdata : mapdata.Layers) {
        // 레이어 태그 무결성 검사
        /*const auto iter = find(m_TagLayers.begin(), m_TagLayers.end(), layerdata.TagLayer);
        if (iter == m_TagLayers.end())
            continue;*/
        MAPOBJ_TYPE eType = Check_LayerTag(layerdata.TagLayer);
        if (MAPOBJ_TYPE::END == eType)
            continue;

        for (auto& objectdata : layerdata.Objects) {
            switch (eType)
            {
            case Client::CMapLoader::MAPOBJ_TYPE::PLACED:
                Place_PlacedObjectFromLoadData(&objectdata);
                break;
            case Client::CMapLoader::MAPOBJ_TYPE::FLOOR:
                break;
            case Client::CMapLoader::MAPOBJ_TYPE::TRIGGER:
                break;
            }
        }
            
    }

	return S_OK;
}

void CMapLoader::Place_PlacedObjectFromLoadData(MapData_Object* pData)
{
    if (nullptr == pData)
        return;

    IObjectService* pObjMgr = CGameInstance::GetInstance()->Get_ObjectMgr();

    CMapPlacedObject::MAPOBJ_DESC* Desc = new CMapPlacedObject::MAPOBJ_DESC;
    Desc->TagLevel = m_TagLevel;
    Desc->TagModelKey = pData->TagModelResourceKey;
    Desc->TagMaterialKey = pData->TagMaterialResourceKey;

    CGameObject* pStaticObject = Builder::Create_Object({ m_TagLevel ,"Proto_GameObject_MapPlacedObject" })
        .Add_ObjDesc(Desc)
        .Build("Placed_Model");

    if (nullptr == pStaticObject)
        return;

    _float4x4 matWorld = {
        pData->vRight[0], pData->vRight[1] , pData->vRight[2] , pData->vRight[3],
        pData->vUp[0],  pData->vUp[1] , pData->vUp[2] , pData->vUp[3],
        pData->vLook[0], pData->vLook[1] , pData->vLook[2] , pData->vLook[3],
        pData->vPos[0], pData->vPos[1] , pData->vPos[2] , pData->vPos[3] };

    pStaticObject->Get_Component<CTransform>()->TranslateMatrix(XMLoadFloat4x4(&matWorld));

    pObjMgr->Add_Object(pStaticObject, { m_TagLevel, m_TagLayers[ENUM(MAPOBJ_TYPE::PLACED)]});
}

CMapLoader::MAPOBJ_TYPE CMapLoader::Check_LayerTag(const string& TagLayer)
{
    MAPOBJ_TYPE eType = {};
    if (m_TagLayers[ENUM(MAPOBJ_TYPE::PLACED)] == TagLayer)
        eType = MAPOBJ_TYPE::PLACED;
    else if (m_TagLayers[ENUM(MAPOBJ_TYPE::FLOOR)] == TagLayer)
        eType = MAPOBJ_TYPE::FLOOR;
    else if (m_TagLayers[ENUM(MAPOBJ_TYPE::TRIGGER)] == TagLayer)
        eType = MAPOBJ_TYPE::TRIGGER;
    else
        eType = MAPOBJ_TYPE::END;

    return eType;
}

CMapLoader* CMapLoader::Create(const string& TagLevel, const string& MapDataPath)
{
    CMapLoader* instance = new CMapLoader();

    if (FAILED(instance->Initialize(TagLevel, MapDataPath))) {
        Safe_Release(instance);
        instance = nullptr;
        MSG_BOX("Failed to Create : CMapLoader");
    }

    return instance;
}

void CMapLoader::Free()
{
    __super::Free();
}
