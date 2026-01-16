#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_Hit :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Hit* Create() { return new CJaneDoeState_Hit(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoe_HitNormal : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override {}
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoe_HitNormal* Create() { return new CJaneDoe_HitNormal(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoe_HitHard : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override {}
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoe_HitHard* Create() { return new CJaneDoe_HitHard(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoe_HitKnockOut : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override {}
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoe_HitKnockOut* Create() { return new CJaneDoe_HitKnockOut(); }
    virtual void Free() override { __super::Free(); }
};
NS_END