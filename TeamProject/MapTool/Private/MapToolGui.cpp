#include "pch.h"
#include "MapToolGui.h"
#include "GameInstance.h"

#include "PlacedObject.h"
#include "Layer.h"
#include "RayReceiver.h"

#include "Helper_Func.h"
#include "Helper_MapTool.h"

CMapToolGui::CMapToolGui(GUI_CONTEXT* pContext)
    : CBasePanel(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
    , m_TagLayers{ "All_Layer", "PlacedObject_Layer", "TriggerObject_Layer" }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMapToolGui::Initialize()
{
    //m_pGameInstance->Get_RayMgr()->Register_Ray(&m_Ray);
    RakeResources();

    m_TagPlacedObjectLayer = "PlacedObject_Layer";
    

    return S_OK;
}

void CMapToolGui::Update_Panel(_float dt)
{
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
    ImGui::Text("Last Ray Hit Pos : %.3f, %.3f, %.3f ", m_vRayHitPos.x, m_vRayHitPos.y, m_vRayHitPos.z);
    ImGui::EndChild();

    ImGui::Text("");/////////////////////////////////

    const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);
    ImGui::Text("Setting Placed Object");
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
        ImGui::InputInt(" ##Version", reinterpret_cast<int*>(&m_iVersion));

        if (ImGui::Button("Save")) {
            Save_MapData();
        }

        if (ImGui::Button("Load")) {
            Load_MapData();
        }

        ImGui::EndChild();
        ImGui::TreePop();
    }


    //ImGui::SameLine();

    if (ImGui::TreeNode("Clear")) {
        ImGui::BeginChild("##MapToolClearLayerList", ImVec2{ 0, childHeight }, true);

        Clear_Layer();

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

    // BG Ray
    //XMStoreFloat3(&m_Ray.vRayDirection, rayDir);
    //XMStoreFloat3(&m_Ray.vRayOrigin, rayOrigin);
    //m_Ray.fMaxDistance = 1550.f;

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

    CGameObject* pStaticObject = Builder::Create_Object({ "MapTool_Level" ,"Proto_GameObject_PlacedObject" })
        .Position(pRayHit->vPoint)
        .Scale(m_vScale_PlacedObject)
        .Add_ObjDesc(Desc)
        .Collider(ColDesc)
        .Build("Static_Model");

    pObjMgr->Add_Object(pStaticObject, { "MapTool_Level", m_TagPlacedObjectLayer });
}

void CMapToolGui::Set_ObjectPicking(_bool is)
{
    // 사용X
    CLayer* pStaticLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ "MapTool_Level", m_TagPlacedObjectLayer });
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

    m_Data.iVersion = m_iVersion;
    m_Data.TagDataFormat = "MapTool.Data";
    //m_TagLayers{ "PlacedObject_Layer", "FloorObject_Layer", "TriggerObject_Layer", "Navigation_Layer" }

    for (_uint i = 0; i < (_uint)m_TagLayers.size(); ++i) {
        CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ "MapTool_Level", m_TagLayers[i] });
        if (nullptr == pLayer)
            continue;
        MapData_Layer DataLayer = {};
        DataLayer.TagLayer = m_TagLayers[i];

        for (auto& pObject : pLayer->Get_AllObject()) {
            if (m_TagLayers[i] == "PlacedObject_Layer") {
                MapData_Object DataDesc = {};
                static_cast<CPlacedObject*>(pObject)->Export_ObjectData(&DataDesc);
                DataLayer.Objects.push_back(DataDesc);
            }
            else if (m_TagLayers[i] == "TriggerObject_Layer") {

            }

        }
        m_Data.Layers.push_back(DataLayer);
    }
    int a = 1;

    string TagFileName = m_Data.TagDataFormat + "_" + std::to_string(m_Data.iVersion);
    string SavePath = "../Bin/Data/" + HelperMT::MakeTimestampFileName(TagFileName, ".json");

    HelperMT::SaveJson_MapTool<MapData_Header>(m_Data, SavePath);
    
    //m_Data = Helper::LoadJson<MapData_Header>(path);
}

