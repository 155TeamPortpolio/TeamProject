#pragma once
#include "IBaseState.h"

NS_BEGIN(Client)

class CTestState_Dash final : public IBaseState<class CTestObject>
{
public:
	CTestState_Dash();
	virtual ~CTestState_Dash() = default;

public:
	virtual void Enter(CTestObject* pOwner) override;
	virtual void Update(CTestObject* pOwner, _float dt) override;
	virtual void Exit(CTestObject* pOwner) override;
	virtual bool Handle_Transition(const string& strState) override;

private:
	_float m_fDashDistance = 0.01f;
};

NS_END