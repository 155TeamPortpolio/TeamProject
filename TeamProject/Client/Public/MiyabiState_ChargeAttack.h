#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_ChargeAttack : public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    void Set_ChargeLevel(_uint iLevel) { m_iChargeLevel = iLevel; }
    _uint Get_ChargeLevel() const { return m_iChargeLevel; }

private:
    _uint m_iChargeLevel = 0;

public:
    static CMiyabiState_ChargeAttack* Create() { return new CMiyabiState_ChargeAttack(); }
    virtual void Free() override { __super::Free(); }
};

// Sub States
class CMiyabiState_Charge_Start : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Charge_Start* Create() { return new CMiyabiState_Charge_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Charge_Start_02 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Charge_Start_02* Create() { return new CMiyabiState_Charge_Start_02(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Charge_Start_03 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Charge_Start_03* Create() { return new CMiyabiState_Charge_Start_03(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Charge_Attack01 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override {}
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_Charge_Attack01* Create() { return new CMiyabiState_Charge_Attack01(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Charge_Attack03 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override {}
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_Charge_Attack03* Create() { return new CMiyabiState_Charge_Attack03(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Charge_End : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Charge_End* Create() { return new CMiyabiState_Charge_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END