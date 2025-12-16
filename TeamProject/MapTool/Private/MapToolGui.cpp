#include "pch.h"
#include "MapToolGui.h"
#include "GameInstance.h"

#include "StaticObject.h"
#include "Layer.h"
#include "RayReceiver.h"

#include "Helper_Func.h"

CMapToolGui::CMapToolGui(GUI_CONTEXT* pContext)
    : CBasePanel(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMapToolGui::Initialize()
{
    m_pGameInstance->Get_RayMgr()->Register_Ray(&m_Ray);
    RakeResources();

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

        if (ImGui::Button("Save")) {
            Save_MapData();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            Load_MapData();
        }

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
        }
    }
}

void CMapToolGui::Compute_Ray()
{
    m_Ray = {};

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


    XMStoreFloat3(&m_Ray.vRayDirection, rayDir);
    XMStoreFloat3(&m_Ray.vRayOrigin, rayOrigin);
    m_Ray.fMaxDistance = 1550.f;
}

void CMapToolGui::Place_Object(RAY_HIT* pRayHit)
{
    if (nullptr == pRayHit->pObject ||
        -1 == m_iSelectedIndex)
        return;

    IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();

    CStaticObject::STATIC_OBJECT_DESC* Desc = new CStaticObject::STATIC_OBJECT_DESC;
    Desc->isRayReceiver = m_isObjectPicking;
    Desc->TagModelKey = m_ModelPathPack[m_iSelectedIndex].TagModelKey;
    Desc->TagMaterialKey = m_ModelPathPack[m_iSelectedIndex].TagMaterialKey;

    CGameObject* pStaticObject = Builder::Create_Object({ "MapTool_Level" ,"Proto_GameObject_StaticObject" })
        .Position(pRayHit->vHittedPosition)
        .Scale(m_vScale_PlacedObject)
        .Add_ObjDesc(Desc)
        .Build("Static_Model");

    pObjMgr->Add_Object(pStaticObject, { "MapTool_Level", "Static_Layer" });
}

void CMapToolGui::Set_ObjectPicking(_bool is)
{
    CLayer* pStaticLayer = m_pGameInstance->Get_ObjectMgr()->Get_Layer({ "MapTool_Level", "Static_Layer" });
    if (nullptr == pStaticLayer)
        return;

    for (auto& pObject : pStaticLayer->Get_AllObject()) {
        pObject->Get_Component<CRayReceiver>()->Set_CompActive(is);
    }
}

void CMapToolGui::PreSet_ModelResource()
{
    ImGuiListClipper clipper;
    clipper.Begin((_int)m_ModelPathPack.size());
    while (clipper.Step()) {
        for (_uint i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
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
}

void CMapToolGui::Save_MapData()
{
}

void CMapToolGui::Load_MapData()
{
}

void CMapToolGui::KeyInput()
{
    auto pInputDev = m_pGameInstance->Get_InputDev();

    // 오브젝트 피킹
    if (pInputDev->Mouse_Tap(MOUSE_BTN::LB)) {
        // 오브젝트 피킹이 꺼져있을때 임시로 킴
        _bool isResetObjectPicking = false;
        if (false == m_isObjectPicking) {
            isResetObjectPicking = true;
            Set_ObjectPicking(true);
        }

        RAY_HIT* pRayHit = m_pGameInstance->Get_RayMgr()->Get_FrontRayHit();
        if (nullptr != pRayHit) {
            if ("Static_Layer" == pRayHit->pObject->Get_Layer()->Get_LayerTag())
                CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = pRayHit->pObject;
        }

        if (true == isResetObjectPicking)
            Set_ObjectPicking(false);
    }

    // Inspector 창에 떠있는 오브젝트 삭제
    if (pInputDev->Key_Tap(VK_DELETE)) {
        auto pGuiContext = m_pGameInstance->Get_GUISystem()->Get_Context();

        if (nullptr != pGuiContext->pSelectedObject &&
            "Static_Layer" == pGuiContext->pSelectedObject->Get_LayerDesc().LayerTag &&
            nullptr != dynamic_cast<CStaticObject*>(pGuiContext->pSelectedObject)) {

            static_cast<CStaticObject*>(pGuiContext->pSelectedObject)->Delete_Object();

        }
    }

    // 레이피킹으로 오브젝트 배치
    if (m_pGameInstance->Get_InputDev()->Key_Tap('P')) {
        RAY_HIT* pRayHit = m_pGameInstance->Get_RayMgr()->Get_FrontRayHit();
        if (nullptr != pRayHit) {
            m_vRayHitPos = pRayHit->vHittedPosition;
            Place_Object(pRayHit);
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
