#pragma once
#include "Base.h"
#include "StateParameter.h"
#include "IHState.h"
#include "Animator3D.h"
#include "GameObject.h"


NS_BEGIN(Client)

template<typename Type>
class CStateMachine : public CBase
{
public:
	enum TRANSITION_CONDITION
	{
		CONDITION_NONE = 0,
		CONDITION_ANIMATION_END,
		CONDITION_TIME_GREATER,
		CONDITION_TIME_LESS,
		CONDITION_BOOL_TRUE,
		CONDITION_BOOL_FALSE,
		CONDITION_TRIGGER,
	};

	typedef struct TransitionCondition
	{
		TRANSITION_CONDITION eCondition = CONDITION_NONE;
		string strParameter = "";
		_float fTimer = 0.f;  // TIME_GREATER, TIME_LESS
	}CONDITION_INFO;

	typedef struct TransitionInfo
	{
		string strFromState = "";
		string strToState = "";
		vector<CONDITION_INFO> Conditions;	// 다중조건(AND)
		_float fExitTime = 1.f;
		_bool  bExit = false;
	}TRANSITION_INFO;

	typedef struct StateTransitionRecord
	{
		string strFromState;
		string strToState;
		_float fTimestamp = { 0.f };        // 게임 시간 기준
		_float fPrevStateTime = { 0.f };    // 이전 상태에서 머문 시간
		string strTriggerReason;  // 어떤 조건으로 전환되었는지
	}STATE_RECORD;

public:
	CStateMachine() DEFAULT;
	virtual ~CStateMachine();

public:
	HRESULT Initialize(Type* pOwner);
	void    Update(_float dt);
	void	Render_GUI();

public:
	HRESULT Register_State(const string& strState, IBaseState<Type>* pState);
	void    UnRegister_State(const string& strState);
	IBaseState<Type>* Get_State(const string& strState);

	HRESULT Register_Transition(const TRANSITION_INFO& transition);
	HRESULT Register_Transition(const string& strFrom, const string& strTo,
		const TRANSITION_CONDITION eCondition = CONDITION_NONE,
		const string& strParam = "", _float fTimer = 0.f);			// 단일 조건
	HRESULT Register_Transition(const string& strFrom, const string& strTo,
		const vector<CONDITION_INFO>& Conditions,
		_bool bExit = false, _float fExitTime = 1.f);				// 다중 조건
	HRESULT Register_AnyStateTransition(const string& strTo,
		TRANSITION_CONDITION eCondition, const string& strParam);	// 단일
	HRESULT Register_AnyStateTransition(const string& strTo,
		const vector<CONDITION_INFO>& Conditions);					// 다중

	void    Change_State(const string& strState);
	void    Set_DefaultState(const string& strState);

	void    Set_Float(const string& strParam, _float fValue);
	void    Set_Int(const string& strParam, _int iValue);
	void    Set_Bool(const string& strParam, _bool bValue);
	void    Set_Trigger(const string& strParam);
	void    Reset_Trigger(const string& strParam);

	_float  Get_Float(const string& strParam) const;
	_int    Get_Int(const string& strParam) const;
	_bool   Get_Bool(const string& strParam) const;
	_bool   Get_Trigger(const string& strParam) const;

	void	Set_ShowWindow(_bool bShow) { m_bShowWindow = bShow; }
	_bool	Get_ShowWindow() const { return m_bShowWindow; }

public:
	IBaseState<Type>* Get_CurrentState() const { return m_pCurrentState; }
	const string&	  Get_CurrentStateName() const { return m_strCurrentState; }
	const string&	  Get_PrevStateName() const { return m_strPrevState; }
	const string&	  Get_DefaultStateName() const { return m_strDefaultState; }
	_float			  Get_StateTime() const { return m_fStateTime; }
	const unordered_map<string, IBaseState<Type>*>& Get_States() const { return m_States; }

private:
	void    Update_AnimProgress();
	void    Check_Transitions();
	void    Check_AnyStateTransitions();
	_bool   Check_Transition(const TRANSITION_INFO& transition);

