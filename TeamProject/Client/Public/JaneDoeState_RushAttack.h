#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_RushAttack :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_RushAttack* Create() { return new CJaneDoeState_RushAttack(); }
    virtual void Free() override { __super::Free(); }

};

class CJaneDoeState_Rush_Start final 
    : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Rush_Start* Create() { return new CJaneDoeState_Rush_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Rush_End final
    : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override {}
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Rush_End* Create() { return new CJaneDoeState_Rush_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END