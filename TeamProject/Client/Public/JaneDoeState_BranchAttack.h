#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_BranchAttack :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {};

public:
    static CJaneDoeState_BranchAttack* Create() { return new CJaneDoeState_BranchAttack(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_BranchAttack_Start : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_BranchAttack_Start* Create() { return new CJaneDoeState_BranchAttack_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_BranchAttack_Loop : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_BranchAttack_Loop* Create() { return new CJaneDoeState_BranchAttack_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_BranchAttack_Release01 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_BranchAttack_Release01* Create() { return new CJaneDoeState_BranchAttack_Release01(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_BranchAttack_Release02 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_BranchAttack_Release02* Create() { return new CJaneDoeState_BranchAttack_Release02(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_BranchAttack_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_BranchAttack_End* Create() { return new CJaneDoeState_BranchAttack_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END