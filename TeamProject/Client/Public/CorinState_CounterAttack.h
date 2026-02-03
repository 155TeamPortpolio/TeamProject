#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_CounterAttack final :
    public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_CounterAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Counter_Start : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Counter_Start* Create() { return new CCorinState_Counter_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Counter_Explode : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Counter_Explode* Create() { return new CCorinState_Counter_Explode(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Counter_End : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Counter_End* Create() { return new CCorinState_Counter_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END