#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_ExAttack :
    public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_ExAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_ExAttack_Start : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_ExAttack_Start* Create() { return new CMiyabiState_ExAttack_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_ExAttack_01 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_ExAttack_01* Create() { return new CMiyabiState_ExAttack_01(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CMiyabi* pOwner);

};

class CMiyabiState_ExAttack_02 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

private:
    _vector3 m_vPos = {};
    _vector3 m_vLook = {};
    _int     m_iCount = {0};
    _float   m_fProgress = { 0.2f };
    _float   m_fInterval = { 0.1f };
    _uint    m_iMask = { 0xFFFFFFFF }; // 충돌마스크 복구용

public:
    static CMiyabiState_ExAttack_02* Create() { return new CMiyabiState_ExAttack_02(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CMiyabi* pOwner);

    _uint m_iRepeatCount{};
    _float m_fRepeatProgress{};
    _float m_fRepeatInterval = 0.05f;
    _float m_fDistanceInterval = 0.9f;
    _float4 m_vStartRotation = { 0.f,0.f,0.f,1.f };
};

class CMiyabiState_ExAttack_03 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

private:
    _vector3 m_vPos = {};
    _vector3 m_vLook = {};
    _int     m_iCount = { 0 };
    _float   m_fProgress = { 0.3f };
    _float   m_fInterval = { 0.05f };
    _uint    m_iMask = { 0xFFFFFFFF }; // 충돌마스크 복구용


public:
    static CMiyabiState_ExAttack_03* Create() { return new CMiyabiState_ExAttack_03(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CMiyabi* pOwner);

    _float3 m_vMinRange{ -5.f,0.f,1.f };
    _float3 m_vMaxRange{ -4.f, 9.f, 2.f };
    _float3 m_vCenter{ 0.f,2.f,1.f };
    _uint m_iRepeatCount{};
    _float m_fRepeatProgress{};
    _float m_fRepeatInterval = 0.04f;
};

class CMiyabiState_ExAttack_End : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override {}
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_ExAttack_End* Create() { return new CMiyabiState_ExAttack_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END