	void	Render_Info();
	void	Render_Animation();
	void	Render_Parameters();
	void	Render_Transition();
	void	Render_History();
	void	Render_StateGraph();
	void	Render_Hierarchy(IBaseState<Type>* pState, _uint iDepth = 0);
	void	Draw_Arrow(ImDrawList* pDrawList, ImVec2 vFrom, ImVec2 vTo, ImU32 color);

	string	Get_Condition(const TRANSITION_INFO& transition);
	_bool	Evaluate_SingleCondition(const CONDITION_INFO& condition);
	_bool	Evaluate_Condition(const TRANSITION_INFO& transition);
	void	Record_Transition(const string& strFrom, const string& strTo, const string& strReason, _float fPrevStateTime);

private:
	Type*									 m_pOwner = { nullptr };
	unordered_map<string, IBaseState<Type>*> m_States;
	unordered_map<string, CStateParameter>   m_Parameters;
	vector<TRANSITION_INFO>                  m_Transitions;
	vector<TRANSITION_INFO>                  m_AnyStateTransitions;
	IBaseState<Type>*						 m_pCurrentState = { nullptr };
	string									 m_strPrevState = "";
	string                                   m_strCurrentState = "";
	string                                   m_strDefaultState = "";
	_float                                   m_fStateTime = 0.f;
	// Render_GUI
	deque<STATE_RECORD>						 m_History = {};
	_uint									 m_iMaxHistory = 20;
	_float									 m_fTotalTime = 0.f;
	_bool									 m_bShowWindow = false;

public:
	static CStateMachine<Type>* Create() { return new CStateMachine<Type>(); }
	void Free() override;
};

#pragma region IMPLEMENT
template<typename Type>
CStateMachine<Type>::~CStateMachine()
{
	for (auto& pair : m_States)
		Safe_Delete(pair.second);

	m_States.clear();
	m_Parameters.clear();
	m_Transitions.clear();
	m_AnyStateTransitions.clear();
}

template<typename Type>
HRESULT CStateMachine<Type>::Initialize(Type* pOwner)
{
	if (nullptr == pOwner)
		return E_FAIL;

	m_pOwner = pOwner;

	// 항상 DefaultState로 리셋
	if (!m_strDefaultState.empty())
	{
		m_pCurrentState = nullptr;
		m_strCurrentState = "";
		Change_State(m_strDefaultState);
	}

	return S_OK;
}

template<typename Type>
void CStateMachine<Type>::Update(_float dt)
{
	if (nullptr == m_pCurrentState)
		return;

	m_fTotalTime += dt;
	m_fStateTime += dt;
	m_pCurrentState->m_fStateTime = m_fStateTime;

	m_pCurrentState->Update(m_pOwner, dt);

	Update_AnimProgress();

	Check_AnyStateTransitions();
	if (m_fStateTime == 0.f) // 방금 전환됨
		return;

	Check_Transitions();
}

template<typename Type>
void CStateMachine<Type>::Render_GUI()
{
	if (!m_bShowWindow)	return;
	string strWindowName = "StateMachine Debug##" + to_string(reinterpret_cast<uintptr_t>(this));
	ImGui::SetNextWindowSize(ImVec2(450.f, 600.f), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(strWindowName.c_str(), &m_bShowWindow))
	{
		ImGui::End();
		return;
	}

	Render_Info();
	Render_Animation();
	Render_Parameters();
	Render_Transition();
	Render_History();
	Render_StateGraph();

	if (ImGui::CollapsingHeader("State Hierarchy"))
	{
		for (auto& pair : m_States)
		{
			if (!pair.second->Get_ParentState())
				Render_Hierarchy(pair.second, 0);
		}
	}

	ImGui::End();
}

