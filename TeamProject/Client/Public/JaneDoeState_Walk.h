#pragma once
#include "IHState.h"
NS_BEGIN(Client)

class CJaneDoe;

class CJaneDoeState_Walk final : public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Walk* Create() { return new CJaneDoeState_Walk(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Walk_Start : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Walk_Start* Create() { return new CJaneDoeState_Walk_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Walk_Loop : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Walk_Loop* Create() { return new CJaneDoeState_Walk_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Walk_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override {}
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Walk_End* Create() { return new CJaneDoeState_Walk_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END