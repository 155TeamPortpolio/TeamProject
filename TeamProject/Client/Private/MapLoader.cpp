#include "pch.h"
#include "MapLoader.h"
#include "GameInstance.h"

#include "MapData_Defines.h"
#include "Helper_Func.h"
#include "MapLoader_Helper.h"

#include "MapDataCloud.h"
#include "MapPlacedObject.h"

CMapLoader::CMapLoader()
    : m_TagLayers{ "PlacedObject_Layer", "TriggerObject_Layer" }
{
}

HRESULT CMapLoader::Initialize(const string& TagLevel, CMapDataCloud* pMapDataCloud, const string& TagArea)
{
    if (nullptr == pMapDataCloud)
        return E_FAIL;

    m_TagLevel = TagLevel;

    auto pPackets = pMapDataCloud->Get_MapDataPacket(TagArea);

    _bool isFindBaseData = { false };
    for (auto& packet : *pPackets) {
        if ("Base" == packet.TagSlotFormat) {
            LoadBaseData(&packet);
            isFindBaseData = true;
        }
        else
            CacheSlotDataFile(packet.TagDataFilePath);
    }

    // 맵 베이스 데이터 없으면 로드 불가!
    if (false == isFindBaseData)
        return E_FAIL;

    auto iter = m_SlotFormatData.find("Collider");
    if (iter != m_SlotFormatData.end())
        m_hasColliderData = true;

    


    for (auto& layerdata : m_MapBaseData.Layers) {
        // 레이어 태그 무결성 검사
        MAPOBJ_TYPE eType = Check_LayerTag(layerdata.TagLayer);
        if (MAPOBJ_TYPE::END == eType)
            continue;

        for (auto& objectdata : layerdata.Objects) {
            switch (eType)
            {
            case Client::CMapLoader::MAPOBJ_TYPE::PLACED:
                Place_PlacedObjectFromLoadData(&objectdata);
                break;
            case Client::CMapLoader::MAPOBJ_TYPE::TRIGGER:
                Place_TriggerObjectFromLoadData(&objectdata);
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

    COLLIDER_DESC ColliderDesc = {};

    for (auto& tSlotData : m_SlotFormatData) {
        if (tSlotData.first == "Collider" && true == m_hasColliderData) {
            // physics 데이터 넣는 부분. 개선의 여지가 있음
            for (auto& physicsData : tSlotData.second[pData->iObjID]) {

                if (physicsData.TagName == "bCooking" && physicsData.defaultvalue.type == SLOT_DATA_TYPE::Bool) {
                    auto bCooking = GetSlotValue<_bool>(physicsData.defaultvalue);
                    Desc->bCooking = *bCooking;
                    ColliderDesc.bCooking = *bCooking;
                    ColliderDesc.strModelKey = Desc->TagModelKey;
                }
            }
        }
        
        // 일단 데이터 다 때려넣기
        for (auto& FieldData : tSlotData.second[pData->iObjID]) 
            Desc->SlotDataValues[tSlotData.first].push_back(FieldData);
    }
  

    _float3 vScl{}, vRot{}, vTrans{};
    _float4 vRotQ{};
   
    // collider에 위치정보 구우려면 빌더단계에서ㄱㄱ
    _float4x4 matWorld = {
        pData->vRight[0], pData->vRight[1] , pData->vRight[2] , pData->vRight[3],
        pData->vUp[0],  pData->vUp[1] , pData->vUp[2] , pData->vUp[3],
        pData->vLook[0], pData->vLook[1] , pData->vLook[2] , pData->vLook[3],
        pData->vPos[0], pData->vPos[1] , pData->vPos[2] , pData->vPos[3] };

    if (true == ExtractSRT(XMLoadFloat4x4(&matWorld), vScl, vRotQ, vTrans))
        vRot = QuaternionToEuler(vRotQ);


    CGameObject* pStaticObject = Builder::Create_Object({ m_TagLevel ,"Proto_GameObject_MapPlacedObject" })
        .Add_ObjDesc(Desc)
        .Scale(vScl)
        .Rotate(vRot)
        .Position(vTrans)
        .Collider(ColliderDesc)
        .Build(Helper::GetFileNameWithOutExtension(pData->TagModelResourceKey));

    if (nullptr == pStaticObject)
        return;

    
    //pStaticObject->Get_Component<CTransform>()->TranslateMatrix(XMLoadFloat4x4(&matWorld));

    pObjMgr->Add_Object(pStaticObject, { m_TagLevel, m_TagLayers[ENUM(MAPOBJ_TYPE::PLACED)]});
}

void CMapLoader::Place_TriggerObjectFromLoadData(MapData_Object* pData)
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

    CGameObject* pStaticObject = Builder::Create_Object({ m_TagLevel ,"Proto_GameObject_MapTriggerObject" })
        .Collider(ColDesc)
        .Position({ pData->vPos[0], pData->vPos[1], pData->vPos[2] })
        .Build(pData->TagModelResourceKey);

    pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

    IObjectService* pObjMgr = CGameInstance::GetInstance()->Get_ObjectMgr();
    pObjMgr->Add_Object(pStaticObject, { m_TagLevel, m_TagLayers[ENUM(MAPOBJ_TYPE::TRIGGER)] });
}

CMapLoader::MAPOBJ_TYPE CMapLoader::Check_LayerTag(const string& TagLayer)
{
    MAPOBJ_TYPE eType = {};
    if (m_TagLayers[ENUM(MAPOBJ_TYPE::PLACED)] == TagLayer)
        eType = MAPOBJ_TYPE::PLACED;
    else if (m_TagLayers[ENUM(MAPOBJ_TYPE::TRIGGER)] == TagLayer)
        eType = MAPOBJ_TYPE::TRIGGER;
    else
        eType = MAPOBJ_TYPE::END;

    return eType;
}

HRESULT CMapLoader::LoadBaseData(const MapData_Path_Packet* pPacket)
{
    filesystem::path OpenPath = pPacket->TagDataFilePath;

    if (OpenPath.empty())
        return E_FAIL;

    if (OpenPath.extension().string() != ".json") {
        MSG_BOX("[MapTool] Load Map Data Failed.\nJson 파일이 아닙니다.");
        return E_FAIL;
    }

    m_MapBaseData = Helper::LoadJson<MapData_Header>(OpenPath.string());
    if (-1 == m_MapBaseData.iVersion)
        return E_FAIL;

    if (m_MapBaseData.iVersion != g_iMapDataVersion) {
        MSG_BOX("[MapTool] Load Map Data Failed.\n잘못된 버전입니다.");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CMapLoader::CacheSlotDataFile(const string& SlotDataFilePath)
{
    ifstream ifs(SlotDataFilePath);
    if (false == ifs.is_open())
        return E_FAIL;

    json jRoot = json::parse(ifs, nullptr, false);
    if (jRoot.is_discarded())
        return E_FAIL;

    if (false == jRoot.is_object())
        return E_FAIL;

    string TagSlotFormat = {};
    if (false == TryReadString(jRoot, "TagDataFormat", TagSlotFormat) || TagSlotFormat.empty())
        return E_FAIL;

    const json* values = FindPtr(jRoot, "values");
    if (nullptr == values || false == values->is_array())
        return S_OK; // 빈 값으로 채우고 나가기 (터짐 방지)

    ObjFieldMap& perObj = m_SlotFormatData[TagSlotFormat];

    for (const auto& elem : *values) {
        FIELD_DATA FieldData = {};
        if (false == TryParseFieldData(elem, FieldData))
            continue; // row 깨지면 무시하고 넘김

        perObj[FieldData.iObjID].push_back(move(FieldData));
    }

    return S_OK;
}

_bool CMapLoader::isThereFormat(const string& TagSlotFormat)
{
    auto iter = m_SlotFormatData.find(TagSlotFormat);
    if (iter == m_SlotFormatData.end())
        return false;
    return true;
}

CMapLoader* CMapLoader::Create(const string& TagLevel, CMapDataCloud* pMapDataCloud, const string& TagArea)
{
    CMapLoader* instance = new CMapLoader();

    if (FAILED(instance->Initialize(TagLevel, pMapDataCloud, TagArea))) {
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
