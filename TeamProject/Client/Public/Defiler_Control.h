#pragma once

#include "DefilerState_Born.h"
#include "DefilerState_Idle.h"
#include "DefilerState_Attack.h"
#include "DefilerState_Other.h"

#include "GameInstance.h"
#include "Material.h"
#include "Texture.h"

NS_BEGIN(Client)
/*실제 위치 계산*/
enum class FOUR_DIR { FRONT, LEFT, RIGHT, BACK };
/*상태간 타겟 추적 모드*/
enum class TraceFlag : _uint
{
	None = 0,
	TrackTarget = 1 << 0,  // 타겟 방향으로 이동/회전
	StopAtTarget = 1 << 1,  // 타겟 도달 시 정지
	AllowThroughTarget = 1 << 2,  // 타겟 지나침 가능
	IgnoreTarget = 1 << 3,  // 타겟 존재 무시 (연출/패턴 고정)
	IgnoreRotation = 1 << 4,
	IgnoreAnim = 1 << 5,
};

inline TraceFlag operator|(TraceFlag left, TraceFlag right)
{
	return static_cast<TraceFlag>(
		static_cast<_uint>(left) | static_cast<_uint>(right));
}

inline TraceFlag operator&(TraceFlag left, TraceFlag right)
{
	return static_cast<TraceFlag>(
		static_cast<_uint>(left) & static_cast<_uint>(right));
}

inline TraceFlag operator^(TraceFlag left, TraceFlag right)
{
	return static_cast<TraceFlag>(
		static_cast<_uint>(left) ^ static_cast<_uint>(right));
}

inline TraceFlag operator~(TraceFlag value)
{
	return static_cast<TraceFlag>(~static_cast<_uint>(value));
}

inline TraceFlag& operator|=(TraceFlag& left, TraceFlag right)
{
	left = (left | right);
	return left;
}

inline TraceFlag& operator&=(TraceFlag& left, TraceFlag right)
{
	left = (left & right);
	return left;
}

inline TraceFlag& operator^=(TraceFlag& left, TraceFlag right)
{
	left = (left ^ right);
	return left;
}

inline TraceFlag operator+(TraceFlag left, TraceFlag right)
{
	return (left | right); // add flags
}

inline TraceFlag& operator+=(TraceFlag& left, TraceFlag right)
{
	left |= right;
	return left;
}

inline TraceFlag operator-(TraceFlag left, TraceFlag right)
{
	return static_cast<TraceFlag>(
		static_cast<_uint>(left) & ~static_cast<_uint>(right)); // remove flags
}

inline TraceFlag& operator-=(TraceFlag& left, TraceFlag right)
{
	left = (left - right);
	return left;
}

inline bool HasFlag(TraceFlag value, TraceFlag flag)
{
	return (static_cast<_uint>(value) & static_cast<_uint>(flag)) != 0;
}

inline void AddFlag(TraceFlag& value, TraceFlag flag)
{
	value |= flag;
}

inline void RemoveFlag(TraceFlag& value, TraceFlag flag)
{
	value = static_cast<TraceFlag>(
		static_cast<_uint>(value) & ~static_cast<_uint>(flag));
}

