#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;
class CMiyabiState_SwitchInParryAid :
    public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_SwitchInParryAid* Create();
    virtual void Free() override { __super::Free(); }
};


class CMiyabiState_SwitchInParryAid_Start : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_SwitchInParryAid_Start* Create() { return new CMiyabiState_SwitchInParryAid_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_SwitchInParryAid_L_Loop : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_SwitchInParryAid_L_Loop* Create() { return new CMiyabiState_SwitchInParryAid_L_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_SwitchInParryAid_L_End : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_SwitchInParryAid_L_End* Create() { return new CMiyabiState_SwitchInParryAid_L_End(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_SwitchInParryAid_H_Loop : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_SwitchInParryAid_H_Loop* Create() { return new CMiyabiState_SwitchInParryAid_H_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_SwitchInParryAid_H_End : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_SwitchInParryAid_H_End* Create() { return new CMiyabiState_SwitchInParryAid_H_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END