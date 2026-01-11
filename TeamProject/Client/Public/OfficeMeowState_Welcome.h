#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class COfficeMeow;
class COfficeMeowState_Welcome :
    public IHState<COfficeMeow>
{
public:
    virtual void Enter(COfficeMeow* pOwner) override;
    virtual void Update(COfficeMeow* pOwner, _float dt) override;
    virtual void Exit(COfficeMeow* pOwner) override {}

public:
    static COfficeMeowState_Welcome* Create() { return new COfficeMeowState_Welcome(); }
    virtual void Free() override { __super::Free(); }
};

NS_END