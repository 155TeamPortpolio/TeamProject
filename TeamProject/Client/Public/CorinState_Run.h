#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_Run final : public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_Run* Create() { return new CCorinState_Run(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Run_Loop : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

private:
    _float m_fTurnbackCooldown = 0.f;

public:
    static CCorinState_Run_Loop* Create() { return new CCorinState_Run_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Run_Turnback : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Run_Turnback* Create() { return new CCorinState_Run_Turnback(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Run_End : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Run_End* Create() { return new CCorinState_Run_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END