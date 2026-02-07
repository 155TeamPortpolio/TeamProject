#include "pch.h"
#include "MapToolGui.h"
#include "GameInstance.h"
#include "MapToolCore.h"
#include "GUI_Inc/imgui_stdlib.h"

#include "PlacedObject.h"
#include "Layer.h"
#include "RayReceiver.h"

#include "Helper_Func.h"
#include "Helper_MapTool.h"
#include "SlotFieldGui.h"
#include "MapToolAssistant.h"
#include "BattleObject.h"
#include "BattleSpawnerPoint.h"
#include "EntityObject.h"
#include "LightPoint.h"

CMapToolGui::CMapToolGui(GUI_CONTEXT* pContext)
    : CBasePanel(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
    , m_pMapToolCore(CMapToolCore::GetInstance())

{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pMapToolCore);
    m_pMapToolCore->RegisterGuiPanel(this);
}

HRESULT CMapToolGui::Initialize()
{
    RakeResources();

    /* For.SlotFieldGui */
    m_pSlotFieldGui = CSlotFieldGui::Create(m_pContext);
    if (nullptr == m_pSlotFieldGui)
        return E_FAIL;
    Safe_AddRef(m_pSlotFieldGui);
    CGameInstance::GetInstance()->Get_GUISystem()->Register_Panel(m_pSlotFieldGui);

    /* For.MapToolAssistant */
    m_pAssistant = CMapToolAssistant::Create(m_pContext);
    if (nullptr == m_pAssistant)
        return E_FAIL;
    Safe_AddRef(m_pAssistant);
    CGameInstance::GetInstance()->Get_GUISystem()->Register_Panel(m_pAssistant);

    m_pMapToolContext = m_pMapToolCore->Get_Context();
    
    // 저장 성공 시, 알림 쿨타임
    m_vShowDataSaveFinish = { 3.f, 0.f };

    // PhysicsRay Trigger도 검사 여부 켜기
    m_PhysicsRay.bQueryTrigger = true;

    // 시작하자마자 Collider Render
    CollisionSystem()->Set_Render(true);

    return S_OK;
}

void CMapToolGui::Update_Panel(_float dt)
{
    CheckCoolTime(dt);
    Compute_Ray();
    KeyInput();
}

