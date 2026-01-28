#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;
class CCorinState_SwitchInParryAid :
    public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_SwitchInParryAid* Create();
    virtual void Free() override { __super::Free(); }
};


class CCorinState_SwitchInParryAid_Start : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_SwitchInParryAid_Start* Create() { return new CCorinState_SwitchInParryAid_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_SwitchInParryAid_L_Loop : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_SwitchInParryAid_L_Loop* Create() { return new CCorinState_SwitchInParryAid_L_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_SwitchInParryAid_L_End : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_SwitchInParryAid_L_End* Create() { return new CCorinState_SwitchInParryAid_L_End(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_SwitchInParryAid_H_Loop : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_SwitchInParryAid_H_Loop* Create() { return new CCorinState_SwitchInParryAid_H_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_SwitchInParryAid_H_End : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_SwitchInParryAid_H_End* Create() { return new CCorinState_SwitchInParryAid_H_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END