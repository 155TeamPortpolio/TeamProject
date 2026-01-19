#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CBangBooPay;
class CBangBooPayState_Idle :
    public IHState<CBangBooPay>
{
public:
    virtual void Enter(CBangBooPay* pOwner) override;
    virtual void Update(CBangBooPay* pOwner, _float dt) override;
    virtual void Exit(CBangBooPay* pOwner) override {}

public:
    static CBangBooPayState_Idle* Create() { return new CBangBooPayState_Idle(); }
    virtual void Free() override { __super::Free(); }
};

class CBangBooPayState_Idle_Start : public IBaseState<CBangBooPay>
{
public:
    virtual void Enter(CBangBooPay* pOwner) override;
    virtual void Update(CBangBooPay* pOwner, _float dt) override {}
    virtual void Exit(CBangBooPay* pOwner) override {}

public:
    static CBangBooPayState_Idle_Start* Create() { return new CBangBooPayState_Idle_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CBangBooPayState_Idle_Loop : public IBaseState<CBangBooPay>
{
public:
    virtual void Enter(CBangBooPay* pOwner) override;
    virtual void Update(CBangBooPay* pOwner, _float dt) override {}
    virtual void Exit(CBangBooPay* pOwner) override {}

public:
    static CBangBooPayState_Idle_Loop* Create() { return new CBangBooPayState_Idle_Loop(); }
    virtual void Free() override { __super::Free(); }
};

NS_END