void CMapToolGui::Render_GUI()
{
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(200, 50), ImGuiCond_FirstUseEver);
    ImGui::Begin("MapTool");

    ImGui::PushID(this);

    ImGui::SeparatorText("MapTool");

    float childWidth = ImGui::GetContentRegionAvail().x;
    const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
    const float OneLineHeight = textLineHeight * 2.f;
    const float childControllerHeight = (textLineHeight * 2) + (ImGui::GetStyle().WindowPadding.y * 2);

    ///////////////////////////////

    ImGui::Text("Controller");
    ImGui::BeginChild("##MapToolGuiControllerChild", ImVec2{ 0, childControllerHeight }, true);

    ImGui::Text("Last Ray Hit Pos : %.3f, %.3f, %.3f ", m_vRayHitPos.x, m_vRayHitPos.y, m_vRayHitPos.z);
    ImGui::EndChild();

    ImGui::Text("");/////////////////////////////////

    const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);

    ImGui::Text("Setting Object");
    _float fObjhectSettingChild = OneLineHeight; 
    // 크기 조절하고 entity 설치 확인
    switch (static_cast<MAPOBJ_TYPE>(m_iSelectedLayerIndex))
    {
    case MapTool::MAPOBJ_TYPE::NONE:
        fObjhectSettingChild = OneLineHeight;
        break;
    case MapTool::MAPOBJ_TYPE::PLACED:
        fObjhectSettingChild *= 6.7f;
        break;
    case MapTool::MAPOBJ_TYPE::TRIGGER:
        fObjhectSettingChild *= 3.f;
        break;
    case MapTool::MAPOBJ_TYPE::ENTITY:
        fObjhectSettingChild *= 4.f;
        break;
    case MapTool::MAPOBJ_TYPE::BATTLE:
        fObjhectSettingChild *= 4.f;
        break;
    case MapTool::MAPOBJ_TYPE::MOVEPOINT:
        fObjhectSettingChild *= 4.f;
        break;
    }

    ImGui::BeginChild("##MapToolGuiObjectSettingChild", ImVec2{ 0, fObjhectSettingChild }, true);
    
    Select_PlaceType("OBJ Type");
    Setting_SelectType();
    
	ImGui::EndChild();

    if (ImGui::TreeNode("Assistant")) {
        if (ImGui::Button("Open Assistant"))
            m_pAssistant->Set_isOpen(!m_pAssistant->IsOpen());

        ImGui::TreePop();
    }

    ImGui::Text("");/////////////////////////////////

    ImGui::Text("Data");
    if (ImGui::TreeNode("Data Save & Load")) {
        ImGui::BeginChild("##MapToolGuiDataSaveChild", ImVec2{ 0, childHeight + OneLineHeight * 1.5f }, true);

        ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Data Version");
        ImGui::InputInt("##Version", &m_pMapToolContext->iVersion);

        ImGui::SetNextItemWidth(80.0f);
        ImGui::InputText("Area Name##InputAreaName", &m_pMapToolContext->TagArea);

        if (m_pMapToolContext->TagArea.empty())
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImVec2 pmin = ImGui::GetItemRectMin();
            ImVec2 pmax = ImGui::GetItemRectMax();
            dl->AddRect(pmin, pmax, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
        }

        Select_PlaceType("Data Save Type", false);

        if (ImGui::Button("Save") && false == m_pMapToolContext->TagArea.empty()) {
            switch (static_cast<MAPOBJ_TYPE>(m_iSelectedLayerIndex))
            {
            case MapTool::MAPOBJ_TYPE::PLACED:
                Save_MapData();
                break;
            case MapTool::MAPOBJ_TYPE::ENTITY:
                Save_EntityData();
                break;
            case MapTool::MAPOBJ_TYPE::BATTLE:
                Save_BattleData();
                break;
            case MapTool::MAPOBJ_TYPE::LIGHT:
                Save_LightData();
                break;
            }
        }

        if (m_isShowDataSaveFinish) {
            ImGui::SameLine(0.f, 20.f);
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Save Json Success!");
        }


        if (ImGui::Button("Load")) {
            m_pMapToolCore->Load_MapData();
        }
        ImGui::SameLine();
        if (ImGui::Button("LoadOnce")) { 
            m_pMapToolCore->Load_WithEntityData();
        }
        ImGui::SameLine();
        if (ImGui::Button("SetEntityModel")) {
            Set_EntityModel();
        }
        ImGui::EndChild();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Slot Data")) {
        if (ImGui::Button("Open Slot Field")) {
            m_pSlotFieldGui->Set_isOpen(m_bOpenSlotField);
            m_bOpenSlotField = !m_bOpenSlotField;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Clear")) {
        ImGui::BeginChild("##MapToolClearLayerList", ImVec2{ 0, childHeight }, true);

        Render_ClearLayer();

        ImGui::EndChild();
        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::End();
}

void CMapToolGui::RakeResources()
{
    string openpath = "../Bin/Resources/Model/";

    HelperMT::EnsureDirectoryExists(openpath);

    m_EntityModelPathPackName.push_back("Default");

    auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
    for (const auto& entry : filesystem::recursive_directory_iterator(openpath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".model")
        {
            filesystem::path ModelPath = entry.path();

            ModelPathPack mpp = {};
            mpp.TagName = ModelPath.stem().string();
            mpp.TagModelPath = ModelPath.string();
            mpp.TagModelKey = ModelPath.filename().string();
            filesystem::path MaterialPath = ModelPath.replace_extension(".mat");
            mpp.TagMaterialPath = MaterialPath.string();
            mpp.TagMaterialKey = MaterialPath.filename().string();

            m_ModelPathPack.push_back(mpp);

            pRcsMgr->Add_ResourcePath(mpp.TagModelKey, mpp.TagModelPath);
            pRcsMgr->Add_ResourcePath(mpp.TagMaterialKey, mpp.TagMaterialPath);


            //Get Only EntityModelName
            if (ModelPath.string().find("Entity") != string::npos) {
                m_EntityModelPathPackName.push_back(mpp.TagName);
            }
            
        }
    }

    Load_EntityInit();
}

void CMapToolGui::CheckCoolTime(_float dt)
{
    if (m_isShowDataSaveFinish) {
        m_vShowDataSaveFinish.y += dt;
        if (m_vShowDataSaveFinish.x < m_vShowDataSaveFinish.y) {
            m_vShowDataSaveFinish.y = 0.f;
            m_isShowDataSaveFinish = false;
        }
    }
}

void CMapToolGui::Compute_Ray()
{
    POINT   pt = {};

    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);

    /*마우스 좌표 변환 */
    _vector vMouseOrigin = XMVectorSet(
        static_cast<float>(pt.x),
        static_cast<float>(pt.y),
        0.f,
        1.f
    );
    _matrix matProj = XMLoadFloat4x4(m_pGameInstance->Get_CameraMgr()->Get_ProjMatrix());
    _matrix matView = XMLoadFloat4x4(m_pGameInstance->Get_CameraMgr()->Get_ViewMatrix());

    /*마우스 레이 변환 */
    _vector raySrc = XMVector3Unproject(
        vMouseOrigin,
        0.0f, 0.0f,
        static_cast<float>(MapTool::g_iWinSizeX),
        static_cast<float>(MapTool::g_iWinSizeY),
        0.0f, 1.0f,
        matProj,
        matView,
        XMMatrixIdentity()
    );

    /*마우스 목적지 좌표*/
    _vector vMouseDest = XMVectorSet(
        static_cast<float>(pt.x),
        static_cast<float>(pt.y),
        1.f,
        1.f
    );

    _vector rayDest = XMVector3Unproject(
        vMouseDest,
        0.0f, 0.0f,
        static_cast<float>(MapTool::g_iWinSizeX),
        static_cast<float>(MapTool::g_iWinSizeY),
        0.0f, 1.0f,
        matProj,
        matView,
        XMMatrixIdentity()
    );

    _vector rayOrigin = raySrc;
    _vector rayDir = XMVector3Normalize(rayDest - raySrc);

    // SB RAY
    XMStoreFloat3(&m_PhysicsRay.vOrigin, rayOrigin);
    XMStoreFloat3(&m_PhysicsRay.vDirection, rayDir);
}

void CMapToolGui::Set_ObjectPicking(_bool is)
{
    // 사용X
    CLayer* pStaticLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::PLACED)] });
    if (nullptr == pStaticLayer)
        return;

    for (auto& pObject : pStaticLayer->Get_AllObject()) {
        pObject->Get_Component<CRayReceiver>()->Set_CompActive(is);
    }
}

void CMapToolGui::PreSet_ModelResource()
{
    ImGui::PushID("MapTool_RakeResource");
    ImGuiListClipper clipper;
    clipper.Begin((_int)m_ModelPathPack.size());
    while (clipper.Step()) {
        for (_int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            const string TagResourceName = m_ModelPathPack[i].TagName;

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_NoTreePushOnOpen |
                ImGuiTreeNodeFlags_SpanFullWidth |
                ((m_iSelectedModelIndex == i) ? ImGuiTreeNodeFlags_Selected : 0);

            ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", TagResourceName.c_str());

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                m_iSelectedModelIndex = i;
                m_TagSelectedModelName = TagResourceName;

                if (false == m_ModelPathPack[i].isLoaded) {
                    auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
                    pRcsMgr->Add_ResourcePath(m_ModelPathPack[i].TagModelKey, m_ModelPathPack[i].TagModelPath);
                    pRcsMgr->Add_ResourcePath(m_ModelPathPack[i].TagMaterialKey, m_ModelPathPack[i].TagMaterialPath);
                    m_ModelPathPack[i].isLoaded = true;
                }

            }
        }
    }
    ImGui::PopID();
}


