#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;
class CMiyabiState_UltimateAttack :
    public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_UltimateAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_UltimateAttack_Start : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_UltimateAttack_Start* Create() { return new CMiyabiState_UltimateAttack_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_UltimateAttack_Loop : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

private:
    _float m_fDamageTimer = 0.f;
    const _float m_fDamageInterval = 0.08f;
    _bool m_bDamageActive = true;

public:
    static CMiyabiState_UltimateAttack_Loop* Create() { return new CMiyabiState_UltimateAttack_Loop(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CMiyabi* pOwner);

    _uint m_iRepeatCount{};
    _float m_fRepeatProgress{};
    _float m_fRepeatInterval = 0.01f;
    _float m_fDistanceInterval = 0.53f;
    _float4 m_vStartRotation{};

    _uint m_iStingRepeatCount{};
    _float m_fStingRepeatProgress{};
    _float m_fStingRepeatInterval = 0.015f;
    _float m_fStingDistanceInterval = 0.6f;
    _float3 m_vMinRange{ -5.f,0.f,-1.f };
    _float3 m_vMaxRange{ -4.f, 3.f, 1.f };
};

class CMiyabiState_UltimateAttack_End : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_UltimateAttack_End* Create() { return new CMiyabiState_UltimateAttack_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END