#pragma once
#include "IBaseState.h"

NS_BEGIN(Client)

class CTestState_Walk final : public IBaseState<class CTestObject>
{
public:
    CTestState_Walk() DEFAULT;
    virtual ~CTestState_Walk() DEFAULT;

public:
    virtual void Enter(CTestObject* pOwner) override;
    virtual void Update(CTestObject* pOwner, _float dt) override;
    virtual void Exit(CTestObject* pOwner) override;

public:
    static CTestState_Walk* Create() { return new CTestState_Walk(); }
};

NS_END