template<typename Type>
void CStateMachine<Type>::Update_AnimProgress()
{
	if (!m_pCurrentState || !m_pOwner)
		return;

	CGameObject* pGameObject = reinterpret_cast<CGameObject*>(m_pOwner);
	if (nullptr == pGameObject)
		return;

	CAnimator3D* pAnimator = pGameObject->template Get_Component<CAnimator3D>();
	if (nullptr == pAnimator)
		return;

	_float fProgress = pAnimator->Get_CurAnimDuration();
	m_pCurrentState->m_fAnimProgress = fProgress;

	IHState<Type>* pHState = dynamic_cast<IHState<Type>*>(m_pCurrentState);
	if (pHState && pHState->Has_SubStateMachine())
	{
		auto pSubFSM = pHState->Get_SubStateMachine();
		if (pSubFSM->Get_CurrentState())
			pSubFSM->Get_CurrentState()->m_fAnimProgress = fProgress;
	}
}

template<typename Type>
HRESULT CStateMachine<Type>::Register_State(const string& strState, IBaseState<Type>* pState)
{
	if (nullptr == pState)
		return E_FAIL;

	auto iter = m_States.find(strState);
	if (iter != m_States.end())
		return E_FAIL;

	pState->Set_StateName(strState);
	m_States[strState] = pState;

	return S_OK;
}

template<typename Type>
void CStateMachine<Type>::UnRegister_State(const string& strState)
{
	auto iter = m_States.find(strState);
	if (iter == m_States.end())
		return;

	if (m_pCurrentState == iter->second)
		m_pCurrentState = nullptr;

	Safe_Delete(iter->second);
	m_States.erase(iter);
}

template<typename Type>
IBaseState<Type>* CStateMachine<Type>::Get_State(const string& strState)
{
	auto iter = m_States.find(strState);
	if (iter == m_States.end())
		return nullptr;

	return iter->second;
}

template<typename Type>
HRESULT CStateMachine<Type>::Register_Transition(const TRANSITION_INFO& transition)
{
	if (transition.strFromState.empty() || transition.strToState.empty())
		return E_FAIL;

	m_Transitions.push_back(transition);

	return S_OK;
}

template<typename Type>
HRESULT CStateMachine<Type>::Register_Transition(const string& strFrom, const string& strTo,
	const TRANSITION_CONDITION eCondition, const string& strParam, _float fTimer)
{
	if (strFrom.empty() || strTo.empty())
		return E_FAIL;

	TRANSITION_INFO transition;
	transition.strFromState = strFrom;
	transition.strToState = strTo;

	if (eCondition != CONDITION_NONE)
	{
		CONDITION_INFO condition;
		condition.eCondition = eCondition;
		condition.strParameter = strParam;
		condition.fTimer = fTimer;
		transition.Conditions.push_back(condition);
	}

	m_Transitions.push_back(transition);

	return S_OK;
}

template<typename Type>
HRESULT CStateMachine<Type>::Register_Transition(const string& strFrom, const string& strTo, const vector<CONDITION_INFO>& Conditions, _bool bExit, _float fExitTime)
{
	if (strFrom.empty() || strTo.empty())
		return E_FAIL;

	TRANSITION_INFO transition;
	transition.strFromState = strFrom;
	transition.strToState = strTo;
	transition.Conditions = Conditions;
	transition.bExit = bExit;
	transition.fExitTime = fExitTime;

	m_Transitions.push_back(transition);

	return S_OK;
}

template<typename Type>
HRESULT CStateMachine<Type>::Register_AnyStateTransition(const string& strTo,
	TRANSITION_CONDITION eCondition, const string& strParam)
{
	if (strTo.empty())
		return E_FAIL;

	TRANSITION_INFO transition;
	transition.strFromState = "AnyState";
	transition.strToState = strTo;

	if (eCondition != CONDITION_NONE)
	{
		CONDITION_INFO condition;
		condition.eCondition = eCondition;
		condition.strParameter = strParam;
		transition.Conditions.push_back(condition);
	}

	m_AnyStateTransitions.push_back(transition);

	return S_OK;
}

template<typename Type>
HRESULT CStateMachine<Type>::Register_AnyStateTransition(const string& strTo, const vector<CONDITION_INFO>& Conditions)
{
	if (strTo.empty())
		return E_FAIL;

	TRANSITION_INFO transition;
	transition.strFromState = "AnyState";
	transition.strToState = strTo;
	transition.Conditions = Conditions;

	m_AnyStateTransitions.push_back(transition);

	return S_OK;
}

