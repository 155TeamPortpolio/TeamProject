#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;

class CJaneDoeState_AssaultAttack :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_AssaultAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Assault_Start : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Assault_Start* Create() { return new CJaneDoeState_Assault_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Assault_End : public IBaseState<CJaneDoe>
{
public:
    virtual void  Enter(CJaneDoe* pOwner) override;
    virtual void  Update(CJaneDoe* pOwner, _float dt) override;
    virtual void  Exit(CJaneDoe* pOwner) override {}
    virtual _bool Handle_Transition(CJaneDoe* pOwner, const string& strState) override;

public:
    static CJaneDoeState_Assault_End* Create() { return new CJaneDoeState_Assault_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END