typedef struct tagDefilerBlackBoard
{
    _bool MiasmaPhase = {};
	_vector3 vTargetPos = {};
	_vector3 vTargetDir = {};
	
	_float3 vStartPos = {};
	_bool isRequestNext = false;//다음 상태가 존재 할 때 상태 전환 요청
	_bool isChainOpen = false;  //현재 상태에서 다음으로 진행 가능여부

	/*움직임*/
	TraceFlag eTraceFlag = {};
	_bool LockTarget = {};
	_bool LockRotate = {};
	_vector3 CurrentDir = _vector3(0.f, 0.f, 1.f);

	/*패턴*/
	_int patternIndex = { 0 };
    struct DefilerPattern { string nextPattern{}; _float animStartProgress{}; _float animEndProgress{}; };
	deque<DefilerPattern> patternTransition;
	DefilerPattern reservedPattern = {};

	/*추적 상태 */
	_bool CompareTrace(TraceFlag flag) { return HasFlag(eTraceFlag, flag); }
	void TraceType_Fierce() { ResetTraceFlag(); eTraceFlag = TraceFlag::TrackTarget | TraceFlag::AllowThroughTarget; };
	void TraceType_OnTarget() { ResetTraceFlag(); eTraceFlag = TraceFlag::TrackTarget | TraceFlag::StopAtTarget; };
	void TraceType_OnlyAnim() { ResetTraceFlag(); eTraceFlag = TraceFlag::IgnoreTarget; };
	void TraceType_IgnoreRotation() { eTraceFlag += TraceFlag::IgnoreRotation; };
    void TractType_IgnoreAnim() { eTraceFlag += TraceFlag::IgnoreAnim; };

	/*전환 상태*/
	void ReservePattern() {
		reservedPattern = patternTransition.front();
		patternTransition.pop_front();
	}
	void ResetNextFlag() {
		isChainOpen = false;
		isRequestNext = false;
	}
	void ResetTraceFlag() {
		eTraceFlag = TraceFlag::None;
	}

}DEFILER_BLACK_BOARD;

struct DefilerAttackType {
	string AtkBone = { "Weapon" };
	string AtkEvade = { "Evade" };
	string AtkPower = { "Hard" };
	_bool  OnOff = { false };
};

static const unordered_map<string, DefilerAttackType> DefilerAtkData =
{
	{"A_Type_On",{"Weapon","Parry","Normal",true}}  ,{"A_Type_Off",{"Weapon","Parry","Normal",false}},
	{"B_Type_On",{"Weapon","Evade","Normal",true}}  ,{"B_Type_Off",{"Weapon","Evade","Normal",false}},
	{"C_Type_On",{"Weapon","Parry","Hard",true}}    ,{"C_Type_Off",{"Weapon","Parry","Hard",false}},
	{"D_Type_On",{"Weapon","Evade","Hard",true}}    ,{"D_Type_Off",{"Weapon","Evade","Hard",false}},
	{"E_Type_On",{"Tail","Parry","Normal",true}}    ,{"E_Type_Off",{"Tail","Parry","Normal",false}},
	{"F_Type_On",{"Tail","Evade","Normal",true}}    ,{"F_Type_Off",{"Tail","Evade","Normal",false}},
	{"G_Type_On",{"Tail","Parry","Hard",true}}      ,{"G_Type_Off",{"Tail","Parry","Hard",false}},
	{"H_Type_On",{"Tail","Evade","Hard",true}}      ,{"H_Type_Off",{"Tail","Evade","Hard",false}},
	{"I_Type_On",{"Area","Parry","Normal",true}}    ,{"I_Type_Off",{"Area","Parry","Normal",false}},
	{"J_Type_On",{"Area","Evade","Normal",true}}    ,{"J_Type_Off",{"Area","Evade","Normal",false}},
	{"K_Type_On",{"Area","Parry","Hard",true}}      ,{"K_Type_Off",{"Area","Parry","Hard",false}},
	{"L_Type_On",{"Area","Evade","Hard",true}}      ,{"L_Type_Off",{"Area","Evade","Hard",false}},
	{"M_Type_On",{"Ground","Evade","Normal",true}}  ,{"M_Type_Off",{"Ground","Evade","Normal",false}},
};

struct DefilerDissolve {
	enum DISSOLVE_STATE { DISAPPEAR, APPEAR, NONE, END };
	DISSOLVE_STATE eDissolveState = NONE;
	_float fDissolveDuration = 2.f;
	_float fDissolveElapsedTime = 0.f;
	_float fDissolveProgress = 0.f;

	void Set_DissolveState(DISSOLVE_STATE state, _float duration)
	{
		eDissolveState = state;
		fDissolveDuration = duration;
		fDissolveElapsedTime = 0.f;
		fDissolveProgress = 0.f;
	}
	void Appear(_float duration) { Set_DissolveState(DISSOLVE_STATE::APPEAR, duration); }
	void DisAppear(_float duration) { Set_DissolveState(DISSOLVE_STATE::DISAPPEAR, duration); }