template<typename Type>
void CStateMachine<Type>::Change_State(const string& strState)
{
	auto iter = m_States.find(strState);
	if (iter == m_States.end())
		return;
	// 변경될 상태로 넘어가도 되는지 확인
	if (m_pCurrentState && !m_pCurrentState->Handle_Transition(m_pOwner, strState))
		return;

	if (m_pCurrentState)
	{
		string strReason = "Manual";
		Record_Transition(m_strCurrentState, strState, strReason, m_fStateTime);
	}

	if (m_pCurrentState)
	{
		m_pCurrentState->Begin_Transition(m_pOwner, strState);
		m_pCurrentState->Exit(m_pOwner);
	}

	string strPrevState = m_strCurrentState;
	m_strPrevState = m_strCurrentState;

	m_pCurrentState = iter->second;
	m_strCurrentState = strState;
	m_fStateTime = 0.f;
	m_pCurrentState->m_fStateTime = 0.f;
	m_pCurrentState->m_fAnimProgress = 0.f;

	m_pCurrentState->Enter(m_pOwner);
	m_pCurrentState->End_Transition(m_pOwner, strPrevState);
}

template<typename Type>
void CStateMachine<Type>::Set_DefaultState(const string& strState)
{
	m_strDefaultState = strState;
}

template<typename Type>
void CStateMachine<Type>::Check_Transitions()
{
	for (auto& transition : m_Transitions)
	{
		if (transition.strFromState != m_strCurrentState)
			continue;

		if (Check_Transition(transition))
		{
			for (auto& condition : transition.Conditions)
			{	// 트리거 소모
				if (condition.eCondition == CONDITION_TRIGGER)
					Reset_Trigger(condition.strParameter);
			}
			Change_State(transition.strToState);
			return;
		}
	}
}

template<typename Type>
void CStateMachine<Type>::Check_AnyStateTransitions()
{
	for (auto& transition : m_AnyStateTransitions)
	{
		if (transition.strToState == m_strCurrentState)
			continue;

		if (Check_Transition(transition))
		{
			for (auto& condition : transition.Conditions)
			{	// 트리거 소모
				if (condition.eCondition == CONDITION_TRIGGER)
					Reset_Trigger(condition.strParameter);
			}
			Change_State(transition.strToState);
			return;
		}
	}
}

template<typename Type>
_bool CStateMachine<Type>::Check_Transition(const TRANSITION_INFO& transition)
{
	// ExitTime 체크
	if (transition.bExit && m_pCurrentState)
	{
		if (m_pCurrentState->Get_AnimProgress() < transition.fExitTime)
			return false;
	}

	// 조건이 없으면 통과
	if (transition.Conditions.empty())
		return true;

	// 모든 조건 충족 확인 (AND)
	for (auto& condition : transition.Conditions)
	{
		if (!Evaluate_SingleCondition(condition))
			return false;
	}

	return true;
}

#pragma region RENDER
template<typename Type>
void CStateMachine<Type>::Render_Info()
{
	ImGui::Text("Current State: %s", m_strCurrentState.c_str());
	ImGui::Text("Default State: %s", m_strDefaultState.c_str());
	ImGui::Text("State Time: %.2f", m_fStateTime);
	ImGui::Text("Total Time: %.2f", m_fTotalTime);
	ImGui::Separator();
}

template<typename Type>
void CStateMachine<Type>::Render_Animation()
{
	if (!ImGui::CollapsingHeader("Animation"))
		return;

	_float fProgress = m_pCurrentState->Get_AnimProgress();

	ImGui::ProgressBar(fProgress, ImVec2(-1, 0));
	ImGui::Text("Progress: %.1f%%", fProgress * 100.f);
	ImGui::Text("AnimEnd: %s", m_pCurrentState->Is_AnimEnd() ? "TRUE" : "FALSE");
}

