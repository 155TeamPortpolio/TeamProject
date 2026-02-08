#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_CounterAttack :
    public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_CounterAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Counter_Start : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

private:
    _uint m_iMask = {};
    _vector3 m_vPos = {};
    _vector3 m_vLook = {};

public:
    static CMiyabiState_Counter_Start* Create() { return new CMiyabiState_Counter_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Counter_End : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_Counter_End* Create() { return new CMiyabiState_Counter_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END