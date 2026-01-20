#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;
class CCorinState_UltimateAttack :
    public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_UltimateAttack* Create() { return new CCorinState_UltimateAttack(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_UltimateAttack_Start : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_UltimateAttack_Start* Create() { return new CCorinState_UltimateAttack_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_UltimateAttack_Loop : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_UltimateAttack_Loop* Create() { return new CCorinState_UltimateAttack_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_UltimateAttack_End : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {};

public:
    static CCorinState_UltimateAttack_End* Create() { return new CCorinState_UltimateAttack_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END