template<typename Type>
void CStateMachine<Type>::Render_Parameters()
{
	if (!ImGui::CollapsingHeader("Parameters"))
		return;

	if (m_Parameters.empty())
	{
		ImGui::Text("No parameters");
		return;
	}

	ImGui::Columns(3, "ParamColumns");
	ImGui::Text("Name"); ImGui::NextColumn();
	ImGui::Text("Type"); ImGui::NextColumn();
	ImGui::Text("Value"); ImGui::NextColumn();
	ImGui::Separator();

	for (auto& pair : m_Parameters)
	{
		ImGui::Text("%s", pair.first.c_str());
		ImGui::NextColumn();

		switch (pair.second.Get_Type())
		{
		case CStateParameter::PARAM_FLOAT:
			ImGui::Text("Float");
			ImGui::NextColumn();
			ImGui::Text("%.2f", pair.second.Get_Float());
			break;
		case CStateParameter::PARAM_INT:
			ImGui::Text("Int");
			ImGui::NextColumn();
			ImGui::Text("%d", pair.second.Get_Int());
			break;
		case CStateParameter::PARAM_BOOL:
			ImGui::Text("Bool");
			ImGui::NextColumn();
			ImGui::TextColored(
				pair.second.Get_Bool() ? ImVec4(0.f, 1.f, 0.f, 1.f) : ImVec4(1.f, 0.f, 0.f, 1.f),
				"%s", pair.second.Get_Bool() ? "true" : "false");
			break;
		case CStateParameter::PARAM_TRIGGER:
			ImGui::Text("Trigger");
			ImGui::NextColumn();
			ImGui::TextColored(
				pair.second.Get_Trigger() ? ImVec4(0.f, 1.f, 0.f, 1.f) : ImVec4(0.5f, 0.5f, 0.5f, 1.f),
				"%s", pair.second.Get_Trigger() ? "ACTIVE" : "idle");
			break;
		}
		ImGui::NextColumn();
	}
	ImGui::Columns(1);
}

template<typename Type>
void CStateMachine<Type>::Render_Transition()
{
	if (!ImGui::CollapsingHeader("Available Transitions"))
		return;

	ImGui::Text("From: %s", m_strCurrentState.c_str());
	ImGui::Separator();

	for (auto& transition : m_Transitions)
	{
		if (transition.strFromState != m_strCurrentState)
			continue;

		_bool bConditionMet = Evaluate_Condition(transition);
		_bool bExitTimeMet = !transition.bExit ||
			(m_pCurrentState->Get_AnimProgress() >= transition.fExitTime);
		_bool bCanTransit = bConditionMet && bExitTimeMet;

		ImVec4 color = bCanTransit ? ImVec4(0.2f, 1.f, 0.2f, 1.f) : ImVec4(0.6f, 0.6f, 0.6f, 1.f);

		ImGui::TextColored(color, "-> %s", transition.strToState.c_str());
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.f),
			"[%s]", Get_Condition(transition).c_str());

		if (transition.bExit)
		{
			ImGui::SameLine();
			ImGui::Text("(Exit: %.0f%%/%.0f%%)",
				m_pCurrentState->Get_AnimProgress() * 100.f,
				transition.fExitTime * 100.f);
		}
	}

	ImGui::Separator();
	ImGui::Text("AnyState:");
	for (auto& transition : m_AnyStateTransitions)
	{
		if (transition.strToState == m_strCurrentState)
			continue;

		_bool bConditionMet = Evaluate_Condition(transition);
		ImVec4 color = bConditionMet ? ImVec4(1.f, 1.f, 0.f, 1.f) : ImVec4(0.5f, 0.5f, 0.5f, 1.f);

		ImGui::TextColored(color, "-> %s [%s]",
			transition.strToState.c_str(),
			Get_Condition(transition).c_str());
	}
}

template<typename Type>
void CStateMachine<Type>::Render_History()
{
	if (!ImGui::CollapsingHeader("Transition History"))
		return;

	if (m_History.empty())
	{
		ImGui::Text("No transitions yet");
		return;
	}

	for (auto it = m_History.rbegin(); it != m_History.rend(); ++it)
	{
		ImGui::Text("[%.2fs] %s -> %s",
			it->fTimestamp,
			it->strFromState.c_str(),
			it->strToState.c_str());
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5f, 0.8f, 0.5f, 1.f),
			"(%.2fs, %s)", it->fPrevStateTime, it->strTriggerReason.c_str());
	}
}

