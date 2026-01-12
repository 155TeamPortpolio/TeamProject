#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CBangBooDeliver;
class CBangBooDeliverState_Idle :
    public IHState<CBangBooDeliver>
{
public:
    virtual void Enter(CBangBooDeliver* pOwner) override;
    virtual void Update(CBangBooDeliver* pOwner, _float dt) override;
    virtual void Exit(CBangBooDeliver* pOwner) override {}

public:
    static CBangBooDeliverState_Idle* Create() { return new CBangBooDeliverState_Idle(); }
    virtual void Free() override { __super::Free(); }
};

class CBangBooDeliverState_Idle_Start : public IBaseState<CBangBooDeliver>
{
public:
    virtual void Enter(CBangBooDeliver* pOwner) override;
    virtual void Update(CBangBooDeliver* pOwner, _float dt) override {}
    virtual void Exit(CBangBooDeliver* pOwner) override {}

public:
    static CBangBooDeliverState_Idle_Start* Create() { return new CBangBooDeliverState_Idle_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CBangBooDeliverState_Idle_Loop : public IBaseState<CBangBooDeliver>
{
public:
    virtual void Enter(CBangBooDeliver* pOwner) override;
    virtual void Update(CBangBooDeliver* pOwner, _float dt) override {}
    virtual void Exit(CBangBooDeliver* pOwner) override {}

public:
    static CBangBooDeliverState_Idle_Loop* Create() { return new CBangBooDeliverState_Idle_Loop(); }
    virtual void Free() override { __super::Free(); }
};

NS_END