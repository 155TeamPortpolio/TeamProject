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
};

class CMiyabiState_ExAttack_02 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_ExAttack_02* Create() { return new CMiyabiState_ExAttack_02(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_ExAttack_03 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabiState_ExAttack_03* Create() { return new CMiyabiState_ExAttack_03(); }
    virtual void Free() override { __super::Free(); }
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