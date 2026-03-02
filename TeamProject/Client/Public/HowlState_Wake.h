#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CHowl;
class CHowlState_Wake :
    public IHState<CHowl>
{
public:
    virtual void Enter(CHowl* pOwner) override;
    virtual void Update(CHowl* pOwner, _float dt) override;
    virtual void Exit(CHowl* pOwner) override;

public:
    static CHowlState_Wake* Create() { return new CHowlState_Wake(); }
    virtual void Free() override { __super::Free(); }
};

class CHowlState_Wake_Start : public IBaseState<CHowl>
{
public:
    virtual void Enter(CHowl* pOwner) override;
    virtual void Update(CHowl* pOwner, _float dt) override {}
    virtual void Exit(CHowl* pOwner) override {}

public:
    static CHowlState_Wake_Start* Create() { return new CHowlState_Wake_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CHowlState_Wake_Loop : public IBaseState<CHowl>
{
public:
    virtual void Enter(CHowl* pOwner) override;
    virtual void Update(CHowl* pOwner, _float dt) override {}
    virtual void Exit(CHowl* pOwner) override {}

public:
    static CHowlState_Wake_Loop* Create() { return new CHowlState_Wake_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CHowlState_Wake_End : public IBaseState<CHowl>
{
public:
    virtual void Enter(CHowl* pOwner) override;
    virtual void Update(CHowl* pOwner, _float dt) override {}
    virtual void Exit(CHowl* pOwner) override {}

public:
    static CHowlState_Wake_End* Create() { return new CHowlState_Wake_End(); }
    virtual void Free() override { __super::Free(); }
};

class CHowlState_Wake_Idle : public IBaseState<CHowl>
{
public:
    virtual void Enter(CHowl* pOwner) override;
    virtual void Update(CHowl* pOwner, _float dt) override {}
    virtual void Exit(CHowl* pOwner) override {}

public:
    static CHowlState_Wake_Idle* Create() { return new CHowlState_Wake_Idle(); }
    virtual void Free() override { __super::Free(); }
};

NS_END