#pragma PlaceObject
void CMapToolGui::Place_Object(PHYSICS_RAY_HIT* pRayHit)
{
    if (nullptr == pRayHit->pHitObject ||
        m_iSelectedLayerIndex >= ENUM(MAPOBJ_TYPE::END))
        return;

    IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
    
    MAPOBJ_TYPE eType = static_cast<MAPOBJ_TYPE>(m_iSelectedLayerIndex);

    // PLACED일때 모델이 선택되어있지 않으면 return
    if (MAPOBJ_TYPE::PLACED == eType &&
        -1 == m_iSelectedModelIndex)
        return;

    switch (eType)
    {
    case MAPOBJ_TYPE::PLACED:   Place_Placed(pRayHit);      break;
    case MAPOBJ_TYPE::TRIGGER:  Place_Trigger(pRayHit);     break;
    case MAPOBJ_TYPE::ENTITY:   Place_Entity(pRayHit);      break;
    case MAPOBJ_TYPE::BATTLE:   Place_Battle(pRayHit);      break;
    case MAPOBJ_TYPE::LIGHT:    Place_Light(pRayHit);       break;
    case MAPOBJ_TYPE::MOVEPOINT:Place_MovePoint(pRayHit);   break;
    case MAPOBJ_TYPE::ALL:
    case MAPOBJ_TYPE::END:
    default:
        break;
    }
}

void CMapToolGui::Place_Placed(PHYSICS_RAY_HIT* pRayHit)
{
    CPlacedObject::MAPTOOL_OBJECT_DESC* Desc = new CPlacedObject::MAPTOOL_OBJECT_DESC;
    Desc->TagModelKey = m_ModelPathPack[m_iSelectedModelIndex].TagModelKey;
    Desc->TagMaterialKey = m_ModelPathPack[m_iSelectedModelIndex].TagMaterialKey;

    COLLIDER_DESC ColDesc = {};
    ColDesc.bAutoFit = true; // 충돌 박스 생성하는 트리거
    ColDesc.strModelKey = m_ModelPathPack[m_iSelectedModelIndex].TagModelKey;

    string fileName = Helper::GetFileNameWithOutExtension(m_ModelPathPack[m_iSelectedModelIndex].TagModelKey);

    CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,"Proto_GameObject_PlacedObject" })
        .Position(pRayHit->vPoint)
        .Scale(m_vScale_PlacedObject)
        .Add_ObjDesc(Desc)
        .Collider(ColDesc)
        .Build(fileName);

    pStaticObject->Get_Component<CCollider>()->Set_DebugRender(m_pMapToolContext->isAllDebugRender);

    ObjectManager()->Add_Object(pStaticObject, {g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::PLACED)]});
}

void CMapToolGui::Place_Trigger(PHYSICS_RAY_HIT* pRayHit)
{
    COLLIDER_DESC ColDesc = {};
    ColDesc.eType = m_TriggerTransform.eType;
    ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
    ColDesc.vSize = m_TriggerTransform.vScale;
    ColDesc.vRotation = { XMConvertToRadians(m_TriggerTransform.vRotation.x),
                          XMConvertToRadians(m_TriggerTransform.vRotation.y),
                          XMConvertToRadians(m_TriggerTransform.vRotation.z) };

    string TagInstanceName = "Trigger_Object" + to_string(m_iTriggerIndex++);
    CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,"Proto_GameObject_TriggerObject" })
        .Collider(ColDesc)
        .Position(pRayHit->vPoint)
        .Build(TagInstanceName);

    pStaticObject->Get_Component<CCollider>()->Set_DebugRender(m_pMapToolContext->isAllDebugRender);

    ObjectManager()->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::TRIGGER)] });
}

void CMapToolGui::Place_Entity(PHYSICS_RAY_HIT* pRayHit)
{
    COLLIDER_DESC ColDesc = {};
    ColDesc.eType = COLLIDER_TYPE::BOX;
    ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
    ColDesc.vSize = m_vEntitySize;

    string TagInstanceName = "Entity" + to_string(m_iTriggerIndex++);
    CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,"Proto_GameObject_EntityObject" })
        .Collider(ColDesc)
        .Position(pRayHit->vPoint)
        .Build(TagInstanceName);

    pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

    ObjectManager()->Add_Object(pStaticObject, {g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::ENTITY)]});
}

void CMapToolGui::Place_Battle(PHYSICS_RAY_HIT* pRayHit)
{
    if (BATTLE_TYPE::NONE == m_eBattlyDataType)
        return;

    COLLIDER_DESC ColDesc = {};
    ColDesc.eType = COLLIDER_TYPE::BOX;
    ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
    ColDesc.vSize = m_vBattleDataSize;

    CBattleObject::BATTLE_INIT_DESC* desc = new CBattleObject::BATTLE_INIT_DESC();
    string tagProto = "";
    string tagInstanceName = "";
    switch (m_eBattlyDataType)
    {
    case MapTool::BATTLE_TYPE::PLAYER:
        tagProto = "Proto_GameObject_BattlePlayerPoint";
        desc->iIndex = m_iPlayerIndex++;
        //tagInstanceName = "PlayerPoint";
        break;
    case MapTool::BATTLE_TYPE::SPAWNER:
        tagProto = "Proto_GameObject_BattleSpawnerPoint";
        desc->iIndex = m_iSpawnerIndex++;
        //tagInstanceName = "Spawner" + to_string(m_iSpawnerIndex++); 
        break;
    case MapTool::BATTLE_TYPE::MONSTER:
        tagProto = "Proto_GameObject_BattleMonsterPoint";
        desc->iIndex = m_iMonsterIndex++;
        //tagInstanceName = "Monster" + to_string(m_iMonsterIndex++);
        break;
    case MapTool::BATTLE_TYPE::ENDPOINT:
        tagProto = "Proto_GameObject_BattleEndPoint";
        desc->iIndex = m_iEndPointIndex++;
        //tagInstanceName = "EndPoint" + to_string(m_iEndPointIndex++);
        break;
    }

    CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,tagProto })
        .Add_ObjDesc(desc)
        .Collider(ColDesc)
        .Position(pRayHit->vPoint)
        .Build(tagInstanceName);

    pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

    ObjectManager()->Add_Object(pStaticObject, {g_TagMapToolLevel, g_tagBattleObjType[ENUM(m_eBattlyDataType)]});
}

