#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;

class CJaneDoeState_Run final : public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Run* Create();
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Run_Loop : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

private:
    _float m_fTurnbackCooldown = 0.f;

public:
    static CJaneDoeState_Run_Loop* Create() { return new CJaneDoeState_Run_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Run_Turnback : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Run_Turnback* Create() { return new CJaneDoeState_Run_Turnback(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Run_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Run_End* Create() { return new CJaneDoeState_Run_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END