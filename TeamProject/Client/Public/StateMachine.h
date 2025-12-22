#pragma once
#include "Base.h"
#include "StateParameter.h"
#include "IBaseState.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine : public CBase
{
public:
	enum TRANSITION_CONDITION
	{
		CONDITION_NONE = 0,
		CONDITION_ANIMATION_END,      // 애니메이션이 끝났을 때
		CONDITION_TIME_GREATER,       // 상태 시간이 특정 값보다 클 때
		CONDITION_TIME_LESS,          // 상태 시간이 특정 값보다 작을 때
		CONDITION_BOOL_TRUE,          // Bool 파라미터가 true일 때
		CONDITION_BOOL_FALSE,         // Bool 파라미터가 false일 때
		CONDITION_TRIGGER,            // Trigger 파라미터가 설정되었을 때
	};

	typedef struct TransitionInfo
	{
		string strFromState = "";
		string strToState = "";
		TRANSITION_CONDITION eCondition = CONDITION_NONE;
		string strParameter = "";
		_float fTimer = 0.f;		// m_fStateTime과의 비교
		_bool  bExitTime = false;   // Exit Time 사용 여부
		_float fExitTime = 1.f;     // Exit Time 값 (Progress 기준)
	}TRANSITION_INFO;

public:
	CStateMachine();
	virtual ~CStateMachine();

public:
	HRESULT Initialize(Type* pOwner);
	void	Update(_float dt);

public:
	// State 관리
	HRESULT	Register_State(const string& strState, IBaseState<Type>* pState);
	void	UnRegister_State(const string& strState);
	IBaseState<Type>* Get_State(const string& strState);
	
	// Transition 관리
	HRESULT	Register_Transition(const TRANSITION_INFO& transition);
	HRESULT Register_Transition(const string& strFrom, const string& strTo,
								const TRANSITION_CONDITION eCondition = CONDITION_NONE,
								const string& strParam = "", _float fTimer = 0.f);
	HRESULT	Register_AnyStateTransition(const string& strTo, TRANSITION_CONDITION eCondition, const string& strParam);
	
	// State 전환
	void	Change_State(const string& strState);
	void	Set_DefaultState(const string& strState);
	
	// Parameter 관리
	void	Set_Float(const string& strParam, _float fValue);
	void	Set_Int(const string& strParam, _int iValue);
	void	Set_Bool(const string& strParam, _bool bValue);
	void	Set_Trigger(const string& strParam);
	void	Reset_Trigger(const string& strParam);
	_float	Get_Float(const string& strParam) const;
	_int	Get_Int(const string& strParam) const;
	_bool	Get_Bool(const string& strParam) const;
	_bool	Get_Trigger(const string& strParam) const;

public:
	IBaseState<Type>* Get_CurrentState() const { return m_pCurrentState; }
	const string&	  Get_CurrentStateName() const { return m_strCurrentState; }
	_float			  Get_StateTime() const { return m_fStateTime; }

private:
	void	Update_AnimProgress();
	void	Check_Transitions();
	void	Check_AnyStateTransitions();
	_bool	Check_Transition(const TRANSITION_INFO& transition);

private:
	Type* m_pOwner = { nullptr };

	unordered_map<string, IBaseState<Type>*> m_States;
	unordered_map<string, CStateParameter>	 m_Parameters;
	vector<TRANSITION_INFO>					 m_Transitions;
	vector<TRANSITION_INFO>					 m_AnyStateTransitions;

	IBaseState<Type>*						 m_pCurrentState = { nullptr };
	string									 m_strCurrentState = "";
	string									 m_strDefaultState = "";
	_float									 m_fStateTime = 0.f;

public:
	 void Free() override { __super::Free(); }
};

using CTestStateMachine = CStateMachine<class CTestObject>;

NS_END