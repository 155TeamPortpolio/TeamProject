#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_Attack : public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Attack* Create() { return new CMiyabiState_Attack(); }
    virtual void Free() override { __super::Free(); }
};

// Sub States

class CMiyabiState_Attack_01 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Attack_01* Create() { return new CMiyabiState_Attack_01(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Attack_02 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Attack_02* Create() { return new CMiyabiState_Attack_02(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Attack_03 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Attack_03* Create() { return new CMiyabiState_Attack_03(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Attack_04 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Attack_04* Create() { return new CMiyabiState_Attack_04(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Attack_05 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Attack_05* Create() { return new CMiyabiState_Attack_05(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabiState_Attack_06 : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Attack_06* Create() { return new CMiyabiState_Attack_06(); }
    virtual void Free() override { __super::Free(); }
};
NS_END