void CMapToolGui::Place_Light(PHYSICS_RAY_HIT* pRayHit)
{
    COLLIDER_DESC ColDesc = {};
    ColDesc.eType = COLLIDER_TYPE::SPHERE;
    ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거

    string TagInstanceName = "LightPoint" + to_string(m_iTriggerIndex++);
    CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,"Proto_GameObject_LightPoint" })
        .Collider(ColDesc)
        .Position(pRayHit->vPoint)
        .Build(TagInstanceName);

    pStaticObject->Get_Component<CCollider>()->Set_DebugRender(m_pMapToolContext->isAllDebugRender);

    ObjectManager()->Add_Object(pStaticObject, {g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::LIGHT)]});
}

void CMapToolGui::Place_MovePoint(PHYSICS_RAY_HIT* pRayHit)
{
    string TagInstanceName = "MovePoint_" + to_string(m_iTriggerIndex++);
    CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,"Proto_GameObject_MovePoint" })
        .Position(pRayHit->vPoint)
        .Build(TagInstanceName);

    pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

    ObjectManager()->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::MOVEPOINT)] });
}


void CMapToolGui::Save_MapData()
{
    m_MapData = {};

    m_MapData.iVersion = m_pMapToolContext->iVersion;
    m_MapData.TagArea = m_pMapToolContext->TagArea;
    m_MapData.TagDataFormat = "Base";
    //m_TagLayers{ "PlacedObject_Layer", "FloorObject_Layer", "TriggerObject_Layer", "Navigation_Layer" }
    _int    iObjIndex = {};


    for (_uint i = 0; i < ENUM(MAPOBJ_TYPE::TRIGGER); ++i) {
        CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagMapObjType[i] });
        if (nullptr == pLayer)
            continue;
        MapData_Layer DataLayer = {};
        DataLayer.TagLayer = g_tagMapObjType[i];

        for (auto& pObject : pLayer->Get_AllObject()) {
            MapData_Object DataDesc = {};
            //if (i == ENUM(MAPOBJ_TYPE::PLACED)) {
            //    static_cast<CPlacedObject*>(pObject)->Export_ObjectData(&DataDesc);
            //}
            //else if (i == ENUM(MAPOBJ_TYPE::TRIGGER)) {
            //    static_cast<CPlacedObject*>(pObject)->Export_ObjectData(&DataDesc);
            //}
            
            static_cast<CPlacedObject*>(pObject)->Export_ObjectData(&DataDesc);
            DataDesc.iObjID = iObjIndex++;
            DataLayer.Objects.push_back(DataDesc);

        }
        m_MapData.Layers.push_back(DataLayer);
    }
    int a = 1;

    string TagFileName = g_TagFileName_MapData + "." + m_pMapToolContext->TagArea + "." + m_MapData.TagDataFormat + "." + std::to_string(m_MapData.iVersion);
    string SavePath = "../Bin/Data/NewBaseData/" + HelperMT::MakeTimestampFileName(TagFileName, ".json");

    //Helper::SaveJson<MapData_Header>(m_MapData, SavePath);
    if (true == HelperMT::ExportJsonFile<MapData_Header>(m_MapData, SavePath))
        m_isShowDataSaveFinish = true;
}

void CMapToolGui::Save_EntityData()
{
    m_EntityData = {};    
    
    m_EntityData.iVersion = m_pMapToolContext->iVersion;
    m_EntityData.TagArea = m_pMapToolContext->TagArea;
    m_EntityData.TagDataFormat = "Base";

    _int    iEntityIndex = {};
    
    CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::ENTITY)]});
    if (nullptr == pLayer)
        return;
    
    for (auto& pObject : pLayer->Get_AllObject()) 
    {
        ENTITY EntityDesc = {};

        static_cast<CEntityObject*>(pObject)->Export_ObjectData(&EntityDesc);
        EntityDesc.iEntityID = iEntityIndex++;
        m_EntityData.Entities.push_back(EntityDesc);

        string InstanceName = static_cast<CEntityObject*>(pObject)->Get_InstanceName();
        string ModelTag = static_cast<CEntityObject*>(pObject)->Get_CurrentModel()->ModelTag;
        m_iniModelName.emplace(InstanceName, ModelTag);

        auto it = m_iniModelName.find(InstanceName);
        if (it == m_iniModelName.end())
            return;

        m_iniModelName[it->second] = ModelTag;
    }
    
    // 버전 없어도 될거같은데
    string TagFileName = "EntityData." + m_pMapToolContext->TagArea + "." + m_EntityData.TagDataFormat + "." + std::to_string(m_MapData.iVersion);
    string SavePath = "../Bin/Data/NewEntityData/" + HelperMT::MakeTimestampFileName(TagFileName, ".json");

    if (true == HelperMT::ExportJsonFile<Entity_Header>(m_EntityData, SavePath))
        m_isShowDataSaveFinish = true;

    Save_EntityInit();
}

