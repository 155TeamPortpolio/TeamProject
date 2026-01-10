#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_SwitchInExAttack :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInExAttack* Create() { return new CJaneDoeState_SwitchInExAttack(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_SwitchInExAttack_Start : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInExAttack_Start* Create() { return new CJaneDoeState_SwitchInExAttack_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_SwitchInExAttack_Loop : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInExAttack_Loop* Create() { return new CJaneDoeState_SwitchInExAttack_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_SwitchInExAttack_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInExAttack_End* Create() { return new CJaneDoeState_SwitchInExAttack_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END