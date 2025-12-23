#pragma once
#include "pch.h"
#include "Base.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

template<typename Type>
class IBaseState abstract : public CBase
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

public:
	void Free() override { __super::Free(); }
};

NS_END