	_bool isComplete() {
		return fDissolveElapsedTime >= fDissolveDuration;
	}
	_bool isComplete(DISSOLVE_STATE state) {
		return (eDissolveState == state && fDissolveElapsedTime >= fDissolveDuration);
	}
};


struct TsunamiWallDesc {
    _bool isCharcter_On_Wall = { false };
};
struct TsunamiDesc {
    _bool isEndTsunami = { false };
    _bool isHitGround = { false };
};


struct DefilerMaterialParam {
    _float3 vEmissiveColor= { 1.378, 0.039, 0.039 };
    _float  fEmissiveStrength{};
    _float  fRimLightPower={4.f};
    _float3 vRimLightColor={ 0.378, 0.029, 0.070 };
};

struct DefilerMaterialPreset {
    vector<DefilerMaterialParam> MaterialParams;
    void Initialize(class CMaterial* pMaterial) {
        auto& materialInstances = pMaterial->Get_MaterialInstances();
        _uint size = materialInstances.size();
        MaterialParams.resize(size);
        auto dissolveTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Dissolve.png");
        for (size_t i = 0; i < size; i++)
        {
            auto instance = materialInstances[i];
            instance->Set_Param("vRimLightColor",       { &MaterialParams[i].vRimLightColor         ,"float3",sizeof(_float3) });
            instance->Set_Param("fRimLightPower",       { &MaterialParams[i].fRimLightPower         ,"float",sizeof(_float) });
          }
    }
  
};

enum class ENVTYPE {
    REDSKY,BLACKSKY,SURGE
};

NS_END


#pragma once
#ifdef _USING_GUI
NS_BEGIN(Client)
namespace DefilerDebugGUI
{
    inline void AppendFlagText(char* buffer, int bufferSize, const char* name, bool enabled)
    {
        if (!enabled) return;
        if (buffer[0] != '\0') strncat_s(buffer, bufferSize, " | ", _TRUNCATE);
        strncat_s(buffer, bufferSize, name, _TRUNCATE);
    }

    inline const char* TraceFlagToText(TraceFlag flags)
    {
        static char buffer[256];
        buffer[0] = '\0';

        AppendFlagText(buffer, (int)sizeof(buffer), "None", flags == TraceFlag::None);
        AppendFlagText(buffer, (int)sizeof(buffer), "TrackTarget", HasFlag(flags, TraceFlag::TrackTarget));
        AppendFlagText(buffer, (int)sizeof(buffer), "StopAtTarget", HasFlag(flags, TraceFlag::StopAtTarget));
        AppendFlagText(buffer, (int)sizeof(buffer), "AllowThroughTarget", HasFlag(flags, TraceFlag::AllowThroughTarget));
        AppendFlagText(buffer, (int)sizeof(buffer), "IgnoreTarget", HasFlag(flags, TraceFlag::IgnoreTarget));
        AppendFlagText(buffer, (int)sizeof(buffer), "IgnoreRotation", HasFlag(flags, TraceFlag::IgnoreRotation));

        if (buffer[0] == '\0') strcpy_s(buffer, "None");
        return buffer;
    }

    inline void DrawBoolBadge(const char* label, bool value)
    {
        ImGui::TextUnformatted(label);
        ImGui::SameLine();
        ImGui::TextColored(
            value ? ImVec4(0.2f, 1.f, 0.2f, 1.f) : ImVec4(1.f, 0.3f, 0.3f, 1.f),
            value ? "TRUE" : "FALSE"
        );
    }

    inline void DrawVec3(const char* label, const _vector3& v)
    {
        ImGui::Text("%s: (%.3f, %.3f, %.3f)", label, v.x, v.y, v.z);
    }

    inline void DrawFloat3(const char* label, const _float3& v)
    {
        ImGui::Text("%s: (%.3f, %.3f, %.3f)", label, v.x, v.y, v.z);
    }

