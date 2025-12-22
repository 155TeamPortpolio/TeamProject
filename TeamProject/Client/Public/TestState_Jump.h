#pragma once
#include "IBaseState.h"

NS_BEGIN(Client)

class CTestState_Jump final : public IBaseState<class CTestObject>
{
public:
    CTestState_Jump() DEFAULT;
    virtual ~CTestState_Jump() DEFAULT;

public:
    virtual void Enter(CTestObject* pOwner) override;
    virtual void Update(CTestObject* pOwner, _float dt) override;
    virtual void Exit(CTestObject* pOwner) override;

private:
    _bool m_bMove = false;
};

NS_END