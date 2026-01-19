#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;

class CJaneDoeState_SwitchInAttack :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_SwitchInAttack* Create() { return new CJaneDoeState_SwitchInAttack(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_SwitchInAttack_Start : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInAttack_Start* Create() { return new CJaneDoeState_SwitchInAttack_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_SwitchInAttack_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInAttack_End* Create() { return new CJaneDoeState_SwitchInAttack_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END