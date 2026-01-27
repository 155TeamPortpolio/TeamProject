#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;

class CJaneDoeState_CounterAttack final :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_CounterAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Counter_01 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Counter_01* Create() { return new CJaneDoeState_Counter_01(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Counter_02 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Counter_02* Create() { return new CJaneDoeState_Counter_02(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Counter_03 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Counter_03* Create() { return new CJaneDoeState_Counter_03(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Counter_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Counter_End* Create() { return new CJaneDoeState_Counter_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END