#include "pch.h"
#include "BattleSystem_Panel.h"
#include "BattleSystem.h"
#include "GameInstance.h"
#include "BattleFXFlow.h"
#include "Engine_Math.h"
#include "Layer.h"
#ifdef _USING_GUI

NS_BEGIN(Client)

static BATTLE_OBJ_TYPE ToBattleType(int index)
{
    return static_cast<BATTLE_OBJ_TYPE>(index);
}

CBattleSystem_Panel::CBattleSystem_Panel(GUI_CONTEXT* context)
    : CBasePanel(context)
{
}

const char* CBattleSystem_Panel::TypeName(BATTLE_OBJ_TYPE type) const
{
    switch (type)
    {
    case BATTLE_OBJ_TYPE::PLAYER:  return "PLAYER";
    case BATTLE_OBJ_TYPE::MONSTER: return "MONSTER";
    case BATTLE_OBJ_TYPE::CAMERA:  return "CAMERA";
    case BATTLE_OBJ_TYPE::EFFECT:  return "EFFECT";
    default: return "UNKNOWN";
    }
}

void CBattleSystem_Panel::Render_GUI()
{
    if (!BattleSystem()->Debug_IsActive())
        return;
    if (!ImGui::Begin("BattleSystem##DebugPanel"))
    {
        ImGui::End();
        return;
    }

    DrawHeader();

    if (ImGui::BeginTabBar("##BattleSystemTabs"))
    {
        if (ImGui::BeginTabItem("Overview"))
        {
            DrawTab_Overview();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Objects"))
        {
            DrawTab_Objects();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Index Audit"))
        {
            DrawTab_IndexAudit();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Tools"))
        {
            DrawTab_Tools();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void CBattleSystem_Panel::Update_Panel(_float dt)
{
    (void)dt;
}

void CBattleSystem_Panel::DrawHeader()
{
    auto* bs = BattleSystem();
    if (!bs)
    {
        ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "BattleSystem nullptr");
        return;
    }

    ImGui::SeparatorText("Runtime");

    ImGui::Text("Active: %s", bs->Debug_IsActive() ? "TRUE" : "FALSE");

    auto* fx = bs->Debug_GetFXFlow();
    ImGui::Text("FXFlow: %s", (fx && fx->IsRunning()) ? "RUNNING" : "idle");
    ImGui::Checkbox("Auto Refresh", &m_autoRefresh);
    ImGui::SameLine();

    ImGui::Separator();
}

void CBattleSystem_Panel::DrawTab_Overview()
{
    auto* bs = BattleSystem();
    const auto& infos = bs->Debug_GetInfos();
    const auto& snaps = bs->Debug_GetSnapshots();
    const auto& indexMap = BattleSystem()->Debug_GetIndexMap();
    const auto& layerArray = bs->Get_BattleLayer();
    ImGui::SeparatorText("Counts");

    // 타입별 집계
    for (int i = 0; i < (int)BATTLE_OBJ_TYPE::END; ++i)
    {
        auto type = ToBattleType(i);

        int infoCount = 0;
        int snapCount = 0;
        int onFieldCount = 0;
        int validCount = 0;

        auto itInfo = infos.find(type);
        if (itInfo != infos.end())
        {
            infoCount = (int)itInfo->second.size();
            for (const auto& e : itInfo->second)
            {
                if (e.isOnField) ++onFieldCount;
                if (e.hObject.isValid()) ++validCount;
            }
        }

        auto itSnap = snaps.find(type);
        if (itSnap != snaps.end())
            snapCount = (int)itSnap->second.size();

        ImGui::Text("%s  infos:%d  snap:%d  onField:%d  valid:%d",
            TypeName(type), infoCount, snapCount, onFieldCount, validCount);
    }

    ImGui::SeparatorText("IndexMap");
    ImGui::Text("m_BattleObjIndex size: %d", (int)indexMap.size());

    const string nowLevelKey = LevelManager()->Get_NowLevelKey();

    for (const string& layerKey : layerArray)
    {
        CLayer* layerPtr = ObjectManager()->Get_Layer({ nowLevelKey, layerKey });
        if (layerPtr)
        {
            const _float timeScaleValue = layerPtr->Get_TimeScale();
            ImGui::Text("%s : TimeScale: %.5f", layerKey.c_str(), (double)timeScaleValue);
        }
        else
        {
            ImGui::TextColored(ImVec4(1.f, 0.3f, 0.3f, 1.f), "%s : (missing)", layerKey.c_str());
        }
    }
}

void CBattleSystem_Panel::DrawTab_Objects()
{
    auto* bs = BattleSystem();
    const auto& snaps = bs->Debug_GetSnapshots();

    ImGui::SeparatorText("Filter");
    ImGui::Checkbox("Only OnField", &m_onlyOnField);
    ImGui::SameLine();
    ImGui::Checkbox("Only Valid", &m_onlyValid);

    // 타입 선택
    const char* typeItems[] = { "PLAYER", "MONSTER", "CAMERA", "EFFECT" };
    ImGui::Combo("Type", &m_selectedType, typeItems, IM_ARRAYSIZE(typeItems));

    const auto type = ToBattleType(m_selectedType);

    auto it = snaps.find(type);
    if (it == snaps.end())
    {
        ImGui::Text("No snapshot for %s", TypeName(type));
        return;
    }

    const auto& list = it->second;

    ImGui::SeparatorText("Snapshot List");
    ImGui::BeginChild("##BattleSnapshotList", ImVec2(0, 260), true);

    // 헤더
    ImGui::Columns(5, "##cols");
    ImGui::Text("Idx"); ImGui::NextColumn();
    ImGui::Text("OnField"); ImGui::NextColumn();
    ImGui::Text("Valid"); ImGui::NextColumn();
    ImGui::Text("Name"); ImGui::NextColumn();
    ImGui::Text("Pos/Radius"); ImGui::NextColumn();
    ImGui::Separator();

    int shown = 0;
    for (int i = 0; i < (int)list.size(); ++i)
    {
        const auto& e = list[i];
        if (m_onlyOnField && !e.isOnField) continue;
        if (m_onlyValid && !e.hObject.isValid()) continue;

        ImGui::Text("%d", i); ImGui::NextColumn();
        ImGui::Text(e.isOnField ? "Y" : "N"); ImGui::NextColumn();
        ImGui::Text(e.hObject.isValid() ? "Y" : "N"); ImGui::NextColumn();

        bool selected = (m_selectedRow == i);
        if (ImGui::Selectable(e.TagInstanceName.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns))
            m_selectedRow = i;
        ImGui::NextColumn();

        ImGui::Text("(%.2f, %.2f, %.2f) r=%.2f",
            e.vPos.x, e.vPos.y, e.vPos.z, e.fRadius);
        ImGui::NextColumn();

        ++shown;
    }

    ImGui::Columns(1);
    ImGui::EndChild();

    ImGui::Text("shown: %d / total: %d", shown, (int)list.size());

    // 선택된 항목 상세
    if (m_selectedRow >= 0 && m_selectedRow < (int)list.size())
    {
        const auto& e = list[m_selectedRow];

        ImGui::SeparatorText("Selected");
        ImGui::Text("Name: %s", e.TagInstanceName.c_str());
        ImGui::Text("OnField: %s", e.isOnField ? "TRUE" : "FALSE");
        ImGui::Text("Valid: %s", e.hObject.isValid() ? "TRUE" : "FALSE");
        ImGui::Text("Pos: (%.3f, %.3f, %.3f)", e.vPos.x, e.vPos.y, e.vPos.z);
        ImGui::Text("Radius: %.3f", e.fRadius);

        if (e.hObject.isValid())
        {
            if (ImGui::Button("Exclude (OnField=false)"))
                bs->ExcludeBattleObject(type, e.hObject);

            ImGui::SameLine();
            if (ImGui::Button("Exit (remove)"))
                bs->ExitBattleObject(type, e.hObject);
        }
    }
}

void CBattleSystem_Panel::DrawTab_IndexAudit()
{
    auto* bs = BattleSystem();
    const auto& infos = bs->Debug_GetInfos();
    const auto& indexMap = bs->Debug_GetIndexMap();

    ImGui::SeparatorText("IndexMap Integrity");

    int errors = 0;
    int checked = 0;

    ImGui::BeginChild("##IndexAuditScroll", ImVec2(0, 360), true);

    for (const auto& pair : indexMap)
    {
        const OBJECT_HANDLE& handle = pair.first;
        const auto& idx = pair.second;

        ++checked;

        auto itVec = infos.find(idx.objType);
        if (itVec == infos.end())
        {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1),
                "ERR: map entry has invalid objType (no vector).");
            ++errors;
            continue;
        }

        const auto& vec = itVec->second;
        if (idx.indexInVector >= vec.size())
        {
            ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1),
                "ERR: out of range type=%s idx=%u vecSize=%d",
                TypeName(idx.objType), idx.indexInVector, (int)vec.size());
            ++errors;
            continue;
        }

        const auto& info = vec[idx.indexInVector];
        if (!(info.hObject == handle))
        {
            ImGui::TextColored(ImVec4(1, 0.5f, 0.2f, 1),
                "WARN: handle mismatch type=%s idx=%u vecHandleValid=%s",
                TypeName(idx.objType), idx.indexInVector,
                info.hObject.isValid() ? "Y" : "N");
            ++errors;
        }
    }

    ImGui::EndChild();

    ImGui::Text("checked: %d   issues: %d", checked, errors);

    ImGui::SeparatorText("Notes");
    ImGui::BulletText("SwapPop에서 movedHandle index 갱신은 되어있음.");
    ImGui::BulletText("Exclude는 indexMap 유지 + isOnField만 false.");
    ImGui::BulletText("isValid() false인 핸들은 Cleanup에서 snapshot에 안 들어감.");
}

void CBattleSystem_Panel::DrawTab_Tools()
{
    auto* bs = BattleSystem();

    ImGui::SeparatorText("Stage / Active");
    if (ImGui::Button("SetActive(TRUE)")) bs->SetActive(true);
    ImGui::SameLine();
    if (ImGui::Button("SetActive(FALSE)")) bs->SetActive(false);

    if (ImGui::Button("ClearBattleStage()")) bs->ClearBattleStage();
    ImGui::SameLine();
    if (ImGui::Button("AllKill")) bs->AllKill();

    ImGui::SeparatorText("Spawn Monster");
    ImGui::InputText("ProtoTag", m_spawnProto, (int)sizeof(m_spawnProto));
    ImGui::DragFloat3("SpawnPos", m_spawnPos, 0.1f);
    ImGui::DragFloat3("SpawnRot", m_spawnRot, 0.5f);

    if (ImGui::Button("Spawn"))
    {
        bs->SpawnMosnter(m_spawnProto,
            _float3(m_spawnPos[0], m_spawnPos[1], m_spawnPos[2]),
            _float3(m_spawnRot[0], m_spawnRot[1], m_spawnRot[2]));
    }

    ImGui::SeparatorText("VFX");
    if (ImGui::Button("Start EVADE")) bs->StartGimmick(BATTLE_VFX_TYPE::EVADE);
    ImGui::SameLine();
    if (ImGui::Button("Start PARRY")) bs->StartGimmick(BATTLE_VFX_TYPE::PARRY);
    ImGui::SameLine();
    if (ImGui::Button("Hit NORMAL")) bs->HitVFX(DAMAGE_TYPE::NORMAL);
    ImGui::SameLine();
    if (ImGui::Button("Hit HARD")) bs->HitVFX(DAMAGE_TYPE::HARD);

    ImGui::SeparatorText("Damage Test (Snapshot MONSTER)");
    const char* dmgItems[] = { "NORMAL", "HARD", "AIRBORNE", "ULTIMATE" };
    ImGui::Combo("DamageType", &m_damageType, dmgItems, IM_ARRAYSIZE(dmgItems));
    ImGui::DragFloat("Damage", &m_damage, 1.f, 0.f, 999999.f);

    HitDesc hit{};
    hit.eDamageType = (DAMAGE_TYPE)m_damageType;
    hit.fDamage = (_float)m_damage;
    // hit.eName은 너 프로젝트에 따라 채워야 해서 여기선 생략/기본값으로 둠.

    if (ImGui::Button("TakeAllDamage"))
        bs->TakeAllDamage(hit);

    ImGui::SeparatorText("Area Damage");
    ImGui::DragFloat3("Center##AOE", m_aoeCenter, 0.1f);
    ImGui::DragFloat("Radius##AOE", &m_aoeRadius, 0.1f, 0.f, 999.f);

    if (ImGui::Button("TakeAreaDamage (Sphere)"))
        bs->TakeAreaDamage(_float3(m_aoeCenter[0], m_aoeCenter[1], m_aoeCenter[2]), (_float)m_aoeRadius, hit);

    ImGui::SeparatorText("Cone Damage");
    ImGui::DragFloat3("Dir##Cone", m_coneDir, 0.05f);
    ImGui::DragFloat("Angle##Cone", &m_coneAngle, 1.f, 1.f, 179.f);

    if (ImGui::Button("TakeAreaDamage (Cone)"))
        bs->TakeAreaDamage(
            _float3(m_aoeCenter[0], m_aoeCenter[1], m_aoeCenter[2]),
            (_float)m_aoeRadius,
            _float3(m_coneDir[0], m_coneDir[1], m_coneDir[2]),
            (_float)m_coneAngle,
            hit);

    ImGui::SeparatorText("Box Damage");
    ImGui::DragFloat3("Center##Box", m_boxCenter, 0.1f);
    ImGui::DragFloat3("HalfExtents##Box", m_boxHalf, 0.1f, 0.f, 999.f);
    ImGui::DragFloat3("RotEuler##Box", m_boxRotEuler, 0.5f);

    // Euler -> quaternion (Y-up 기준 가정)
    _quaternion qRot =
        _quaternion::CreateFromYawPitchRoll(
            XMConvertToRadians(m_boxRotEuler[1]),
            XMConvertToRadians(m_boxRotEuler[0]),
            XMConvertToRadians(m_boxRotEuler[2]));

    if (ImGui::Button("TakeBoxDamage"))
        bs->TakeBoxDamage(
            _float3(m_boxCenter[0], m_boxCenter[1], m_boxCenter[2]),
            _float3(m_boxHalf[0], m_boxHalf[1], m_boxHalf[2]),
            qRot,
            hit);
}

CBattleSystem_Panel* CBattleSystem_Panel::Create(GUI_CONTEXT* context)
{
    CBattleSystem_Panel* instance = new CBattleSystem_Panel(context);
    if (FAILED(instance->Initialize()))
    {
        Safe_Release(instance);
        return nullptr;
    }
    return instance;
}

void CBattleSystem_Panel::Free()
{
    __super::Free();
}

NS_END

#endif // _USING_GUI