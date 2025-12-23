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

    m_pSlotFieldGui = CSlotFieldGui::Create(m_pContext);
    if (nullptr == m_pSlotFieldGui)
        return E_FAIL;
    Safe_AddRef(m_pSlotFieldGui);
    CGameInstance::GetInstance()->Get_GUISystem()->Register_Panel(m_pSlotFieldGui);

    m_pMapToolContext = m_pMapToolCore->Get_Context();
    
    // 저장 성공 시, 알림 쿨타임
    m_vShowSaveFinish = { 3.f, 0.f };

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
    ImGui::PushID(this);

    ImGui::SeparatorText("MapTool");

    float childWidth = ImGui::GetContentRegionAvail().x;
    const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
    const float childControllerHeight = (textLineHeight * 2) + (ImGui::GetStyle().WindowPadding.y * 2);

    ///////////////////////////////

    ImGui::Text("Controller");
    ImGui::BeginChild("##MapToolGuiControllerChild", ImVec2{ 0, childControllerHeight }, true);

#ifdef _DEBUG
    if (ImGui::Checkbox("IsDebugRender", &m_pMapToolContext->isAllDebugRender))
        m_pMapToolCore->Set_AllObjectDebugRender(m_pMapToolContext->isAllDebugRender);
#endif // _DEBUG

    ImGui::Text("Last Ray Hit Pos : %.3f, %.3f, %.3f ", m_vRayHitPos.x, m_vRayHitPos.y, m_vRayHitPos.z);
    ImGui::EndChild();

    ImGui::Text("");/////////////////////////////////

    const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);
    ImGui::Text("Setting PLACED Object");
    ImGui::BeginChild("##MapToolGuiObjectSettingChild", ImVec2{ 0, childHeight }, true);
    string TagSelectedModelName = "Selected Model Name : " + m_TagSelectedModelName;
    ImGui::Text(TagSelectedModelName.c_str());

    //ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Scale");
    ImGui::InputFloat3(" Scale##Scale", reinterpret_cast<float*>(&m_vScale_PlacedObject), "%.1f");

    //ImGui::Text("");
    if (ImGui::Checkbox("IsObjectPicking", &m_isObjectPicking)) {
        Set_ObjectPicking(m_isObjectPicking);
    }
	ImGui::EndChild();
    if (ImGui::TreeNode("Model Setting")) {
        ImGui::BeginChild("##MapToolRakeResourceList", ImVec2{ 0, childHeight }, true);

        PreSet_ModelResource();

        ImGui::EndChild();
        ImGui::TreePop();
    }


    ImGui::Text("");/////////////////////////////////

    ImGui::Text("Data");
    if (ImGui::TreeNode("Data Save & Load")) {
        ImGui::BeginChild("##MapToolGuiDataSaveChild", ImVec2{ 0, childHeight }, true);

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

        if (ImGui::Button("Save") && false == m_pMapToolContext->TagArea.empty()) {
            Save_MapData();
        }

        if (m_isShowSaveFinish) {
            ImGui::SameLine(0.f, 20.f);
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Save Json Success!");
        }


        if (ImGui::Button("Load")) {
            m_pMapToolCore->Load_MapData();
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
}

void CMapToolGui::RakeResources()
{
    auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
    for (const auto& entry : filesystem::recursive_directory_iterator("../Bin/Resources/Model/"))
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
    if (m_isShowSaveFinish) {
        m_vShowSaveFinish.y += dt;
        if (m_vShowSaveFinish.x < m_vShowSaveFinish.y) {
            m_vShowSaveFinish.y = 0.f;
            m_isShowSaveFinish = false;
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
        -1 == m_iSelectedIndex)
        return;

    IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();

    CPlacedObject::MAPTOOL_OBJECT_DESC* Desc = new CPlacedObject::MAPTOOL_OBJECT_DESC;
    Desc->TagModelKey = m_ModelPathPack[m_iSelectedIndex].TagModelKey;
    Desc->TagMaterialKey = m_ModelPathPack[m_iSelectedIndex].TagMaterialKey;

    COLLIDER_DESC ColDesc = {};
    ColDesc.bAutoFit = true; // 충돌 박스 생성하는 트리거
    ColDesc.strModelKey = m_ModelPathPack[m_iSelectedIndex].TagModelKey;

    CGameObject* pStaticObject = Builder::Create_Object({ g_TagMapToolLevel ,"Proto_GameObject_PlacedObject" })
        .Position(pRayHit->vPoint)
        .Scale(m_vScale_PlacedObject)
        .Add_ObjDesc(Desc)
        .Collider(ColDesc)
        .Build("Static_Model");

#ifdef _DEBUG
    pStaticObject->Get_Component<CCollider>()->Set_DebugRender(m_pMapToolContext->isAllDebugRender);
#endif // _DEBUG
    
    pObjMgr->Add_Object(pStaticObject, { g_TagMapToolLevel, m_pMapToolContext->TagLayers[ENUM(MAPOBJ_TYPE::PLACED)]});
}

void CMapToolGui::Set_ObjectPicking(_bool is)
{
    // 사용X
    CLayer* pStaticLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, m_pMapToolContext->TagLayers[ENUM(MAPOBJ_TYPE::PLACED)] });
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
                ((m_iSelectedIndex == i) ? ImGuiTreeNodeFlags_Selected : 0);

            ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", TagResourceName.c_str());

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                m_iSelectedIndex = i;
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
    

    m_Data.iVersion = m_pMapToolContext->iVersion;
    m_Data.TagArea = m_pMapToolContext->TagArea;
    m_Data.TagDataFormat = "Base";
    //m_TagLayers{ "PlacedObject_Layer", "FloorObject_Layer", "TriggerObject_Layer", "Navigation_Layer" }
    _int    iObjIndex = {};


    for (_uint i = 0; i < (_uint)m_pMapToolContext->TagLayers.size(); ++i) {
        CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ g_TagMapToolLevel, m_pMapToolContext->TagLayers[i] });
        if (nullptr == pLayer)
            continue;
        MapData_Layer DataLayer = {};
        DataLayer.TagLayer = m_pMapToolContext->TagLayers[i];

        for (auto& pObject : pLayer->Get_AllObject()) {
            if (i == ENUM(MAPOBJ_TYPE::PLACED)) {
                MapData_Object DataDesc = {};
                static_cast<CPlacedObject*>(pObject)->Export_ObjectData(&DataDesc);
                DataDesc.iObjID = iObjIndex++;
                DataLayer.Objects.push_back(DataDesc);
            }
            else if (i == ENUM(MAPOBJ_TYPE::TRIGGER)) {

            }

        }
        m_Data.Layers.push_back(DataLayer);
    }
    int a = 1;

    string TagFileName = g_TagFileName_MapData + "." + m_pMapToolContext->TagArea + "." + m_Data.TagDataFormat + "." + std::to_string(m_Data.iVersion);
    string SavePath = "../Bin/Data/NewBaseData/" + HelperMT::MakeTimestampFileName(TagFileName, ".json");

    //Helper::SaveJson<MapData_Header>(m_Data, SavePath);
    if (true == HelperMT::ExportJsonFile<MapData_Header>(m_Data, SavePath))
        m_isShowSaveFinish = true;
}

