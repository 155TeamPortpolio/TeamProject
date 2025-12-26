#include "pch.h"
#include "CamPanel.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Helper_Func.h"

void CCamPanel::Update_Panel(_float dt)
{
    if (m_needRefresh)
        RefreshCandidates();
}

void CCamPanel::Render_GUI()
{
    const ImGuiViewport* vp = ImGui::GetMainViewport();

    const ImVec2 winSize(320.f, 80.f);
    const float margin    = 12.f;
    const float shiftLeft = 400.f;

    const ImVec2 winPos(
        vp->WorkPos.x + vp->WorkSize.x - winSize.x - margin - shiftLeft,
        vp->WorkPos.y + margin
    );

    ImGui::SetNextWindowPos(winPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(winSize, ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse;

    Helper::DarkThemeStyle style;

    if (!ImGui::Begin("CamPanel", nullptr, flags))
    {
        ImGui::End();
        return;
    }
    DrawMainCamSelector();
    ImGui::End();
}

void CCamPanel::RefreshCandidates()
{
    OBJECT_HANDLE prevSelected{};
    if (!m_candidates.empty() && m_selectedIndex >= 0 && m_selectedIndex < (int)m_candidates.size())
        prevSelected = m_candidates[m_selectedIndex].handle;

    m_candidates.clear();

    auto layer = OBJ->Get_Layer({m_levelTag, m_layerTag});
    if (!layer)
    {
        m_selectedIndex = 0;
        m_needRefresh = false;
        return;
    }

    const auto& objs = layer->Get_AllObject();

    unordered_map<string, int> nameCount;

    for (auto obj : objs)
    {
        if (!obj) continue;
        if (!obj->Is_Alive()) continue;
        if (!obj->Is_Root()) continue;

        auto cam = obj->Get_Component<CCamera>();
        if (!cam) continue;

        CamCandidate c{};
        c.handle = obj->Get_Handle();
        c.displayName = obj->Get_InstanceName();

        char uid[256];
        sprintf_s(uid, "%s/%s/%u", c.handle.Level.c_str(), c.handle.Layer.c_str(), c.handle.hObjID);
        c.uniqueId = uid;

        nameCount[c.displayName]++;

        m_candidates.push_back(move(c));
    }

    for (auto& c : m_candidates)
    {
        if (nameCount[c.displayName] > 1)
        {
            char dup[256];
            sprintf_s(dup, "%s (%u)", c.displayName.c_str(), c.handle.hObjID);
            c.displayName = dup;
        }
    }

    sort(m_candidates.begin(), m_candidates.end(),
        [](const CamCandidate& a, const CamCandidate& b)
        {
            if (a.displayName != b.displayName) return a.displayName < b.displayName;
            return a.uniqueId < b.uniqueId;
        });

    if (m_candidates.empty())
    {
        m_selectedIndex = 0;
        m_needRefresh = false;
        return;
    }

    m_selectedIndex = 0;
    for (int i = 0; i < (int)m_candidates.size(); ++i)
    {
        const auto& h = m_candidates[i].handle;
        if (h.Level == prevSelected.Level && h.Layer == prevSelected.Layer && h.hObjID == prevSelected.hObjID)
        {
            m_selectedIndex = i;
            break;
        }
    }

    m_needRefresh = false;
}

void CCamPanel::DrawMainCamSelector()
{
    ImGui::TextUnformatted("MainCam");
    ImGui::SameLine();

    if (m_candidates.empty())
    {
        ImGui::TextUnformatted("-");
        return;
    }

    if (m_selectedIndex < 0) m_selectedIndex = 0;
    if (m_selectedIndex >= (int)m_candidates.size()) m_selectedIndex = (int)m_candidates.size() - 1;

    auto curMain = CAM->Get_BaseCam();

    const auto& selected = m_candidates[m_selectedIndex];
    const char* preview = selected.displayName.c_str();

    ImGui::SetNextItemWidth(220.f);
    if (ImGui::BeginCombo("##MainCamCandidates", preview))
    {
        for (int i = 0; i < (int)m_candidates.size(); ++i)
        {
            const auto& c = m_candidates[i];

            auto obj = OBJ->Request_Object(c.handle);
            auto cam = obj ? obj->Get_Component<CCamera>() : nullptr;

            string visible = c.displayName;
            if (cam && cam == curMain) visible += " (Main)";

            string label = visible + "##" + c.uniqueId;

            const bool isSelected = (i == m_selectedIndex);
            if (ImGui::Selectable(label.c_str(), isSelected))
            {
                m_selectedIndex = i;

                auto selObj = m_candidates[m_selectedIndex].handle.Get();
                auto selCam = selObj ? selObj->Get_Component<CCamera>() : nullptr;
                if (selCam) CAM->Set_MainCam(selCam);
            }

            if (isSelected) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
}