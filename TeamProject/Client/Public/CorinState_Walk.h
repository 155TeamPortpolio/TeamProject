#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_Walk final : public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_Walk* Create() { return new CCorinState_Walk(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Walk_Start : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Walk_Start* Create() { return new CCorinState_Walk_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Walk_Loop : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Walk_Loop* Create() { return new CCorinState_Walk_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Walk_End : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Walk_End* Create() { return new CCorinState_Walk_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END