void CMapToolGui::Save_BattleData()
{
    m_BattleData = {};

    m_BattleData.TagDataFormat = "BattleData";
    m_BattleData.TagArea = m_pMapToolContext->TagArea;
    //m_BattleData.iTableIndex = m_iBattleTableIndex;

    CLayer* pPlayerLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer(
        { g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::PLAYER)] });
    if (nullptr != pPlayerLayer)
    {
        for (auto& pPlayerPoint : pPlayerLayer->Get_AllObject())
        {
            BATTLE_POINT_DATA playerpointdata = {};

            static_cast<CBattleObject*>(pPlayerPoint)->Export_ObjectData(&playerpointdata);
            m_BattleData.PlayerSpawnPoint.push_back(playerpointdata);
        }
    }

    CLayer* pMonsterLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer(
        { g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::MONSTER)] });
    if (nullptr != pMonsterLayer)
    {
        for (auto& pMonsterPoint : pMonsterLayer->Get_AllObject())
        {
            BATTLE_POINT_DATA monsterpointdata = {};

            static_cast<CBattleObject*>(pMonsterPoint)->Export_ObjectData(&monsterpointdata);
            m_BattleData.Monsters.push_back(monsterpointdata);
        }
    }

    CLayer* pSpawnerLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer(
        { g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::SPAWNER)] });
    if (nullptr != pSpawnerLayer)
    {
        for (auto& pSpawnerPoint : pSpawnerLayer->Get_AllObject())
        {
            BATTLE_POINT_SPAWNER_DATA spawnerpointdata = {};

            static_cast<CBattleSpawnerPoint*>(pSpawnerPoint)->Export_ObjectData(&spawnerpointdata);
            m_BattleData.Spawners.push_back(spawnerpointdata);
        }
    }

    CLayer* pEndPointLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer(
        { g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::ENDPOINT)] });
    if (nullptr != pEndPointLayer)
    {
        for (auto& pEndPoint : pEndPointLayer->Get_AllObject())
        {
            BATTLE_POINT_DATA endpointdata = {};

            static_cast<CBattleObject*>(pEndPoint)->Export_ObjectData(&endpointdata);
            m_BattleData.EndPoints.push_back(endpointdata);
        }
    }

    string TagFileName = m_BattleData.TagDataFormat + "." + m_pMapToolContext->TagArea;// + "." + std::to_string(m_iBattleTableIndex);
    string SavePath = "../Bin/Data/NewBattleData/" + HelperMT::MakeTimestampFileName(TagFileName, ".json");

    if (true == HelperMT::ExportJsonFile<BATTLE_FIELD_DATA>(m_BattleData, SavePath))
        m_isShowDataSaveFinish = true;
}

void CMapToolGui::Save_LightData()
{
    m_LightData = {};

    m_LightData.iVersion = m_pMapToolContext->iVersion;
    m_LightData.TagArea = m_pMapToolContext->TagArea;
    m_LightData.TagDataFormat = "Base";

    _int    iLightIndex = {};

    CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::LIGHT)] });
    if (nullptr == pLayer)
        return;

    for (auto& pObject : pLayer->Get_AllObject())
    {
        MAP_LIGHT MapLightDesc = {};

        static_cast<CLightPoint*>(pObject)->Export_ObjectData(&MapLightDesc);
        MapLightDesc.iIndex = iLightIndex++;
        m_LightData.Lights.push_back(MapLightDesc);
    }

    // 버전 없어도 될거같은데
    string TagFileName = "LightData." + m_pMapToolContext->TagArea + "." + m_LightData.TagDataFormat + "." + std::to_string(m_MapData.iVersion);
    string SavePath = "../Bin/Data/NewLightData/" + HelperMT::MakeTimestampFileName(TagFileName, ".json");

    if (true == HelperMT::ExportJsonFile<Light_Header>(m_LightData, SavePath))
        m_isShowDataSaveFinish = true;
}

