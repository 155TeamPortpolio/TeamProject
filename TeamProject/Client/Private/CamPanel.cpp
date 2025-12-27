#include "pch.h"
#include "CamPanel.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Helper_Func.h"
#include "DebugCamTrace.h"


void CCamPanel::Update_Panel(_float dt)
{
    RefreshCandidates();

    if (!traceEnabled) return;
    if (m_candidates.empty()) return;

    if (traceA < 0) traceA = 0;
    if (traceB < 0) traceB = 0;
    if (traceA >= (int)m_candidates.size()) traceA = (int)m_candidates.size() - 1;
    if (traceB >= (int)m_candidates.size()) traceB = (int)m_candidates.size() - 1;

    auto objA = OBJ->Request_Object(m_candidates[traceA].handle);
    auto objB = OBJ->Request_Object(m_candidates[traceB].handle);

    auto camA = objA ? objA->Get_Component<CCamera>() : nullptr;
    auto camB = objB ? objB->Get_Component<CCamera>() : nullptr;

    DebugCamTrace::Trace_EveryFrame(camA, "A", camB, "B");
}

void CCamPanel::Render_GUI()
{
    const ImGuiViewport* vp = ImGui::GetMainViewport();

    const ImVec2 winSize(320.f, 200.f);
    const float margin = 12.f;
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

    ImGui::Separator();

    ImGui::Checkbox("Trace(Output)", &traceEnabled);

    if (!m_candidates.empty())
    {
        if (traceA < 0) traceA = 0;
        if (traceB < 0) traceB = 0;
        if (traceA >= (int)m_candidates.size()) traceA = (int)m_candidates.size() - 1;
        if (traceB >= (int)m_candidates.size()) traceB = (int)m_candidates.size() - 1;

        ImGui::TextUnformatted("A");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(250.f);

        string aLabel = m_candidates[traceA].displayName + "##TraceA";
        if (ImGui::BeginCombo("##TraceA", m_candidates[traceA].displayName.c_str()))
        {
            for (int i = 0; i < (int)m_candidates.size(); ++i)
            {
                const bool sel = (i == traceA);
                if (ImGui::Selectable(m_candidates[i].displayName.c_str(), sel)) traceA = i;
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::TextUnformatted("B");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(250.f);

        if (ImGui::BeginCombo("##TraceB", m_candidates[traceB].displayName.c_str()))
        {
            for (int i = 0; i < (int)m_candidates.size(); ++i)
            {
                const bool sel = (i == traceB);
                if (ImGui::Selectable(m_candidates[i].displayName.c_str(), sel)) traceB = i;
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    else
    {
        ImGui::TextUnformatted("No camera candidates.");
    }

    ImGui::End();
}


void CCamPanel::RefreshCandidates()
{
    OBJECT_HANDLE prevSelected{};
    if (!m_candidates.empty() && m_selectedIndex >= 0 && m_selectedIndex < (int)m_candidates.size())
        prevSelected = m_candidates[m_selectedIndex].handle;

    OBJECT_HANDLE mainSelected{};
    auto curMainCam = CAM->Get_BaseCam();
    if (curMainCam)
        mainSelected = curMainCam->Get_Owner()->Get_Handle();

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
            char displayBuf[256];
            sprintf_s(displayBuf, "%s (%u)", c.displayName.c_str(), c.handle.hObjID);
            c.displayName = displayBuf;
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

    auto Match = [](const OBJECT_HANDLE& a, const OBJECT_HANDLE& b)
        {
            return a.Level == b.Level && a.Layer == b.Layer && a.hObjID == b.hObjID;
        };

    m_selectedIndex = 0;

    if (mainSelected.hObjID != 0)
    {
        for (int i = 0; i < (int)m_candidates.size(); ++i)
        {
            if (Match(m_candidates[i].handle, mainSelected))
            {
                m_selectedIndex = i;
                m_needRefresh = false;
                return;
            }
        }
    }

    for (int i = 0; i < (int)m_candidates.size(); ++i)
    {
        if (Match(m_candidates[i].handle, prevSelected))
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

    auto selectedObj = OBJ->Request_Object(selected.handle);
    auto selectedCam = selectedObj ? selectedObj->Get_Component<CCamera>() : nullptr;

    const bool selectedIsWip = selectedObj && selectedObj->Get_InstanceName() == "SequenceCam";

    string previewLabel = selected.displayName;
    if (selectedIsWip) previewLabel += " (WIP)";
    if (selectedCam && selectedCam == curMain) previewLabel += " (Main)";

    ImGui::SetNextItemWidth(220.f);
    if (ImGui::BeginCombo("##MainCamCandidates", previewLabel.c_str()))
    {
        for (int i = 0; i < (int)m_candidates.size(); ++i)
        {
            const auto& c = m_candidates[i];

            auto obj = OBJ->Request_Object(c.handle);
            auto cam = obj ? obj->Get_Component<CCamera>() : nullptr;

            const bool isWip = obj && obj->Get_InstanceName() == "SequenceCam";

            string visible = c.displayName;
            if (isWip) visible += " (WIP)";
            if (cam && cam == curMain) visible += " (Main)";

            string label = visible + "##" + c.uniqueId;

            const bool isSelected = (i == m_selectedIndex);

            if (isWip)
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.45f);

            const bool clicked = ImGui::Selectable(label.c_str(), isSelected);

            if (isWip)
                ImGui::PopStyleVar();

            if (isWip)
            {
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("WIP: SequenceCam is not selectable.");

                if (isSelected) ImGui::SetItemDefaultFocus();
                continue;
            }

            if (clicked)
            {
                m_selectedIndex = i;

                auto selObj = OBJ->Request_Object(m_candidates[m_selectedIndex].handle);
                auto selCam = selObj ? selObj->Get_Component<CCamera>() : nullptr;
                if (selCam) CAM->Set_MainCam(selCam, 0.25f);
            }

            if (isSelected) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
}
