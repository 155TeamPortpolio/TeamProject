#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_Walk : public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Walk* Create();
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Walk_Start : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_Walk_Start* Create() { return new CMiyabiState_Walk_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Walk_Loop : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_Walk_Loop* Create() { return new CMiyabiState_Walk_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Walk_End : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_Walk_End* Create() { return new CMiyabiState_Walk_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END
