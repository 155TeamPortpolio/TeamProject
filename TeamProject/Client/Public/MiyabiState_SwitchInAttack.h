#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_SwitchInAttack :
    public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_SwitchInAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_SwitchInAttack_Start : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_SwitchInAttack_Start* Create() { return new CMiyabiState_SwitchInAttack_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_SwitchInAttack_End : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_SwitchInAttack_End* Create() { return new CMiyabiState_SwitchInAttack_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END