void CMapToolGui::Load_BattleData(const string& filepath)
{
    string Path = filepath;

    if (Path.empty()) {

        filesystem::path OpenPath = Helper::OpenFile_Dialogue();

        if (OpenPath.empty())
            return;

        if (OpenPath.extension().string() != ".json") {
            MSG_BOX("[MapTool] Load Map Data Failed.\nJson 파일이 아닙니다.");
            return;
        }

        Path = OpenPath.string();
    }

    m_pGameInstance->Get_GUISystem()->Get_Context()->pSelectedObject = { nullptr };
    
    m_BattleData = Helper::LoadJson<BATTLE_FIELD_DATA>(Path);

    m_BattleData.TagDataFormat = "BattleData";
    m_BattleData.TagArea = m_pMapToolContext->TagArea;
   
    //Load PlayerSpawnPoint
    CLayer* pPlayerLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::PLAYER)] });
    if (nullptr != pPlayerLayer)
        pPlayerLayer->Clear_Layer();

    m_iPlayerIndex = m_BattleData.PlayerSpawnPoint.size();

    if (!m_BattleData.PlayerSpawnPoint.empty()) {
        for (auto Player : m_BattleData.PlayerSpawnPoint) {
            CBattleObject::BATTLE_INIT_DESC* desc = new CBattleObject::BATTLE_INIT_DESC();
            string tagProto = "Proto_GameObject_BattlePlayerPoint";
            string tagInstanceName = "BattlePlayerPoint";

            COLLIDER_DESC ColDesc = {};
            ColDesc.eType = COLLIDER_TYPE::BOX;
            ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
            ColDesc.vSize = {
                Player.vScale[0],
                Player.vScale[1],
                Player.vScale[2],
            };

            _float3 vPos = {
                Player.vTranslation[0],
                Player.vTranslation[1],
                Player.vTranslation[2],
            };

            _float3 vRot = {
                Player.vRotation[0],
                Player.vRotation[1],
                Player.vRotation[2],
            };

            CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel , "Proto_GameObject_BattlePlayerPoint" })
                .Add_ObjDesc(desc)
                .Collider(ColDesc)
                .Position(vPos)
                .Rotate(vRot)
                .Build(tagInstanceName);

            pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

            ObjectManager()->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::PLAYER)] });
        }
    }

    //Load MonsterSpawnPoint
    CLayer* pMonsterLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::MONSTER)] });
    if (nullptr != pMonsterLayer)
        pMonsterLayer->Clear_Layer();

    m_iMonsterIndex = m_BattleData.Monsters.size();
    if (!m_BattleData.Monsters.empty()) {
        for (auto Monster : m_BattleData.Monsters) {
            CBattleObject::BATTLE_INIT_DESC* Desc = new CBattleObject::BATTLE_INIT_DESC();
            string tagProto = "Proto_GameObject_BattleMonsterPoint";
            string tagInstanceName = "BattleMonsterPoint";
            Desc->iIndex = Monster.iIndex;

            COLLIDER_DESC ColDesc = {};
            ColDesc.eType = COLLIDER_TYPE::BOX;
            ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
            ColDesc.vSize = {
                Monster.vScale[0],
                Monster.vScale[1],
                Monster.vScale[2],
            };

            _float3 vPos = {
                Monster.vTranslation[0],
                Monster.vTranslation[1],
                Monster.vTranslation[2],
            };

            _float3 vRot = {
                Monster.vRotation[0],
                Monster.vRotation[1],
                Monster.vRotation[2],
            };

            CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel , "Proto_GameObject_BattleMonsterPoint" })
                .Add_ObjDesc(Desc)
                .Collider(ColDesc)
                .Position(vPos)
                .Rotate(vRot)
                .Build(tagInstanceName);

            pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

            ObjectManager()->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::MONSTER)] });
        }
    }

    //Load SpawnerSpawnPoint
    CLayer* pSpawnerLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::SPAWNER)] });
    if (nullptr != pSpawnerLayer)
        pSpawnerLayer->Clear_Layer();

    m_iSpawnerIndex = m_BattleData.Spawners.size();
    if (!m_BattleData.Spawners.empty()) {
        for (auto Spawner : m_BattleData.Spawners) {
            CBattleObject::BATTLE_INIT_DESC* Desc = new CBattleObject::BATTLE_INIT_DESC();
            string tagProto = "Proto_GameObject_BattleSpawnerPoint";
            string tagInstanceName = "BattleSpawnerPoint";
            Desc->iIndex = Spawner.iIndex;

            COLLIDER_DESC ColDesc = {};
            ColDesc.eType = COLLIDER_TYPE::BOX;
            ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
            ColDesc.vSize = {
                Spawner.vScale[0],
                Spawner.vScale[1],
                Spawner.vScale[2],
            };

            _float3 vPos = {
                Spawner.vTranslation[0],
                Spawner.vTranslation[1],
                Spawner.vTranslation[2],
            };

            _float3 vRot = {
                Spawner.vRotation[0],
                Spawner.vRotation[1],
                Spawner.vRotation[2],
            };

            CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel , "Proto_GameObject_BattleSpawnerPoint" })
                .Add_ObjDesc(Desc)
                .Collider(ColDesc)
                .Position(vPos)
                .Rotate(vRot)
                .Build(tagInstanceName);

            pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

            ObjectManager()->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::SPAWNER)] });
        }
    }

    //Load EndPortalPoint
    CLayer* pEndPointLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::ENDPOINT)] });
    if (nullptr != pEndPointLayer)
        pEndPointLayer->Clear_Layer();

    m_iEndPointIndex = m_BattleData.EndPoints.size();
    if (!m_BattleData.EndPoints.empty()) {
        for (auto EndPoint : m_BattleData.EndPoints) {
            CBattleObject::BATTLE_INIT_DESC* Desc = new CBattleObject::BATTLE_INIT_DESC();
            string tagProto = "Proto_GameObject_BattleEndPoint";
            string tagInstanceName = "BattleEndPoint";
            Desc->iIndex = EndPoint.iIndex;

            COLLIDER_DESC ColDesc = {};
            ColDesc.eType = COLLIDER_TYPE::BOX;
            ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
            ColDesc.vSize = {
                EndPoint.vScale[0],
                EndPoint.vScale[1],
                EndPoint.vScale[2],
            };

            _float3 vPos = {
                EndPoint.vTranslation[0],
                EndPoint.vTranslation[1],
                EndPoint.vTranslation[2],
            };

            _float3 vRot = {
                EndPoint.vRotation[0],
                EndPoint.vRotation[1],
                EndPoint.vRotation[2],
            };

            CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel , "Proto_GameObject_BattleEndPoint" })
                .Add_ObjDesc(Desc)
                .Collider(ColDesc)
                .Position(vPos)
                .Rotate(vRot)
                .Build(tagInstanceName);

            pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

            ObjectManager()->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagBattleObjType[ENUM(BATTLE_TYPE::ENDPOINT)] });
        }
    }
}