void CMapToolGui::Load_MapData()
{
    filesystem::path OpenPath = Helper::OpenFile_Dialogue();

    if (OpenPath.empty()) 
        return;
    
    if (OpenPath.extension().string() != ".json") {
        MSG_BOX("[MapTool] Load Map Data Failed.\nJson 파일이 아닙니다.");
        return;
    }


    MapData_Header mapdata = Helper::LoadJson<MapData_Header>(OpenPath.string());

    if (mapdata.iVersion != m_iVersion) {
        MSG_BOX("[MapTool] Load Map Data Failed.\n잘못된 버전입니다.");
        return;
    }

    for (auto& layerdata : mapdata.Layers) {
        // 레이어 태그 무결성 검사
        const auto iter = find(m_TagLayers.begin(), m_TagLayers.end(), layerdata.TagLayer);
        if (iter == m_TagLayers.end())
            continue;

        for (auto& objectdata : layerdata.Objects) 
            Place_PlacedObjectFromLoadData(&objectdata);
    }

}

void CMapToolGui::Place_PlacedObjectFromLoadData(MapData_Object* pData)
{
    if (nullptr == pData)
        return;

    IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();

    CPlacedObject::MAPTOOL_OBJECT_DESC* Desc = new CPlacedObject::MAPTOOL_OBJECT_DESC;
    Desc->isRayReceiver = m_isObjectPicking;
    Desc->TagModelKey = pData->TagModelResourceKey;
    Desc->TagMaterialKey = pData->TagMaterialResourceKey;
   
    COLLIDER_DESC ColDesc = {};
    ColDesc.bCooking = true;
    ColDesc.strModelKey = pData->TagModelResourceKey;

    CGameObject* pStaticObject = Builder::Create_Object({ "MapTool_Level" ,"Proto_GameObject_PlacedObject" })
        .Add_ObjDesc(Desc)
        .Collider(ColDesc)
        .Build("Placed_Model");
    
    _float4x4 matWorld = {
        pData->vRight[0], pData->vRight[1] , pData->vRight[2] , pData->vRight[3],
        pData->vUp[0],  pData->vUp[1] , pData->vUp[2] , pData->vUp[3],
        pData->vLook[0], pData->vLook[1] , pData->vLook[2] , pData->vLook[3],
        pData->vPos[0], pData->vPos[1] , pData->vPos[2] , pData->vPos[3] };

    pStaticObject->Get_Component<CTransform>()->TranslateMatrix(XMLoadFloat4x4(&matWorld));

    pObjMgr->Add_Object(pStaticObject, { "MapTool_Level", m_TagPlacedObjectLayer });


}

void CMapToolGui::Clear_Layer()
{
    ImGui::PushID("MapTool_LayerDelete");
    ImGuiListClipper clipper;
    clipper.Begin((_int)m_TagLayers.size());
    while (clipper.Step()) {
        for (_int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            const string TagClearLayer = m_TagLayers[i];

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_NoTreePushOnOpen |
                ImGuiTreeNodeFlags_SpanFullWidth |
                ((m_iSelectedIndex == i) ? ImGuiTreeNodeFlags_Selected : 0);

            ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", TagClearLayer.c_str());

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                if (m_TagLayers[i] == "All_Layer") {
                    for (_int j = 0; j < (_int)m_TagLayers.size(); j++)
                    {
                        CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ "MapTool_Level", m_TagLayers[j]});
                        if (nullptr == pLayer) 
                            continue;
                        pLayer->Clear_Layer();
                    }
                }
                else {
                    CLayer* pLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ "MapTool_Level", TagClearLayer });
                    if (nullptr == pLayer) {
                        ImGui::PopID();
                        return;
                    }
                    pLayer->Clear_Layer();
                }
                m_pGameInstance->Get_GUISystem()->Get_Context()->pSelectedObject = { nullptr };
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
            if (m_TagPlacedObjectLayer == HitDesc.pHitObject->Get_Layer()->Get_LayerTag())
                CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = HitDesc.pHitObject  ;
        }

    }

    // Inspector 창에 떠있는 오브젝트 삭제
    if (pInputDev->Key_Tap(VK_DELETE)) {
        auto pGuiContext = m_pGameInstance->Get_GUISystem()->Get_Context();

        if (nullptr != pGuiContext->pSelectedObject &&
            m_TagPlacedObjectLayer == pGuiContext->pSelectedObject->Get_LayerDesc().LayerTag &&
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
    Safe_Release(m_pGameInstance);
}