template<typename Type>
inline void CStateMachine<Type>::Render_StateGraph()
{
	if (!ImGui::CollapsingHeader("State Graph"))
		return;

	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	ImVec2 vCanvasPos = ImGui::GetCursorScreenPos();
	ImVec2 vCanvasSize(400.f, 300.f);

	pDrawList->AddRectFilled(vCanvasPos,
		ImVec2(vCanvasPos.x + vCanvasSize.x, vCanvasPos.y + vCanvasSize.y),
		IM_COL32(30, 30, 30, 255));

	_uint iStateCount = (_uint)m_States.size();
	if (iStateCount == 0)
	{
		ImGui::Dummy(vCanvasSize);
		return;
	}

	_float fRadius = min(vCanvasSize.x, vCanvasSize.y) * 0.35f;
	ImVec2 vCenter(vCanvasPos.x + vCanvasSize.x * 0.5f,
		vCanvasPos.y + vCanvasSize.y * 0.5f);

	unordered_map<string, ImVec2> StatePositions;
	_uint iIndex = 0;

	for (auto& pair : m_States)
	{
		_float fAngle = (2.f * 3.14159f * iIndex) / iStateCount;
		StatePositions[pair.first] = ImVec2(
			vCenter.x + cosf(fAngle) * fRadius,
			vCenter.y + sinf(fAngle) * fRadius);
		++iIndex;
	}

	if (!m_AnyStateTransitions.empty())
		StatePositions["AnyState"] = vCenter;

	// 일반 Transitions
	for (auto& transition : m_Transitions)
	{
		auto itFrom = StatePositions.find(transition.strFromState);
		auto itTo = StatePositions.find(transition.strToState);
		if (itFrom == StatePositions.end() || itTo == StatePositions.end())
			continue;

		ImU32 color = (transition.strFromState == m_strCurrentState) ?
			IM_COL32(255, 200, 100, 255) : IM_COL32(100, 100, 100, 200);

		Draw_Arrow(pDrawList, itFrom->second, itTo->second, color);
	}

	// AnyState Transitions
	for (auto& transition : m_AnyStateTransitions)
	{
		auto itFrom = StatePositions.find("AnyState");
		auto itTo = StatePositions.find(transition.strToState);
		if (itFrom == StatePositions.end() || itTo == StatePositions.end())
			continue;

		_bool bConditionMet = Evaluate_Condition(transition);
		ImU32 color = bConditionMet ? IM_COL32(255, 100, 100, 255) : IM_COL32(150, 80, 80, 150);

		Draw_Arrow(pDrawList, itFrom->second, itTo->second, color);
	}

	// State nodes
	for (auto& pair : m_States)
	{
		ImVec2 vPos = StatePositions[pair.first];
		_bool bIsCurrent = (pair.first == m_strCurrentState);
		_bool bIsDefault = (pair.first == m_strDefaultState);

		ImU32 nodeColor = bIsCurrent ? IM_COL32(100, 200, 100, 255) :
			bIsDefault ? IM_COL32(100, 100, 200, 255) :
			IM_COL32(80, 80, 80, 255);

		pDrawList->AddCircleFilled(vPos, 20.f, nodeColor);
		pDrawList->AddCircle(vPos, 20.f, IM_COL32(200, 200, 200, 255), 0, 2.f);

		ImVec2 vTextSize = ImGui::CalcTextSize(pair.first.c_str());
		pDrawList->AddText(
			ImVec2(vPos.x - vTextSize.x * 0.5f, vPos.y + 25.f),
			IM_COL32(255, 255, 255, 255),
			pair.first.c_str());
	}

	// AnyState 노드 (다른 색상으로 구분)
	if (!m_AnyStateTransitions.empty())
	{
		pDrawList->AddCircleFilled(vCenter, 15.f, IM_COL32(200, 80, 80, 255));
		pDrawList->AddCircle(vCenter, 15.f, IM_COL32(255, 150, 150, 255), 0, 2.f);

		ImVec2 vTextSize = ImGui::CalcTextSize("Any");
		pDrawList->AddText(
			ImVec2(vCenter.x - vTextSize.x * 0.5f, vCenter.y - vTextSize.y * 0.5f),
			IM_COL32(255, 255, 255, 255),
			"Any");
	}

	ImGui::Dummy(vCanvasSize);
}