void CMapToolGui::Select_PlaceType(const string& tagLabel, _bool isShowDetail)
{
    if (m_iSelectedLayerIndex < 0) 
        m_iSelectedLayerIndex = 0;

    if (m_iSelectedLayerIndex >= (_int)IM_ARRAYSIZE(g_tagMapObjType)) 
        m_iSelectedLayerIndex = (_int)IM_ARRAYSIZE(g_tagMapObjType) - 1;

    const _int iPrevIndex = m_iSelectedLayerIndex;
    const _char* preview = g_tagMapObjType[m_iSelectedLayerIndex];

    string Label = tagLabel + "##SelectType" + tagLabel;
    if (ImGui::BeginCombo(Label.c_str(), preview))
    {
        for (_int i = 0; i < (_int)IM_ARRAYSIZE(g_tagMapObjType); ++i)
        {
            const _bool isSelected = (i == m_iSelectedLayerIndex);
            if (ImGui::Selectable(g_tagMapObjType[i], isSelected))
                m_iSelectedLayerIndex = i;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (true == isShowDetail)
    {
        switch (static_cast<MAPOBJ_TYPE>(m_iSelectedLayerIndex))
        {
        case MapTool::MAPOBJ_TYPE::PLACED:
            ImGui::Text("Selected Model Name : %s", m_TagSelectedModelName.c_str());
            break;
        case MapTool::MAPOBJ_TYPE::TRIGGER:
            Select_TriggerType();
            break;
        case MapTool::MAPOBJ_TYPE::ENTITY:
            break;
        case MapTool::MAPOBJ_TYPE::BATTLE:
            Select_BattleDataType();
            break;
        }
    }
}

void CMapToolGui::Select_TriggerType()
{
    const _char* items[] = { "Box", "Sphere", "Capsule" };

    const _char* preview = items[ENUM(m_TriggerTransform.eType)];

    if (ImGui::BeginCombo("Trigger Shape Type", preview))
    {
        for (_int i = 0; i < (_int)IM_ARRAYSIZE(items); ++i)
        {
            const _bool isSelected = (i == ENUM(m_TriggerTransform.eType));
            if (ImGui::Selectable(items[i], isSelected)) {
                m_TriggerTransform.eType = static_cast<COLLIDER_TYPE>(i);
                m_TriggerTransform.vScale = { 1.f,1.f,1.f };
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void CMapToolGui::Select_BattleDataType()
{
    const _char* items[] = { "None", "Player", "Spawner", "Monster", "EndPoint" };

    const _char* preview = items[ENUM(m_eBattlyDataType)];

    if (ImGui::BeginCombo("BattleData Type", preview))
    {
        for (_int i = 0; i < (_int)IM_ARRAYSIZE(items); ++i)
        {
            const _bool isSelected = (i == ENUM(m_eBattlyDataType));
            if (ImGui::Selectable(items[i], isSelected)) {
                m_eBattlyDataType = static_cast<BATTLE_TYPE>(i);
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void CMapToolGui::Setting_SelectType()
{
    if (ENUM(MAPOBJ_TYPE::ALL) < m_iSelectedLayerIndex)
        return;

    switch (static_cast<MAPOBJ_TYPE>(m_iSelectedLayerIndex))
    {
    case MapTool::MAPOBJ_TYPE::PLACED:
    {
        ImGui::BeginChild("##MapToolRakeResourceList", ImVec2{ 0, 200.f }, true);
        PreSet_ModelResource();

        ImGui::EndChild();
        break;
    }
    case MapTool::MAPOBJ_TYPE::TRIGGER:
    {
        switch (m_TriggerTransform.eType)
        {
        case COLLIDER_TYPE::BOX:
        {
            ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Box Scale ( HalfExtents(x,y,z) )");
            ImGui::InputFloat3("##BoxScale", reinterpret_cast<float*>(&m_TriggerTransform.vScale), "%.1f");
            break;
        }
        case COLLIDER_TYPE::SPHERE:
        {
            _float fRadius = m_TriggerTransform.vScale.x;
            ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Sphere Radius ( Radius(x) )");
            if (ImGui::InputFloat("##SphereScale", reinterpret_cast<float*>(&fRadius), 1.f))
                m_TriggerTransform.vScale = { fRadius,1.f,1.f };
            break;
        }
        case COLLIDER_TYPE::CAPSULE:
        {
            _float2 vCapsuleScale = { m_TriggerTransform.vScale.x, m_TriggerTransform.vScale.y };
            ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Capsule Scale ( Radius(x)/HalfHeight(y) )");
            if (ImGui::InputFloat2("##CapsuleScale", reinterpret_cast<float*>(&vCapsuleScale), "%.1f")) {
                m_TriggerTransform.vScale = { vCapsuleScale.x, vCapsuleScale.y, 1.f };
            }

            break;
        }
        }
        break;
    }
    case MapTool::MAPOBJ_TYPE::ENTITY:
    {
        ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Entity Box Scale ( HalfExtents(x,y,z) )");
        ImGui::InputFloat3("##EntityBoxScale", reinterpret_cast<float*>(&m_vEntitySize), "%.1f");
        
        auto pGuiContext = m_pGameInstance->Get_GUISystem()->Get_Context();

        static string CurModelName = {};
      
        if (pGuiContext->pSelectedObject)
        {
            if (m_pSelectedEntityObject != dynamic_cast<CEntityObject*>(pGuiContext->pSelectedObject))
            { 
                m_pSelectedEntityObject = dynamic_cast<CEntityObject*>(pGuiContext->pSelectedObject);
            }
        }

        if (ImGui::BeginCombo("EntityModel", CurModelName.c_str()))
        {
            for (int i = 0; i < m_EntityModelPathPackName.size(); ++i)
            {
                if (ImGui::Selectable(m_EntityModelPathPackName[i].c_str()))
                {
                    if (nullptr != m_pSelectedEntityObject)
                    {
                        for (auto Pack : m_ModelPathPack)
                        {
                            if (Pack.TagName == m_EntityModelPathPackName[i])
                                m_pSelectedEntityObject->Set_EntityModel(Pack.TagName, Pack.TagModelKey, Pack.TagMaterialKey);
                        }
                    }

                    CurModelName = m_EntityModelPathPackName[i];
                    m_iPickedEntityModelIndex = i;
                }
            }
            ImGui::EndCombo();
        }

        break;
    }
    case MapTool::MAPOBJ_TYPE::BATTLE:
        ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "BattleData Box Scale ( HalfExtents(x,y,z) )");
        ImGui::InputFloat3("##BattleBoxScale", reinterpret_cast<float*>(&m_vBattleDataSize), "%.1f");
        switch (m_eBattlyDataType)
        {
        case MapTool::BATTLE_TYPE::SPAWNER:
            ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "SpawnerIndex");
            ImGui::InputInt("##SpawnerIndex", &m_iSpawnerIndex);
            break;
        case MapTool::BATTLE_TYPE::MONSTER:
            ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "MonsterIndex");
            ImGui::InputInt("##MonsterIndex", &m_iMonsterIndex);
            break;
        default:
            break;
        }
    break;
    case MapTool::MAPOBJ_TYPE::MOVEPOINT:

        break;
    default:
        break;
    }
}

void CMapToolGui::Set_EntityModel()
{
    CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::ENTITY)] });
    if (!pLayer) return;

    for (auto& pObjects : pLayer->Get_AllObject())
    {
        CEntityObject* pEntity = dynamic_cast<CEntityObject*>(pObjects);

        auto iter = m_iniModelName.find(pEntity->Get_InstanceName());
        if (iter == m_iniModelName.end() || iter->second.empty() || iter->second == "None")
            continue;


        pEntity->Set_EntityModel(pEntity->Get_InstanceName(), iter->second + ".model", iter->second + ".mat");
    }
}

void CMapToolGui::Save_EntityInit()
{
    Helper::SaveJson<unordered_map<string, string>>(m_iniModelName, "../Bin/Resources/Model/Entity/ModelIni.json");
}

void CMapToolGui::Load_EntityInit()
{
    auto LoadData = Helper::LoadJson<unordered_map<string, string>>("../Bin/Resources/Model/Entity/ModelIni.json");

    for (auto Data : LoadData)
        m_iniModelName.emplace(Data.first, Data.second);
}

void CMapToolGui::Render_ClearLayer()
{
    auto pTagLayers = &g_tagMapObjType;

    ImGui::PushID("MapTool_LayerDelete");
    ImGuiListClipper clipper;
    clipper.Begin(ENUM(MAPOBJ_TYPE::END));
    while (clipper.Step()) {
        for (_int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            const string TagClearLayer = (*pTagLayers)[i];

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_NoTreePushOnOpen |
                ImGuiTreeNodeFlags_SpanFullWidth |
                ((m_iSelectedModelIndex == i) ? ImGuiTreeNodeFlags_Selected : 0);

            ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", TagClearLayer.c_str());

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                m_pMapToolCore->Clear_Layer(static_cast<MAPOBJ_TYPE>(i));

                if (i == ENUM(MAPOBJ_TYPE::TRIGGER) || i == ENUM(MAPOBJ_TYPE::ALL))
                    m_iTriggerIndex = {};
                else {
                    m_iEndPointIndex = 0;
                    m_iMonsterIndex = 0;
                    m_iSpawnerIndex = 0;
                    m_iPlayerIndex = 0;
                }                
            }
        }
    }
    ImGui::PopID();
}

void CMapToolGui::Clear_BattleData()
{
    m_iPlayerIndex = 0;
    m_iMonsterIndex = 0;
    m_iSpawnerIndex = 0;
    m_iEndPointIndex = 0;
}

void CMapToolGui::KeyInput()
{
    if (true == GUISystem()->UsingUI())
        return;

    auto pInputDev = m_pGameInstance->Get_InputDev();
    
    ImGuiIO& io = ImGui::GetIO();

    // 오브젝트 피킹
    if (pInputDev->Mouse_Tap(MOUSE_BTN::LB) && false == io.WantCaptureMouse) {
        PHYSICS_RAY_HIT HitDesc = {};
        if (true == m_pGameInstance->Get_PhysicsSystem()->Raycast(m_PhysicsRay, HitDesc)) {
            string HitObjLayerTag = HitDesc.pHitObject->Get_Layer()->Get_LayerTag();
            if (g_tagMapObjType[ENUM(MAPOBJ_TYPE::PLACED)] == HitObjLayerTag ||
                g_tagMapObjType[ENUM(MAPOBJ_TYPE::TRIGGER)] == HitObjLayerTag ||
                g_tagMapObjType[ENUM(MAPOBJ_TYPE::ENTITY)] == HitObjLayerTag ||
                g_tagBattleObjType[ENUM(BATTLE_TYPE::PLAYER)] == HitObjLayerTag ||
                g_tagBattleObjType[ENUM(BATTLE_TYPE::MONSTER)] == HitObjLayerTag ||
                g_tagBattleObjType[ENUM(BATTLE_TYPE::SPAWNER)] == HitObjLayerTag ||
                g_tagBattleObjType[ENUM(BATTLE_TYPE::ENDPOINT)] == HitObjLayerTag
                )
                CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = HitDesc.pHitObject  ;
        }

    }

    // Inspector 창에 떠있는 오브젝트 삭제
    if (pInputDev->Key_Tap(VK_DELETE)) {
        auto pGuiContext = m_pGameInstance->Get_GUISystem()->Get_Context();

        if (nullptr != pGuiContext->pSelectedObject &&
            nullptr != dynamic_cast<CMapToolObject*>(pGuiContext->pSelectedObject)) {

            static_cast<CMapToolObject*>(pGuiContext->pSelectedObject)->Delete_Object();

        }
    }

    // 레이피킹으로 오브젝트 배치
    if (m_pGameInstance->Get_InputDev()->Key_Tap('P')) {
        PHYSICS_RAY_HIT HitDesc = {};
        if (true == m_pGameInstance->Get_PhysicsSystem()->Raycast(m_PhysicsRay, HitDesc)) {
            Place_Object(&HitDesc);
        }
    }
}

CMapToolGui* CMapToolGui::Create(GUI_CONTEXT* pContext)
{
    CMapToolGui* pInstance = new CMapToolGui(pContext);
    if (FAILED(pInstance->Initialize())) {
        MSG_BOX("MapToolGui Create Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapToolGui::Free()
{
    __super::Free();

    Safe_Release(m_pAssistant);
    Safe_Release(m_pMapToolCore);
    Safe_Release(m_pSlotFieldGui);
    Safe_Release(m_pGameInstance);
}
