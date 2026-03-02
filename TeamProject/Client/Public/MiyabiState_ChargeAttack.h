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
    static CMiyabiState_ChargeAttack* Create();
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

private:
    void Update_Effects(CMiyabi* pOwner);

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

private:
    void Update_Effects(CMiyabi* pOwner);
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

private:
    void Update_Effects(CMiyabi* pOwner);
};

class CMiyabiState_Charge_End : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) {}

public:
    static CMiyabiState_Charge_End* Create() { return new CMiyabiState_Charge_End(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Charge_Attack01 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_Charge_Attack01* Create() { return new CMiyabiState_Charge_Attack01(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Charge_Attack03 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

private:
    _int   m_iMask = {};
    _float m_fAreaTimer = { 0.f };
    _float m_fAreaInterval = { 0.05f };
    _bool  m_bAreaAttack = { false };

public:
    static CMiyabiState_Charge_Attack03* Create() { return new CMiyabiState_Charge_Attack03(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CMiyabi* pOwner);

    _uint m_iRepeatCount{};
    _float m_fRepeatProgress{};
    _float m_fRepeatInterval = 0.015f;
    _float3 m_vMinRange{ -1.f,-2.f,0.f };
    _float3 m_vMaxRange{ 1.f,-1.f,5.f };

    _bool m_OnEndAttack = false;
    _uint m_iStingRepeatCount{};
    _float m_fStingRepeatProgress{};
    _float m_fStingRepeatInterval = 0.07f;
    _float3 m_vStingMinRange{ -5.f,0.f,0.f };
    _float3 m_vStingMaxRange{ -4.f, 3.f, 2.f };
};

NS_END