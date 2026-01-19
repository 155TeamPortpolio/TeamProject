#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_SwitchInParryAid :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_SwitchInParryAid* Create() { return new CJaneDoeState_SwitchInParryAid(); }
    virtual void Free() override { __super::Free(); }
};


class CJaneDoeState_SwitchInParryAid_Start : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInParryAid_Start* Create() { return new CJaneDoeState_SwitchInParryAid_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_SwitchInParryAid_L_Loop : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInParryAid_L_Loop* Create() { return new CJaneDoeState_SwitchInParryAid_L_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_SwitchInParryAid_L_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInParryAid_L_End* Create() { return new CJaneDoeState_SwitchInParryAid_L_End(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_SwitchInParryAid_H_Loop : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInParryAid_H_Loop* Create() { return new CJaneDoeState_SwitchInParryAid_H_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_SwitchInParryAid_H_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchInParryAid_H_End* Create() { return new CJaneDoeState_SwitchInParryAid_H_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END