    inline void Render(DEFILER_BLACK_BOARD& bb)
    {
        if (!ImGui::CollapsingHeader("Defiler BlackBoard", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        // ... (위의 Summary / Targeting / Movement / Transition Flags는 그대로)

        if (ImGui::TreeNode("Pattern Queue"))
        {
            ImGui::Text("queue size: %d", (int)bb.patternTransition.size());

            // =========================
            // Reserved
            // =========================
            ImGui::SeparatorText("ReservedPattern");
            ImGui::Text("next: %s", bb.reservedPattern.nextPattern.empty() ? "(empty)" : bb.reservedPattern.nextPattern.c_str());
            ImGui::Text("start: %.3f", bb.reservedPattern.animStartProgress);
            ImGui::Text("end:   %.3f", bb.reservedPattern.animEndProgress);

            // =========================
            // Add (by text)
            // =========================
            ImGui::SeparatorText("Add Pattern");

            static char nextPatternBuf[128] = "Attack01_03";
            static float startProgress = 0.f;
            static float endProgress = 1.f;
            static bool pushFront = false;

            ImGui::InputText("nextPattern", nextPatternBuf, (int)sizeof(nextPatternBuf));
            ImGui::DragFloat("start", &startProgress, 0.01f, 0.f, 1.f, "%.2f");
            ImGui::DragFloat("end", &endProgress, 0.01f, 0.f, 1.f, "%.2f");
            if (endProgress < startProgress) endProgress = startProgress;

            ImGui::Checkbox("push front", &pushFront);
            ImGui::SameLine();
            if (ImGui::Button("Add"))
            {
                DEFILER_BLACK_BOARD::DefilerPattern newItem;
                newItem.nextPattern = nextPatternBuf;
                newItem.animStartProgress = startProgress;
                newItem.animEndProgress = endProgress;

                if (pushFront) bb.patternTransition.push_front(newItem);
                else          bb.patternTransition.push_back(newItem);
            }

            ImGui::SameLine();
            if (ImGui::Button("Clear Queue"))
            {
                bb.patternTransition.clear();
            }

            // =========================
            // Add (by number preset)
            // =========================
            ImGui::SeparatorText("Add By Index (Preset)");

            // "번호로도 괜찮아" => Build_Pattern 같은 룰을 GUI에서도 쓰려면
            // 여기서는 간단히 인덱스->패턴 시퀀스 매핑을 UI에서 생성해 push_back 해줌.
            static int presetIndex = 10; // 예: 10이면 Attack09 세트
            static bool presetAppend = true;

            ImGui::InputInt("presetIndex", &presetIndex);
            ImGui::Checkbox("append", &presetAppend);
            ImGui::SameLine();

            auto pushPreset = [&](int index)
                {
                    // presetAppend=false면 기존 큐를 날리고 시작
                    if (!presetAppend)
                        bb.patternTransition.clear();

                    auto push = [&](const char* name, float s, float e)
                        {
                            DEFILER_BLACK_BOARD::DefilerPattern it;
                            it.nextPattern = name;
                            it.animStartProgress = s;
                            it.animEndProgress = e;
                            bb.patternTransition.push_back(it);
                        };

                    switch (index)
                    {
                    case 0:
                        push("Attack01_01", 0.f, 0.41f);
                        push("Attack01_02", 0.19f, 1.f);
                        break;
                    case 1:
                        push("Attack01_03", 0.f, 1.f);
                        break;
                    case 2:
                        push("Attack01_01_P2", 0.f, 1.f);
                        break;
                    case 3:
                        push("Attack02", 0.f, 1.f);
                        break;
                    case 4:
                        push("Attack03", 0.f, 1.f);
                        break;
                    case 5:
                        push("Attack04", 0.f, 1.f);
                        break;
                    case 6:
                        push("Attack05", 0.f, 1.f);
                        break;
                    case 7:
                        push("Attack06", 0.f, 1.f);
                        break;
                    case 8:
                        push("Attack_Evade", 0.f, 1.f);
                        push("Attack07", 0.f, 1.f);
                        push("Attack03", 0.f, 1.f);
                        push("Attack01_01_P2", 0.f, 1.f);
                        break;
                    case 9:
                        push("Attack_Evade", 0.f, 1.f);
                        push("Attack08_01_Start", 0.f, 1.f);
                        push("Attack08_01_Loop", 0.f, 1.f);
                        push("Attack08_01_End", 0.f, 1.f);
                        break;
                    case 10:
                        push("Attack_Evade", 0.f, 1.f);
                        push("Attack09_Start", 0.f, 1.f);
                        push("Attack09_Loop", 0.f, 1.f);
                        push("Attack09_End", 0.f, 1.f);
                        push("Attack01_01_P2", 0.f, 1.f);
                        break;
                    case 11:
                        push("Attack_Grab", 0.f, 1.f);
                        break;
                    case 12:
                        push("Attack_Summon", 0.f, 1.f);
                        break;
                    default:
                        // 알 수 없는 인덱스면 아무 것도 안 넣음
                        break;
                    }
                };

            if (ImGui::Button("Push Preset"))
            {
                pushPreset(presetIndex);
            }

            // =========================
            // Queue list + edit
            // =========================
            ImGui::SeparatorText("Queue (Edit)");
            static int selectedIndex = -1;

            ImGui::BeginChild("##DefilerPatternQueue", ImVec2(0, 240), true);

            int index = 0;
            for (auto& p : bb.patternTransition)
            {
                ImGui::PushID(index);

                bool isSelected = (selectedIndex == index);
                char rowLabel[256];
                sprintf_s(rowLabel, "[%02d] %s", index, p.nextPattern.c_str());

                if (ImGui::Selectable(rowLabel, isSelected))
                    selectedIndex = index;

                ImGui::SameLine(260.f);
                ImGui::Text("s %.2f", p.animStartProgress);
                ImGui::SameLine(340.f);
                ImGui::Text("e %.2f", p.animEndProgress);

                ImGui::PopID();
                ++index;
            }

            ImGui::EndChild();

            // selected item edit controls
            if (selectedIndex >= 0 && selectedIndex < (int)bb.patternTransition.size())
            {
                ImGui::SeparatorText("Selected");

                // deque는 operator[] 가능
                auto& sel = bb.patternTransition[(size_t)selectedIndex];

                static char editNameBuf[128];
                // 선택 바뀔 때마다 버퍼 동기화
                // (ImGui input이 매 프레임 덮여쓰이면 입력이 안되므로, 필요할 때만 sync)
                static int lastSelected = -999;
                if (lastSelected != selectedIndex)
                {
                    strcpy_s(editNameBuf, sel.nextPattern.c_str());
                    lastSelected = selectedIndex;
                }

                ImGui::InputText("edit next", editNameBuf, (int)sizeof(editNameBuf));
                if (ImGui::IsItemDeactivatedAfterEdit())
                    sel.nextPattern = editNameBuf;

                ImGui::DragFloat("edit start", &sel.animStartProgress, 0.01f, 0.f, 1.f, "%.2f");
                ImGui::DragFloat("edit end", &sel.animEndProgress, 0.01f, 0.f, 1.f, "%.2f");
                if (sel.animEndProgress < sel.animStartProgress)
                    sel.animEndProgress = sel.animStartProgress;

                // buttons: move / delete
                if (ImGui::Button("Up") && selectedIndex > 0)
                {
                    std::swap(bb.patternTransition[(size_t)selectedIndex],
                        bb.patternTransition[(size_t)selectedIndex - 1]);
                    selectedIndex--;
                }
                ImGui::SameLine();
                if (ImGui::Button("Down") && selectedIndex + 1 < (int)bb.patternTransition.size())
                {
                    std::swap(bb.patternTransition[(size_t)selectedIndex],
                        bb.patternTransition[(size_t)selectedIndex + 1]);
                    selectedIndex++;
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete"))
                {
                    bb.patternTransition.erase(bb.patternTransition.begin() + selectedIndex);
                    if (selectedIndex >= (int)bb.patternTransition.size())
                        selectedIndex = (int)bb.patternTransition.size() - 1;
                }
            }

            // existing pop-front
            ImGui::Separator();
            if (ImGui::Button("ReservePattern (pop front)"))
            {
                if (!bb.patternTransition.empty())
                    bb.ReservePattern();
            }

            ImGui::TreePop();
        }
    }
}

NS_END
#endif