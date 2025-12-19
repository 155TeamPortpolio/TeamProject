#pragma once
#include "StateParameter.h"
#include "IBaseState.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine final
{
public:
	CStateMachine();
	virtual ~CStateMachine();

public:
	HRESULT Initialize(Type* pOwner);
	void	Update(_float dt);
	void	Update_AnimProgress(_float fProgress);

public:
	// State 包府
	HRESULT	Register_State(const string& strState, IBaseState<Type>* pState);
	void	UnRegister_State(const string& strState);
	IBaseState<Type>* Get_State(const string& strState);
	
	// Transition 包府
	HRESULT	Register_Transition(const TRANSITION_INFO& transition);
	HRESULT Register_Transition(const string& strFrom, const string& strTo,
								const TRANSITION_CONDITION eCondition = CONDITION_NONE,
								const string& strParam = "", _float fTimer = 0.f);
	HRESULT	Register_AnyStateTransition(const string& strTo, TRANSITION_CONDITION eCondition, const string& strParam);
	
	// State 傈券
	void	Change_State(const string& strState);
	void	Set_DefaultState(const string& strState);
	
	// Parameter 包府
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
};

using CCharacterStateMachine = CStateMachine<class CCharacter>;

NS_END