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
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_RushAttack* Create() { return new CJaneDoeState_RushAttack(); }
    virtual void Free() override { __super::Free(); }

};

class CJaneDoeState_Rush01_Start final 
    : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Rush01_Start* Create() { return new CJaneDoeState_Rush01_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Rush01_End final
    : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override {}
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Rush01_End* Create() { return new CJaneDoeState_Rush01_End(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Rush02_Start final
    : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Rush02_Start* Create() { return new CJaneDoeState_Rush02_Start(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CJaneDoe* pOwner);

};

class CJaneDoeState_Rush02_End final
    : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override {}
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Rush02_End* Create() { return new CJaneDoeState_Rush02_End(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Rush03_Start final
    : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Rush03_Start* Create() { return new CJaneDoeState_Rush03_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Rush03_End final
    : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override {}
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Rush03_End* Create() { return new CJaneDoeState_Rush03_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END