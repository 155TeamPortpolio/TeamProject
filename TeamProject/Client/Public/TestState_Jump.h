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
    virtual void Update(CTestObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(CTestObject* pOwner) override;

private:
    _float m_fJumpPower = 3.f;
    _float m_fWalkSpeed = 3.f;
};

NS_END