template<typename Type>
void CStateMachine<Type>::Render_Hierarchy(IBaseState<Type>* pState, _uint iDepth)
{
	if (!pState)
		return;

	_bool bIsCurrent = (pState->Get_StateName() == m_strCurrentState);

	if (bIsCurrent)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.f, 0.2f, 1.f));

	IHState<Type>* pHState = dynamic_cast<IHState<Type>*>(pState);

	if (pHState && pHState->Has_SubStateMachine())
	{
		if (ImGui::TreeNode(pState->Get_StateName().c_str()))
		{
			auto pSubFSM = pHState->Get_SubStateMachine();
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.f),
				"Current: %s", pSubFSM->Get_CurrentStateName().c_str());

			for (auto& pair : pSubFSM->Get_States())
				Render_Hierarchy(pair.second, iDepth + 1);

			ImGui::TreePop();
		}
	}
	else
	{
		ImGui::BulletText("%s", pState->Get_StateName().c_str());
	}

	if (bIsCurrent)
		ImGui::PopStyleColor();
}

template<typename Type>
void CStateMachine<Type>::Draw_Arrow(ImDrawList* pDrawList, ImVec2 vFrom, ImVec2 vTo, ImU32 color)
{
	pDrawList->AddLine(vFrom, vTo, color, 2.f);

	ImVec2 vDir(vTo.x - vFrom.x, vTo.y - vFrom.y);
	_float fLen = sqrtf(vDir.x * vDir.x + vDir.y * vDir.y);
	if (fLen > 0.f)
	{
		vDir.x /= fLen;
		vDir.y /= fLen;
		ImVec2 vArrowPos(vTo.x - vDir.x * 25.f, vTo.y - vDir.y * 25.f);
		ImVec2 vPerp(-vDir.y * 6.f, vDir.x * 6.f);

		pDrawList->AddTriangleFilled(
			ImVec2(vTo.x - vDir.x * 20.f, vTo.y - vDir.y * 20.f),
			ImVec2(vArrowPos.x + vPerp.x, vArrowPos.y + vPerp.y),
			ImVec2(vArrowPos.x - vPerp.x, vArrowPos.y - vPerp.y),
			color);
	}
}
#pragma endregion

template<typename Type>
string CStateMachine<Type>::Get_Condition(const TRANSITION_INFO& transition)
{
	if (transition.Conditions.empty())
		return "None";

	string strResult = "";
	for (size_t i = 0; i < transition.Conditions.size(); ++i)
	{
		auto& cond = transition.Conditions[i];

		switch (cond.eCondition)
		{
		case CONDITION_NONE:
			strResult += "None";
			break;
		case CONDITION_ANIMATION_END:
			strResult += "AnimEnd";
			break;
		case CONDITION_TIME_GREATER:
			strResult += "Time > " + to_string(cond.fTimer);
			break;
		case CONDITION_TIME_LESS:
			strResult += "Time < " + to_string(cond.fTimer);
			break;
		case CONDITION_BOOL_TRUE:
			strResult += cond.strParameter + " == true";
			break;
		case CONDITION_BOOL_FALSE:
			strResult += cond.strParameter + " == false";
			break;
		case CONDITION_TRIGGER:
			strResult += "Trigger: " + cond.strParameter;
			break;
		}

		if (i < transition.Conditions.size() - 1)
			strResult += " && ";
	}

	return strResult;
}