void CMapToolGui::Render_ClearLayer()
{
    auto pTagLayers = &m_pMapToolContext->TagLayers;

    ImGui::PushID("MapTool_LayerDelete");
    ImGuiListClipper clipper;
    clipper.Begin((_int)pTagLayers->size());
    while (clipper.Step()) {
        for (_int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            const string TagClearLayer = (*pTagLayers)[i];

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_NoTreePushOnOpen |
                ImGuiTreeNodeFlags_SpanFullWidth |
                ((m_iSelectedIndex == i) ? ImGuiTreeNodeFlags_Selected : 0);

            ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", TagClearLayer.c_str());

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                m_pMapToolCore->Clear_Layer(static_cast<MAPOBJ_TYPE>(i));
            }
        }
    }
    ImGui::PopID();
}

void CMapToolGui::KeyInput()
{
    auto pInputDev = m_pGameInstance->Get_InputDev();
    
    ImGuiIO& io = ImGui::GetIO();

    // 오브젝트 피킹
    if (pInputDev->Mouse_Tap(MOUSE_BTN::LB) && false == io.WantCaptureMouse) {
        PHYSICS_RAY_HIT HitDesc = {};
        if (true == m_pGameInstance->Get_PhysicsSystem()->Raycast(m_PhysicsRay, HitDesc)) {
            if (m_pMapToolContext->TagLayers[ENUM(MAPOBJ_TYPE::PLACED)] == HitDesc.pHitObject->Get_Layer()->Get_LayerTag())
                CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = HitDesc.pHitObject  ;
        }

    }

    // Inspector 창에 떠있는 오브젝트 삭제
    if (pInputDev->Key_Tap(VK_DELETE)) {
        auto pGuiContext = m_pGameInstance->Get_GUISystem()->Get_Context();

        if (nullptr != pGuiContext->pSelectedObject &&
            m_pMapToolContext->TagLayers[ENUM(MAPOBJ_TYPE::PLACED)] == pGuiContext->pSelectedObject->Get_LayerDesc().LayerTag &&
            nullptr != dynamic_cast<CPlacedObject*>(pGuiContext->pSelectedObject)) {

            static_cast<CPlacedObject*>(pGuiContext->pSelectedObject)->Delete_Object();

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

    Safe_Release(m_pMapToolCore);
    Safe_Release(m_pSlotFieldGui);
    Safe_Release(m_pGameInstance);
}
