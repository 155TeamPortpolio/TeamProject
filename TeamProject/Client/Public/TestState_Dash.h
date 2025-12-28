#pragma once
#include "IBaseState.h"

NS_BEGIN(Client)

class CTestState_Dash final : public IBaseState<class CTestObject>
{
public:
	CTestState_Dash() DEFAULT;
	virtual ~CTestState_Dash() DEFAULT;

public:
	virtual void Enter(CTestObject* pOwner) override;
	virtual void Update(CTestObject* pOwner, _float dt) override;
	virtual void Exit(CTestObject* pOwner) override;
	//virtual bool Handle_Transition(const string& strState) override;

public:
	static CTestState_Dash* Create() { return new CTestState_Dash(); }
};

NS_END