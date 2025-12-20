#pragma once
#include "IBaseState.h"

NS_BEGIN(Client)

class CTestState_Idle final :  public IBaseState<class CTestObject>
{
public:
    CTestState_Idle() DEFAULT;
    virtual ~CTestState_Idle() DEFAULT;

public:
    virtual void Enter(CTestObject* pOwner) override;
    virtual void Update(CTestObject* pOwner, _float dt) override;
    virtual void Exit(CTestObject* pOwner) override;
};

NS_END