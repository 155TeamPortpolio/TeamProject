#pragma once

#include "DefilerState_Born.h"
#include "DefilerState_Idle.h"
#include "DefilerState_Attack.h"
#include "DefilerState_Other.h"
NS_BEGIN(Client)
/*상태간 타겟 추적 모드*/
enum class TraceFlag : _uint
{
	None = 0,
	TrackTarget = 1 << 0,  // 타겟 방향으로 이동/회전
	StopAtTarget = 1 << 1,  // 타겟 도달 시 정지
	AllowThroughTarget = 1 << 2,  // 타겟 지나침 가능
	IgnoreTarget = 1 << 3,  // 타겟 존재 무시 (연출/패턴 고정)
	IgnoreRotation = 1 << 4,
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
	_int patternIndex = { 00 };
	struct DefilerPattern { string nextPattern; _float animStartProgress; _float animEndProgress; };
	deque<DefilerPattern> patternTransition;
	DefilerPattern reservedPattern = {};

	/*추적 상태 */
	_bool CompareTrace(TraceFlag flag) { return HasFlag(eTraceFlag, flag); }
	void TraceType_Fierce() { ResetTraceFlag(); eTraceFlag = TraceFlag::TrackTarget | TraceFlag::AllowThroughTarget; };
	void TraceType_OnTarget() { ResetTraceFlag(); eTraceFlag = TraceFlag::TrackTarget | TraceFlag::StopAtTarget; };
	void TraceType_OnlyAnim() { ResetTraceFlag(); eTraceFlag = TraceFlag::IgnoreTarget; };
	void TraceType_IgnoreRotation() { eTraceFlag += TraceFlag::IgnoreRotation; };

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

        ImGui::SeparatorText("Summary");
        ImGui::Text("patternIndex: %d", bb.patternIndex);
        DrawBoolBadge("isRequestNext", bb.isRequestNext);
        ImGui::SameLine();
        DrawBoolBadge("isChainOpen", bb.isChainOpen);

        DrawBoolBadge("LockTarget", bb.LockTarget);
        ImGui::SameLine();
        DrawBoolBadge("LockRotate", bb.LockRotate);

        ImGui::Text("TraceFlag: %s", TraceFlagToText(bb.eTraceFlag));

        if (ImGui::TreeNode("Targeting"))
        {
            DrawVec3("vTargetPos", bb.vTargetPos);
            DrawVec3("vTargetDir", bb.vTargetDir);
            DrawFloat3("vStartPos", bb.vStartPos);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Movement / Trace"))
        {
            DrawVec3("CurrentDir", bb.CurrentDir);
            ImGui::Text("TraceFlag: %s", TraceFlagToText(bb.eTraceFlag));
            ImGui::Separator();

            if (ImGui::Button("TraceType_Fierce")) bb.TraceType_Fierce();
            ImGui::SameLine();
            if (ImGui::Button("TraceType_OnTarget")) bb.TraceType_OnTarget();
            ImGui::SameLine();
            if (ImGui::Button("TraceType_OnlyAnim")) bb.TraceType_OnlyAnim();
            ImGui::SameLine();
            if (ImGui::Button("Add IgnoreRotation")) bb.TraceType_IgnoreRotation();

            if (ImGui::Button("ResetTraceFlag")) bb.ResetTraceFlag();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Transition Flags"))
        {
            DrawBoolBadge("isRequestNext", bb.isRequestNext);
            DrawBoolBadge("isChainOpen", bb.isChainOpen);

            if (ImGui::Button("ResetNextFlag"))
                bb.ResetNextFlag();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Pattern Queue"))
        {
            ImGui::Text("queue size: %d", (int)bb.patternTransition.size());

            ImGui::SeparatorText("ReservedPattern");
            ImGui::Text("next: %s", bb.reservedPattern.nextPattern.empty() ? "(empty)" : bb.reservedPattern.nextPattern.c_str());
            ImGui::Text("start: %.3f", bb.reservedPattern.animStartProgress);
            ImGui::Text("end:   %.3f", bb.reservedPattern.animEndProgress);

            ImGui::SeparatorText("Queue");
            ImGui::BeginChild("##DefilerPatternQueue", ImVec2(0, 170), true);

            int index = 0;
            for (auto& p : bb.patternTransition)
            {
                ImGui::PushID(index);
                ImGui::Text("[%02d] %s", index, p.nextPattern.c_str());
                ImGui::SameLine(240.f);
                ImGui::Text("start %.2f", p.animStartProgress);
                ImGui::SameLine(360.f);
                ImGui::Text("end %.2f", p.animEndProgress);
                ImGui::PopID();
                ++index;
            }
            ImGui::EndChild();

            if (ImGui::Button("ReservePattern (pop front)") && !bb.patternTransition.empty())
                bb.ReservePattern();

            ImGui::TreePop();
        }
    }
}

NS_END
#endif