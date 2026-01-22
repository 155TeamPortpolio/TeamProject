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

CMapToolGui::CMapToolGui(GUI_CONTEXT* pContext)
    : CBasePanel(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
    , m_pMapToolCore(CMapToolCore::GetInstance())

{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pMapToolCore);
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
    m_vShowMapDataSaveFinish = { 3.f, 0.f };
    m_vShowEntityDataSaveFinish = { 3.f, 0.f };

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

    //if (ImGui::Checkbox("IsDebugRender", &m_pMapToolContext->isAllDebugRender))
    //    m_pMapToolCore->Set_AllObjectDebugRender(m_pMapToolContext->isAllDebugRender);

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
        fObjhectSettingChild *= 2.5f;
        break;
    case MapTool::MAPOBJ_TYPE::BATTLE:
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

        Select_PlaceType("Data Type");

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
                break;
            }
            
        }

        if (m_isShowMapDataSaveFinish) {
            ImGui::SameLine(0.f, 20.f);
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Save Json Success!");
        }


        if (ImGui::Button("Load")) {
            m_pMapToolCore->Load_MapData();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Entity")) {
            m_pMapToolCore->Load_EntityData();
        }

        ImGui::EndChild();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Slot Data")) {
        if (ImGui::Button("Open Slot Field")) {
            m_pSlotFieldGui->Set_isOpen(true);
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
        }
    }
}

void CMapToolGui::CheckCoolTime(_float dt)
{
    if (m_isShowMapDataSaveFinish) {
        m_vShowMapDataSaveFinish.y += dt;
        if (m_vShowMapDataSaveFinish.x < m_vShowMapDataSaveFinish.y) {
            m_vShowMapDataSaveFinish.y = 0.f;
            m_isShowMapDataSaveFinish = false;
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
    case MAPOBJ_TYPE::PLACED:
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

        pObjMgr->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::PLACED)] });
        break;
    }
    case MAPOBJ_TYPE::TRIGGER:
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

        pObjMgr->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::TRIGGER)] });
        break;
    }
    case MAPOBJ_TYPE::ENTITY:
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

        pObjMgr->Add_Object(pStaticObject, { g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::ENTITY)] });

        break;
    }
    case MAPOBJ_TYPE::BATTLE:
    {
        Place_BattleData(pRayHit);
        break;
    }
    case MAPOBJ_TYPE::ALL:
        break;
    case MAPOBJ_TYPE::END:
        break;
    default:
        break;
    }

   
}

void CMapToolGui::Place_BattleData(PHYSICS_RAY_HIT* pRayHit)
{
    COLLIDER_DESC ColDesc = {};
    ColDesc.eType = COLLIDER_TYPE::BOX;
    ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
    ColDesc.vSize = m_vBattleDataSize;

    string tagProto = "";
    string tagInstanceName = "";
    switch (m_eBattlyDataType)
    {
    case MapTool::BATTLE_TYPE::PLAYER:
        tagProto = "Proto_GameObject_BattlePlayerPoint";
        tagInstanceName = "PlayerPoint";
        break;
    case MapTool::BATTLE_TYPE::SPAWNER:
        tagProto = "Proto_GameObject_BattleSpawnerPoint";
        tagInstanceName = "Spawner" + to_string(m_iSpawnerIndex++); 
        break;
    case MapTool::BATTLE_TYPE::MONSTER:
        tagProto = "Proto_GameObject_BattleMonsterPoint";
        tagInstanceName = "Monster" + to_string(m_iMonsterIndex++);
        break;
    case MapTool::BATTLE_TYPE::ENDPOINT:
        break;
    }

    CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,tagProto })
        .Collider(ColDesc)
        .Position(pRayHit->vPoint)
        .Build(tagInstanceName);

    pStaticObject->Get_Component<CCollider>()->Set_DebugRender(true);

    ObjectManager()->Add_Object(pStaticObject, {g_TagMapToolLevel, g_tagMapObjType[ENUM(MAPOBJ_TYPE::BATTLE)]});

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

void CMapToolGui::Save_MapData()
{
    m_MapData = {};

    m_MapData.iVersion = m_pMapToolContext->iVersion;
    m_MapData.TagArea = m_pMapToolContext->TagArea;
    m_MapData.TagDataFormat = "Base";
    //m_TagLayers{ "PlacedObject_Layer", "FloorObject_Layer", "TriggerObject_Layer", "Navigation_Layer" }
    _int    iObjIndex = {};


    for (_uint i = 0; i < ENUM(MAPOBJ_TYPE::END); ++i) {
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
        m_isShowMapDataSaveFinish = true;
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

        static_cast<CPlacedObject*>(pObject)->Export_ObjectData(&EntityDesc);
        EntityDesc.iEntityID = iEntityIndex++;
        m_EntityData.Entities.push_back(EntityDesc);
    }
    
    // 버전 없어도 될거같은데
    string TagFileName = "EntityData." + m_pMapToolContext->TagArea + "." + m_EntityData.TagDataFormat + "." + std::to_string(m_MapData.iVersion);
    string SavePath = "../Bin/Data/NewEntityData/" + HelperMT::MakeTimestampFileName(TagFileName, ".json");

    if (true == HelperMT::ExportJsonFile<Entity_Header>(m_EntityData, SavePath))
        m_isShowEntityDataSaveFinish = true;
}

void CMapToolGui::Save_BattleData()
{
}

void CMapToolGui::Select_PlaceType(const string& tagLabel)
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
        }
        
        break;
    }
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

            }
        }
    }
    ImGui::PopID();
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
                g_tagMapObjType[ENUM(MAPOBJ_TYPE::ENTITY)] == HitObjLayerTag
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
