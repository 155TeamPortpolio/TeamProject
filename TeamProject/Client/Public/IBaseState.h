#pragma once
#include "pch.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

template<typename Type>
class IBaseState abstract
{
public:
	virtual ~IBaseState() DEFAULT;

public:
	virtual void Enter(Type* pOwner) {}
	virtual void Update(Type* pOwner, _float dt) {}
	virtual void Exit(Type* pOwner) {}
	// 상태 전환 관련
	virtual bool Handle_Transition(const string& strState) { return true; }
	virtual void Begin_Transition(Type* pOwner, const string& strNextState) {}
	virtual void End_Transition(Type* pOwner, const string& strPrevState) {}

public:
	const string& Get_StateName() const { return m_strState; }
	void		  Set_StateName(const string& strState) { m_strState = strState; }
	_float		  Get_StateTime() const { return m_fStateTime; }
	_float		  Get_AnimProgress() const { return m_fAnimProgress; }
	_bool		  Is_AnimEnd() const { return m_fAnimProgress >= 1.f; }
	
protected:
	string m_strState = "";
	_float m_fStateTime = { 0.f };	  // 상태 진입 후 경과 시간
	_float m_fAnimProgress = { 0.f }; // 애니매이션 진행도 : 애니매이터에서 가져와 동기화

	friend class CStateMachine<Type>;
};

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

NS_END