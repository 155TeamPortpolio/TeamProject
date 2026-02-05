#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_RushAttack :
    public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_RushAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Rush_Start final
    : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_Rush_Start* Create() { return new CMiyabiState_Rush_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Rush_End final : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override {}
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_Rush_End* Create() { return new CMiyabiState_Rush_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END