#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_Run final :  public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Run* Create() { return new CMiyabiState_Run(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Run_Start : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Run_Start* Create() { return new CMiyabiState_Run_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Run_Loop : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Run_Loop* Create() { return new CMiyabiState_Run_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Run_End : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Run_End* Create() { return new CMiyabiState_Run_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END