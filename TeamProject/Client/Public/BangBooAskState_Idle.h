#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CBangBooAsk;
class CBangBooAskState_Idle :
    public IHState<CBangBooAsk>
{
public:
    virtual void Enter(CBangBooAsk* pOwner) override;
    virtual void Update(CBangBooAsk* pOwner, _float dt) override;
    virtual void Exit(CBangBooAsk* pOwner) override {}

public:
    static CBangBooAskState_Idle* Create() { return new CBangBooAskState_Idle(); }
    virtual void Free() override { __super::Free(); }
};

class CBangBooAskState_Idle_Start : public IBaseState<CBangBooAsk>
{
public:
    virtual void Enter(CBangBooAsk* pOwner) override;
    virtual void Update(CBangBooAsk* pOwner, _float dt) override {}
    virtual void Exit(CBangBooAsk* pOwner) override {}

public:
    static CBangBooAskState_Idle_Start* Create() { return new CBangBooAskState_Idle_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CBangBooAskState_Idle_Loop : public IBaseState<CBangBooAsk>
{
public:
    virtual void Enter(CBangBooAsk* pOwner) override;
    virtual void Update(CBangBooAsk* pOwner, _float dt) override {}
    virtual void Exit(CBangBooAsk* pOwner) override {}

public:
    static CBangBooAskState_Idle_Loop* Create() { return new CBangBooAskState_Idle_Loop(); }
    virtual void Free() override { __super::Free(); }
};

NS_END