template<typename Type>
_bool CStateMachine<Type>::Evaluate_SingleCondition(const CONDITION_INFO& condition)
{
	switch (condition.eCondition)
	{
	case CONDITION_NONE:
		return true;

	case CONDITION_ANIMATION_END:
		return m_pCurrentState && m_pCurrentState->Is_AnimEnd();

	case CONDITION_TIME_GREATER:
		return m_fStateTime > condition.fTimer;

	case CONDITION_TIME_LESS:
		return m_fStateTime < condition.fTimer;

	case CONDITION_BOOL_TRUE:
		return Get_Bool(condition.strParameter);

	case CONDITION_BOOL_FALSE:
		return !Get_Bool(condition.strParameter);

	case CONDITION_TRIGGER:
		return Get_Trigger(condition.strParameter);
	}

	return false;
}

template<typename Type>
_bool CStateMachine<Type>::Evaluate_Condition(const TRANSITION_INFO& transition)
{
	if (transition.Conditions.empty())
		return true;

	for (auto& condition : transition.Conditions)
	{
		if (!Evaluate_SingleCondition(condition))
			return false;
	}

	return true;
}

template<typename Type>
void CStateMachine<Type>::Record_Transition(const string& strFrom, const string& strTo, const string& strReason, _float fPrevStateTime)
{
	StateTransitionRecord record;
	record.strFromState = strFrom;
	record.strToState = strTo;
	record.fTimestamp = m_fTotalTime;
	record.fPrevStateTime = fPrevStateTime;
	record.strTriggerReason = strReason;

	IHState<Type>* pFromHState = dynamic_cast<IHState<Type>*>(Get_State(strFrom));
	if (pFromHState && pFromHState->Get_ParentState())
		record.strTriggerReason = pFromHState->Get_ParentState()->Get_StateName() + "::" + strReason;
	else
		record.strTriggerReason = strReason;

	m_History.push_back(record);

	if (m_History.size() > m_iMaxHistory)
		m_History.pop_front();
}

template<typename Type>
void CStateMachine<Type>::Set_Float(const string& strParam, _float fValue)
{
	m_Parameters[strParam].Set_Float(fValue);
}

template<typename Type>
void CStateMachine<Type>::Set_Int(const string& strParam, _int iValue)
{
	m_Parameters[strParam].Set_Int(iValue);
}

template<typename Type>
void CStateMachine<Type>::Set_Bool(const string& strParam, _bool bValue)
{
	m_Parameters[strParam].Set_Bool(bValue);
}

template<typename Type>
void CStateMachine<Type>::Set_Trigger(const string& strParam)
{
	m_Parameters[strParam].Set_Trigger();
}

template<typename Type>
void CStateMachine<Type>::Reset_Trigger(const string& strParam)
{
	auto iter = m_Parameters.find(strParam);
	if (iter != m_Parameters.end())
		iter->second.Reset_Trigger();
}

template<typename Type>
_float CStateMachine<Type>::Get_Float(const string& strParam) const
{
	auto iter = m_Parameters.find(strParam);
	if (iter == m_Parameters.end())
		return 0.f;

	return iter->second.Get_Float();
}

template<typename Type>
_int CStateMachine<Type>::Get_Int(const string& strParam) const
{
	auto iter = m_Parameters.find(strParam);
	if (iter == m_Parameters.end())
		return 0;

	return iter->second.Get_Int();
}

template<typename Type>
_bool CStateMachine<Type>::Get_Bool(const string& strParam) const
{
	auto iter = m_Parameters.find(strParam);
	if (iter == m_Parameters.end())
		return false;

	return iter->second.Get_Bool();
}

template<typename Type>
_bool CStateMachine<Type>::Get_Trigger(const string& strParam) const
{
	auto iter = m_Parameters.find(strParam);
	if (iter == m_Parameters.end())
		return false;

	return iter->second.Get_Trigger();
}

template<typename Type>
void CStateMachine<Type>::Free()
{
	for (auto& Pair : m_States)
	{
		Safe_Release(Pair.second);
	}
	m_States.clear();
	__super::Free();
}

using CTestStateMachine = CStateMachine<class CTestObject>;
#pragma endregion

NS_END