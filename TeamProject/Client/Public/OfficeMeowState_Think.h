#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class COfficeMeow;
class COfficeMeowState_Think :
    public IHState<COfficeMeow>
{
public:
    virtual void Enter(COfficeMeow* pOwner) override;
    virtual void Update(COfficeMeow* pOwner, _float dt) override {}
    virtual void Exit(COfficeMeow* pOwner) override {}

public:
    static COfficeMeowState_Think* Create() { return new COfficeMeowState_Think(); }
    virtual void Free() override { __super::Free(); }
};

NS_END