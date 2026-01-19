#include "pch.h"
#include "MapToolAssistant.h"
#include "GameInstance.h"
#include "MapToolCore.h"
#include "Helper_Func.h"
#include "Helper_MapTool.h"

CMapToolAssistant::CMapToolAssistant(GUI_CONTEXT* pContext)
    : CBasePanel(pContext)
    , m_pMapToolCore(CMapToolCore::GetInstance())
{
    Safe_AddRef(m_pMapToolCore);
}

HRESULT CMapToolAssistant::Initialize()
{
    m_pMapToolContext = m_pMapToolCore->Get_Context();

    Rake_BoneData();

    return S_OK;
}

void CMapToolAssistant::Update_Panel(_float dt)
{
}

void CMapToolAssistant::Render_GUI()
{

    if (false == m_isOpen)
        return;

    //ImGui::SetNextWindowPos(ImVec2(200.f, g_iWinSizeY * 0.7f), ImGuiCond_Always);
    //ImGui::SetNextWindowSize(ImVec2((_float)g_iWinSizeX - 200.f - (g_iWinSizeX * 0.15f), (_float)g_iWinSizeY * 0.3f), ImGuiCond_Always);
    ImGui::Begin("MapTool Assistant", nullptr);

    ImGui::PushID(this);

    float childWidth = ImGui::GetContentRegionAvail().x;
    const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
    const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);

    if (ImGui::Button("Load BoneInfo")) {
        filesystem::path OpenPath = Helper::OpenFile_Dialogue();
        LoadBoneData(OpenPath);
    }

    if (ImGui::Button("Recommend Position")) {
        Recommend_Transform();
    }

    if (ImGui::TreeNode("Transform Assist##TransformAssist")) {
        ImGui::BeginChild("##BoneInformation", ImVec2{ 0, childHeight }, true);

        RecommendList();

        ImGui::EndChild();
        ImGui::BeginChild("##BoneInformation", ImVec2{ 0, childHeight }, true);
        
        SelectBoneData();

        ImGui::EndChild();
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Model List##TransformAssist_ModelList")) {
        ImGui::BeginChild("##TransformAssist_ModelList", ImVec2{ 0, childHeight }, true);

        SelectModel();

        ImGui::EndChild();
        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::End();
}

void CMapToolAssistant::Rake_BoneData()
{
    filesystem::path OriginPath = "../Bin/Data/BoneData/";
    
    HelperMT::EnsureDirectoryExists(OriginPath);

    for (const auto& entry : filesystem::recursive_directory_iterator(OriginPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            filesystem::path DataPath = entry.path();
            LoadBoneData(DataPath);
        }
    }

}

void CMapToolAssistant::LoadBoneData(filesystem::path& dir)
{

    if (dir.empty())
        return;


    if (dir.extension().string() != ".json") {
        MSG_BOX("[MapTool] Load Bone Data Failed.\nJson 파일이 아닙니다.");
        return;
    }
    if (dir.stem().string().find("BoneData") == string::npos) {
        MSG_BOX("[MapTool] Bone Data 파일이 아닙니다.");
        return;
    }

    BONE_DATA_HEADER bonedata = Helper::LoadJson<BONE_DATA_HEADER>(dir.string());
    
    if ("BoneData" != bonedata.TagDataFormat) {
        MSG_BOX("[MapTool] Bone Data 파일이 아닙니다.");
        return;
    }
    m_BoneData.push_back(bonedata);
    
}

void CMapToolAssistant::SelectModel()
{
    ImGui::PushID("MapToolAssistant_SelectModel");
    ImGuiListClipper clipper;
    clipper.Begin((_int)m_BoneData.size());
    while (clipper.Step()) {
        for (_int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            const string TagResourceName = m_BoneData[i].TagModel;

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_NoTreePushOnOpen |
                ImGuiTreeNodeFlags_SpanFullWidth |
                ((m_iSelectedModelIndex == i) ? ImGuiTreeNodeFlags_Selected : 0);

            ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", TagResourceName.c_str());

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                m_iSelectedModelIndex = i;
            }
        }
    }
    ImGui::PopID();
}

void CMapToolAssistant::SelectBoneData()
{
    if (0 > m_iSelectedModelIndex)
        return;

    vector<BONE_INFO> BoneData = m_BoneData[m_iSelectedModelIndex].BoneInfos;
    ImGui::PushID("MapToolAssistant_SelectBoneInfo");
    ImGuiListClipper clipper;
    clipper.Begin((_int)BoneData.size());
    while (clipper.Step()) {
        for (_int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            const string TagResourceName = BoneData[i].TagBone;

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_NoTreePushOnOpen |
                ImGuiTreeNodeFlags_SpanFullWidth;

            ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", TagResourceName.c_str());

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                if (nullptr != m_pContext->pSelectedObject) {
                    auto pTransform = m_pContext->pSelectedObject->Get_Component<CTransform>();
                    _matrix offsetinversemat = XMMatrixInverse(nullptr, XMLoadFloat4x4(&BoneData[i].BoneOffsetMatrix));
                    _matrix FlipY = XMMatrixScaling(1.f, -1.f, 1.f);
                    _matrix resultmat = FlipY * offsetinversemat * FlipY;
                    pTransform->TranslateMatrix(resultmat);
                }

            }
        }
    }
    ImGui::PopID();


}

void CMapToolAssistant::Recommend_Transform()
{
    if (nullptr == m_pContext->pSelectedObject)
        return;

    m_RecommendList.clear();

    string TagObject = m_pContext->pSelectedObject->Get_InstanceName();

    for (auto& header : m_BoneData) 
        for (auto& boneinfo : header.BoneInfos) 
            if (TagObject == boneinfo.TagBone)
                m_RecommendList.push_back(boneinfo);
        
}

void CMapToolAssistant::RecommendList()
{
    ImGui::PushID("MapToolAssistant_RecommendList");
    ImGuiListClipper clipper;
    clipper.Begin((_int)m_RecommendList.size());
    while (clipper.Step()) {
        for (_int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            const string TagResourceName = m_RecommendList[i].TagBone;

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_NoTreePushOnOpen |
                ImGuiTreeNodeFlags_SpanFullWidth;

            ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", TagResourceName.c_str());

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                if (nullptr != m_pContext->pSelectedObject) {
                    auto pTransform = m_pContext->pSelectedObject->Get_Component<CTransform>();
                    _matrix offsetinversemat = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_RecommendList[i].BoneOffsetMatrix));
                    _matrix FlipY = XMMatrixScaling(1.f, -1.f, 1.f);
                    _matrix resultmat = FlipY * offsetinversemat * FlipY;
                    pTransform->TranslateMatrix(resultmat);
                }

            }
        }
    }
    ImGui::PopID();
}

CMapToolAssistant* CMapToolAssistant::Create(GUI_CONTEXT* pContext)
{
    CMapToolAssistant* pInstance = new CMapToolAssistant(pContext);
    if (FAILED(pInstance->Initialize())) {
        MSG_BOX("CMapToolAssistant Create Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapToolAssistant::Free()
{
    __super::Free();

    Safe_Release(m_pMapToolCore);
    Safe_Release(m_pGameInstance);
}
