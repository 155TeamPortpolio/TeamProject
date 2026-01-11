#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_SwitchInAttack :
    public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_SwitchInAttack* Create() { return new CCorinState_SwitchInAttack(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_SwitchInAttack_Start : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_SwitchInAttack_Start* Create() { return new CCorinState_SwitchInAttack_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_SwitchInAttack_End : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_SwitchInAttack_End* Create() { return new CCorinState_SwitchInAttack_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END