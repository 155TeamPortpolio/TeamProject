#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_RushAttack final : public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_RushAttack* Create() { return new CCorinState_RushAttack(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Rush_Start final : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override {}
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Rush_Start* Create() { return new CCorinState_Rush_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Rush_Explode final : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override {}
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Rush_Explode* Create() { return new CCorinState_Rush_Explode(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Rush_End final : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override {}
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Rush_End* Create() { return new CCorinState_Rush_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END