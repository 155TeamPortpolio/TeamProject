#include "pch.h"
#include "SlotFieldGui.h"
#include "GUI_Inc/imgui_stdlib.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Helper_MapTool.h"
#include "MapToolCore.h"

CSlotFieldGui::CSlotFieldGui(GUI_CONTEXT* pContext)
	: CBasePanel(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
    , m_pMapToolCore(CMapToolCore::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pMapToolCore);
}

HRESULT CSlotFieldGui::Initialize()
{
    m_pMapToolContext = m_pMapToolCore->Get_Context();


    // 저장 성공 시, 알림 쿨타임
    m_vShowSaveFinish = { 3.f, 0.f };

    return S_OK;
}

void CSlotFieldGui::Update_Panel(_float dt)
{
    CheckCoolTime(dt);

    if (m_isRequestApply)
    {
        ApplyDraftToB();
        m_isRequestApply = false;
    }

    if (m_isRequestSave)
    {
        SaveSlotData();
        m_isRequestSave = false;
    }
}

void CSlotFieldGui::Render_GUI()
{
    if (false == m_isOpen)
        return;

    ImGui::SetNextWindowPos(ImVec2(200.f, g_iWinSizeY * 0.7f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2((_float)g_iWinSizeX - 200.f - (g_iWinSizeX * 0.15f), (_float)g_iWinSizeY * 0.3f), ImGuiCond_Always);
    ImGui::Begin("Slot Field", nullptr);

    ImGui::PushID(this);

    
    float fFullW = ImGui::GetContentRegionAvail().x;
    float fLeftW = fFullW * (1.f / 3.f);
    float fRightW = fFullW - fLeftW;
    float fHeight = (_float)g_iWinSizeY * 0.255f;

    const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
    const float childHeight = (textLineHeight * 10.2f) + (ImGui::GetStyle().WindowPadding.y * 2);

    /* Slot Manager */
    ImGui::BeginChild("A##SlotLeft", ImVec2(fLeftW, fHeight), true);

    DrawLeftPanel();

    ImGui::EndChild();



    /* Value Inputer */
    ImGui::SameLine(0.f, 5.f);
    ImGui::BeginChild("B##SlotRight", ImVec2(fRightW - 5.f, fHeight), true);

    DrawRightPanel();


    ImGui::EndChild();



    ImGui::PopID();
    ImGui::End();
}

void CSlotFieldGui::CheckCoolTime(_float dt)
{
    if (m_isShowSaveFinish) {
        m_vShowSaveFinish.y += dt;
        if (m_vShowSaveFinish.x < m_vShowSaveFinish.y) {
            m_vShowSaveFinish.y = 0.f;
            m_isShowSaveFinish = false;
        }
    }
}

SlotValue CSlotFieldGui::MakeDefaultValue(SLOT_DATA_TYPE eType)
{
    switch (eType)
    {
    case MapTool::SLOT_DATA_TYPE::Int:
        return int64_t(0);
    case MapTool::SLOT_DATA_TYPE::Float:
        return _double(0);
    case MapTool::SLOT_DATA_TYPE::Bool:
        return _bool(false);
    case MapTool::SLOT_DATA_TYPE::String:
        return string{};
    case MapTool::SLOT_DATA_TYPE::Float2:
        return XMFLOAT2{ 0.f, 0.f };
    case MapTool::SLOT_DATA_TYPE::Float3:
        return XMFLOAT3{ 0.f, 0.f, 0.f };
    case MapTool::SLOT_DATA_TYPE::Float4:
        return XMFLOAT4{ 0.f, 0.f, 0.f, 0.f };
    }

    return _int(0);
}

string CSlotFieldGui::MakeNewID()
{
    char buf[32];
    std::snprintf(buf, sizeof(buf), "slot_%06u", ++m_iIdCounter);
    return buf;
}

void CSlotFieldGui::InjectDefaultToAllRecords(vector<RECORD>& records, const FIELD_DATA_DEFINE& def)
{
    for (auto& r : records)
        r.values.try_emplace(def.id, def.defaultvalue);
}

const char* CSlotFieldGui::FieldDataTypeName(SLOT_DATA_TYPE eType)
{
    switch (eType)
    {
    case SLOT_DATA_TYPE::Int:    return "Int";
    case SLOT_DATA_TYPE::Float:  return "Float";
    case SLOT_DATA_TYPE::Bool:   return "Bool";
    case SLOT_DATA_TYPE::String: return "String";
    case SLOT_DATA_TYPE::Float2: return "Float2";
    case SLOT_DATA_TYPE::Float3: return "Float3";
    case SLOT_DATA_TYPE::Float4: return "Float4";
    default:                      return "END";
    }
}

FIELD_DATA_DEFINE CSlotFieldGui::MakeFromPrefab(const tagFieldDataDef& prefab)
{
    FIELD_DATA_DEFINE def = prefab;
    def.id = MakeNewID();
    if (def.eDataType == SLOT_DATA_TYPE::END)
        def.eDataType = SLOT_DATA_TYPE::Float;

    def.defaultvalue = MakeDefaultValue(def.eDataType);
    if (def.eDataType != SLOT_DATA_TYPE::Float)
    {
        def.hasRange = FromBool(false);
        def.minV = 0.0;
        def.maxV = 1.0;
    }
    return def;
}

void CSlotFieldGui::ApplyDraftToB()
{
    m_AppliedTabs = m_DraftTabs;

    // Apply 시점에 "값을 전부 생성"하지 않는 게 핵심.
    // 값은 B에서 Set 누른 셀만 생성되도록 유지.
    // (그래야 “모든 오브젝트에 값이 안 들어갈 수 있음”을 자연스럽게 충족)
}

SlotValue& CSlotFieldGui::EnsureValue(_int iObjIndex, const FIELD_DATA_DEFINE& Tab)
{
    auto& mp = m_ObjUserValue[iObjIndex];
    auto it = mp.find(Tab.id);
    if (it == mp.end())
        it = mp.emplace(Tab.id, Tab.defaultvalue).first;

    // 타입 불일치면 default로 리셋(안전)
    switch (Tab.eDataType)
    {
    case SLOT_DATA_TYPE::Int:
        if (!std::holds_alternative<int64_t>(it->second)) it->second = int64_t{ 0 };
        break;
    case SLOT_DATA_TYPE::Float:
        if (!std::holds_alternative<_double>(it->second)) it->second = (_double)0.0;
        break;
    case SLOT_DATA_TYPE::Bool:
        if (!std::holds_alternative<_bool>(it->second)) it->second = (_bool)false;
        break;
    case SLOT_DATA_TYPE::String:
        if (!std::holds_alternative<std::string>(it->second)) it->second = std::string{};
        break;
    case SLOT_DATA_TYPE::Float2:
        if (!std::holds_alternative<XMFLOAT2>(it->second)) it->second = XMFLOAT2{ 0,0 };
        break;
    case SLOT_DATA_TYPE::Float3:
        if (!std::holds_alternative<XMFLOAT3>(it->second)) it->second = XMFLOAT3{ 0,0,0 };
        break;
    case SLOT_DATA_TYPE::Float4:
        if (!std::holds_alternative<XMFLOAT4>(it->second)) it->second = XMFLOAT4{ 0,0,0,0 };
        break;
    default:
        break;
    }

    return it->second;
}

void CSlotFieldGui::LoadBaseData()
{
    m_LoadedData = {};

    m_LoadedData = m_pMapToolCore->Load_MapData();

    if (m_LoadedData.LoadedObjects.size() < 1) {
        MSG_BOX("데이터 읽기 실패!");
        return;
    }
   
    if (m_ObjUserValue.size() != m_LoadedData.LoadedObjects.size())
        m_ObjUserValue.resize(m_LoadedData.LoadedObjects.size());
}

void CSlotFieldGui::to_json(MTjson& j, const SlotValue& value)
{
    j = json::object();
    j["type"] = nlohmann::SlotTypeName(value);

    std::visit([&](auto&& arg)
        {
            j["value"] = arg;
        }, value);
}

void CSlotFieldGui::from_json(const MTjson& j, SlotValue& value)
{
    const std::string t = j.at("type").get<std::string>();

    if (t == "Int")    value = j.at("value").get<int64_t>();
    else if (t == "Float")  value = j.at("value").get<_double>();
    else if (t == "Bool")   value = (_bool)j.at("value").get<bool>();
    else if (t == "String") value = j.at("value").get<std::string>();
    else if (t == "Float2") value = j.at("value").get<DirectX::XMFLOAT2>();
    else if (t == "Float3") value = j.at("value").get<DirectX::XMFLOAT3>();
    else if (t == "Float4") value = j.at("value").get<DirectX::XMFLOAT4>();
    else                    value = int64_t{ 0 };
}

void CSlotFieldGui::SaveSlotData()
{
    MapData_Slot_Header SlotHeader = { };
    SlotHeader.TagDataFormat = m_TagSlotFormat;
    SlotHeader.TagArea = m_pMapToolContext->TagArea;
    SlotHeader.iVersion = m_pMapToolContext->iVersion;

    unordered_map<string, const FIELD_DATA_DEFINE*> TabByID;
    TabByID.reserve(m_AppliedTabs.size());
    for (const auto& t : m_AppliedTabs)
        TabByID.emplace(t.id, &t);

    const _int objCount = (_int)m_LoadedData.LoadedObjects.size();
    for (int objIdx = 0; objIdx < objCount; ++objIdx)
    {
        if (objIdx >= (_int)m_ObjUserValue.size())
            break;

        const _int objNo = m_LoadedData.LoadedObjects[objIdx].iObjIdx;
        const auto& mp = m_ObjUserValue[objIdx];

        for (const auto& [slotId, val] : mp)
        {
            auto itTab = TabByID.find(slotId);
            if (itTab == TabByID.end())
                continue;

            FIELD_DATA_DEFINE out;
            out.iObjID = objNo;
            out.TagName = itTab->second->TagName;
            out.eDataType = itTab->second->eDataType;
            out.defaultvalue = val;

            SlotHeader.values.push_back(std::move(out));
        }

    }

    string TagFileName = m_LoadedData.tagDataFormat + "." + m_pMapToolContext->TagArea + "." + SlotHeader.TagDataFormat + "." + std::to_string(SlotHeader.iVersion);
    string SavePath = "../Bin/Data/NewSlotData/" + HelperMT::MakeTimestampFileName(TagFileName, ".json");

    //Helper::SaveJson<MapData_Slot_Header>(SlotHeader, SavePath);
    if (true == HelperMT::ExportJsonFile<MapData_Slot_Header>(SlotHeader, SavePath))
        m_isShowSaveFinish = true;

}

void CSlotFieldGui::DrawDefaultEditor(FIELD_DATA_DEFINE& def)
{
    switch (def.eDataType)
    {
    case MapTool::SLOT_DATA_TYPE::Int:
    {
        int64_t v = std::get<int64_t>(def.defaultvalue);
        if (ImGui::DragScalar("##def_i64", ImGuiDataType_S64, &v, 1.0f))
            def.defaultvalue = v;
        break;
    }
    case MapTool::SLOT_DATA_TYPE::Float:
    {
        double v = std::get<double>(def.defaultvalue);
        float vf = (float)v;

        if (def.hasRange)
        {
            float mn = (float)def.minV, mx = (float)def.maxV;
            if (ImGui::SliderFloat("##def_f", &vf, mn, mx))
                def.defaultvalue = (double)vf;
        }
        else
        {
            if (ImGui::DragFloat("##def_f", &vf, 0.01f))
                def.defaultvalue = (double)vf;
        }
        break;
    }
    case MapTool::SLOT_DATA_TYPE::Bool:
    {
        bool v = std::get<bool>(def.defaultvalue);
        if (ImGui::Checkbox("##def_b", &v))
            def.defaultvalue = v; 
        break;
    }
    case MapTool::SLOT_DATA_TYPE::String:
    {
        // std::string 직접 편집 (imgui_stdlib) :contentReference[oaicite:4]{index=4}
        std::string v = std::get<std::string>(def.defaultvalue);
        if (ImGui::InputText("##def_s", &v))
            def.defaultvalue = v;
        break;
    }
    case MapTool::SLOT_DATA_TYPE::Float2:
    {
        XMFLOAT2 v = std::get<XMFLOAT2>(def.defaultvalue);
        float arr[2] = { v.x, v.y };
        if (ImGui::DragFloat2("##def_v2", arr, 0.01f))
            def.defaultvalue = XMFLOAT2{ arr[0], arr[1] };
        break;
    }
    case MapTool::SLOT_DATA_TYPE::Float3:
    {
        XMFLOAT3 v = std::get<XMFLOAT3>(def.defaultvalue);
        float arr[3] = { v.x, v.y, v.z };
        if (ImGui::DragFloat3("##def_v3", arr, 0.01f))
            def.defaultvalue = XMFLOAT3{ arr[0], arr[1], arr[2] };
        break;
    }
    case MapTool::SLOT_DATA_TYPE::Float4: 
    {
        XMFLOAT4 v = std::get<XMFLOAT4>(def.defaultvalue);
        float arr[4] = { v.x, v.y, v.z, v.w };
        if (ImGui::DragFloat4("##def_v4", arr, 0.01f))
            def.defaultvalue = XMFLOAT4{ arr[0], arr[1], arr[2], arr[3] };
        break;
    }
    }
}

void CSlotFieldGui::DrawValueEditor(const FIELD_DATA_DEFINE& tab, SlotValue& v)
{
    switch (tab.eDataType)
    {
    case SLOT_DATA_TYPE::Int:
    {
        auto* p = std::get_if<int64_t>(&v);
        int64_t cur = p ? *p : 0;
        if (ImGui::DragScalar("##i", ImGuiDataType_S64, &cur, 1.0f))
            v = cur;
    } break;

    case SLOT_DATA_TYPE::Float:
    {
        auto* p = std::get_if<_double>(&v);
        _double cur = p ? *p : 0.0;
        if (ToBool(tab.hasRange))
        {
            _double mn = tab.minV, mx = tab.maxV;
            if (ImGui::SliderScalar("##f", ImGuiDataType_Double, &cur, &mn, &mx))
                v = cur;
        }
        else
        {
            if (ImGui::DragScalar("##f", ImGuiDataType_Double, &cur, 0.01f))
                v = cur;
        }
    } break;

    case SLOT_DATA_TYPE::Bool:
    {
        auto* p = std::get_if<_bool>(&v);
        bool b = p ? ToBool(*p) : false;
        if (ImGui::Checkbox("##b", &b))
            v = FromBool(b);
    } break;

    case SLOT_DATA_TYPE::String:
    {
        auto* p = std::get_if<std::string>(&v);
        if (!p) { v = std::string{}; p = std::get_if<std::string>(&v); }
        ImGui::InputText("##s", p);
    } break;

    case SLOT_DATA_TYPE::Float2:
    {
        auto* p = std::get_if<XMFLOAT2>(&v);
        XMFLOAT2 cur = p ? *p : XMFLOAT2{ 0,0 };
        float arr[2] = { cur.x, cur.y }; // XMFLOAT2 멤버 가정
        if (ImGui::DragFloat2("##v2", arr, 0.01f))
            v = XMFLOAT2{ arr[0], arr[1] };
    } break;
    case SLOT_DATA_TYPE::Float3:
    {
        auto* p = std::get_if<XMFLOAT3>(&v);
        XMFLOAT3 cur = p ? *p : XMFLOAT3{ 0,0,0 };
        float arr[3] = { cur.x, cur.y, cur.z }; // XMFLOAT3 멤버 가정
        if (ImGui::DragFloat3("##v3", arr, 0.01f))
            v = XMFLOAT3{ arr[0], arr[1], arr[2] };
    } break;
    case SLOT_DATA_TYPE::Float4:
    {
        auto* p = std::get_if<XMFLOAT4>(&v);
        XMFLOAT4 cur = p ? *p : XMFLOAT4{ 0,0,0,0 };
        float arr[4] = { cur.x, cur.y, cur.z, cur.w }; // XMFLOAT3 멤버 가정
        if (ImGui::DragFloat4("##v4", arr, 0.01f))
            v = XMFLOAT4{ arr[0], arr[1], arr[2], arr[3] };
    } break;

    default:
        ImGui::TextUnformatted("-");
        break;
    }
}

void CSlotFieldGui::DrawLeftPanel()
{
    ImGui::TextUnformatted("Custom Tab");
    
    // (2-1) Prefab 선택 + Add
    if (!m_FieldPrefabDefs.empty())
    {
        const char* preview = m_FieldPrefabDefs[m_iSelectedPrefabIndex].TagName.c_str();
        if (ImGui::BeginCombo("Prefab", preview))
        {
            for (int i = 0; i < (int)m_FieldPrefabDefs.size(); ++i)
            {
                bool sel = (i == m_iSelectedPrefabIndex);
                if (ImGui::Selectable(m_FieldPrefabDefs[i].TagName.c_str(), sel))
                    m_iSelectedPrefabIndex = i;
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("+"))
        {
            m_DraftTabs.push_back(MakeFromPrefab(m_FieldPrefabDefs[m_iSelectedPrefabIndex]));
        }
    }
    else
    {
        //ImGui::TextUnformatted("No Prefab defs.");
        if (ImGui::Button("+"))
        {
            // 프리팹이 없어도 테스트용으로 하나 추가
            FIELD_DATA_DEFINE d;
            d.id = MakeNewID();
            d.TagName = "NewField";
            d.eDataType = SLOT_DATA_TYPE::Float;
            d.defaultvalue = MakeDefaultValue(d.eDataType);
            m_DraftTabs.push_back(d);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Apply"))
        m_isRequestApply = true;

    ImGui::Separator();

    // Draft 테이블: TagName / Type / Del
    ImGuiTableFlags flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_ScrollY;

    const char* typeNames[] = { "Int","Float","Bool","String","Float2","Float3","Float4" };

    int deleteIdx = -1;

    if (ImGui::BeginTable("DraftTabs", 3, flags, ImVec2(0, 0)))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("TagName");
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 140.f);
        ImGui::TableSetupColumn("Del", ImGuiTableColumnFlags_WidthFixed, 40.f);
        ImGui::TableHeadersRow();

        for (int i = 0; i < (int)m_DraftTabs.size(); ++i)
        {
            auto& d = m_DraftTabs[i];

            ImGui::TableNextRow();
            ImGui::PushID(d.id.c_str());

            ImGui::TableSetColumnIndex(0);
            ImGui::InputText("##TagName", &d.TagName);

            ImGui::TableSetColumnIndex(1);
            int t = (int)d.eDataType;
            if (t < 0 || t >= (int)SLOT_DATA_TYPE::END) t = 0;
            if (ImGui::Combo("##Type", &t, typeNames, IM_ARRAYSIZE(typeNames)))
            {
                d.eDataType = (SLOT_DATA_TYPE)t;
                d.defaultvalue = MakeDefaultValue(d.eDataType);

                if (d.eDataType != SLOT_DATA_TYPE::Float)
                {
                    d.hasRange = FromBool(false);
                    d.minV = 0.0;
                    d.maxV = 1.0;
                }
            }

            ImGui::TableSetColumnIndex(2);
            if (ImGui::SmallButton("X"))
                deleteIdx = i;

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    if (deleteIdx >= 0)
        m_DraftTabs.erase(m_DraftTabs.begin() + deleteIdx);
}

void CSlotFieldGui::DrawRightPanel()
{
    ImGui::TextUnformatted("Object Value Input Slot");

    ImGui::SameLine(0.f, 20.f);

    if (ImGui::Button("Load Base Data"))
        LoadBaseData();

    ImGui::SameLine(0.f, 20.f);
    ImGui::Text("DataFormat : %s", m_LoadedData.tagDataFormat.c_str());
    
    ImGui::SameLine(0.f, 20.f);
    ImGui::Text("Loaded Objects: %d", (int)m_LoadedData.LoadedObjects.size());
    


    // (1-1) 로드된 맵 오브젝트가 B의 기본 행
    ImGui::Text("Loaded Area : %s", m_pMapToolContext->TagArea.c_str());



    ImGui::SameLine(0.f, 50.f);

    ImGui::SetNextItemWidth(80.0f);
    ImGui::InputText("Format Name", &m_TagSlotFormat);

    if (m_TagSlotFormat.empty())
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 pmin = ImGui::GetItemRectMin();
        ImVec2 pmax = ImGui::GetItemRectMax();
        dl->AddRect(pmin, pmax, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
    }

    ImGui::SameLine(0.f, 10.f);
    if (ImGui::Button("Save Json")) {
        if (false == m_TagSlotFormat.empty())
            m_isRequestSave = true;
    }
    if (m_isShowSaveFinish) {
        ImGui::SameLine(0.f, 20.f);
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Save Json Success!");
    }


    ImGui::Separator();

    // Apply 전에는 “컬럼(입력칸)”을 늘리지 않음
    if (m_AppliedTabs.empty())
    {
        ImGui::TextUnformatted("No Applied Tabs. (Use Apply in Custom Tab <<<)");
        // 그래도 오브젝트 리스트만 보여주고 싶으면 간단 테이블
        if (ImGui::BeginTable("ObjOnly", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("ObjectIndex", ImGuiTableColumnFlags_WidthFixed, 80.f);
            ImGui::TableSetupColumn("Name");
            ImGui::TableHeadersRow();
            for (auto& o : m_LoadedData.LoadedObjects)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", o.iObjIdx);
                ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(o.TagModelKey.c_str());
            }
            ImGui::EndTable();
        }
        return;
    }

    // Apply 후: 동적 컬럼(=DataTab)
    const int colCount = 2 + (int)m_AppliedTabs.size();

    ImGuiTableFlags flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_ScrollX |
        ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable("ObjValues", colCount, flags, ImVec2(0, 0)))
    {
        ImGui::TableSetupScrollFreeze(2, 1); // ObjectNo/PrefabKey + 헤더 고정
        ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_WidthFixed, 40.f);
        ImGui::TableSetupColumn("ModelKey", ImGuiTableColumnFlags_WidthFixed, 140.f);

        for (auto& tab : m_AppliedTabs)
            ImGui::TableSetupColumn(tab.TagName.c_str(), ImGuiTableColumnFlags_WidthFixed, 150.f);

        ImGui::TableHeadersRow();

        for (int objIdx = 0; objIdx < (int)m_LoadedData.LoadedObjects.size(); ++objIdx)
        {
            const auto& o = m_LoadedData.LoadedObjects[objIdx];

            ImGui::TableNextRow();
            ImGui::PushID(objIdx);

            // 기본 정보
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", o.iObjIdx);

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(o.TagModelKey.c_str());

            // 탭 컬럼들
            for (int t = 0; t < (int)m_AppliedTabs.size(); ++t)
            {
                const auto& tab = m_AppliedTabs[t];
                ImGui::TableSetColumnIndex(2 + t);
                ImGui::PushID(tab.id.c_str());

                auto& mp = m_ObjUserValue[objIdx];
                auto it = mp.find(tab.id);

                if (it == mp.end())
                {
                    // 값 미입력 상태
                    if (ImGui::SmallButton("Set"))
                    {
                        SlotValue& v = EnsureValue(objIdx, tab);
                        (void)v;
                    }
                    ImGui::SameLine();
                    ImGui::TextUnformatted("-");
                }
                else
                {
                    // 값 입력 상태
                    DrawValueEditor(tab, it->second);
                    ImGui::SameLine();
                    if (ImGui::SmallButton("Clear"))
                    {
                        mp.erase(it);
                    }
                }

                ImGui::PopID();
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

}

CSlotFieldGui* CSlotFieldGui::Create(GUI_CONTEXT* pContext)
{
    CSlotFieldGui* pInstance = new CSlotFieldGui(pContext);
    if (FAILED(pInstance->Initialize())) {
        MSG_BOX("SlotFieldGui Create Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSlotFieldGui::Free()
{
    __super::Free();

    //Safe_Release(m_pMapToolCore);
    Safe_Release(m_pGameInstance);
}
