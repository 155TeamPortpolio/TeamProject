#pragma once
#include "pch.h"

NS_BEGIN(Client)

template<typename Type>
class IBaseState abstract
{
public:
	virtual ~IBaseState() DEFAULT;

public:
	virtual Enter(Type* pOwner) {}
	virtual Update(Type* pOwner, _float dt) {}
	virtual Exit(Type* pOwner) {}
	// 상태 전환 관련
	virtual bool Handle_Transition(const string& strState) { return true; }
	virtual void Begin_Transition(Type* pOwner, const string& strNextState) {}
	virtual void End_Transition(Type* pOwner, const string& strPrevState) {}

public:
	const string& Get_StateName() const { return m_strState; }
	void Set_StateName(const string& strState) { m_strState = strState; }
	
protected:
	string m_strState = "";
	_float m_fStateTime = { 0.f };	// 상태 진입 후 경과 시간
};

using ICharacterState = IBaseState<class CCharacter>;

NS_END