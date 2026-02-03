#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;
class CMiyabiState_Hit :
    public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

public:
    static CMiyabiState_Hit* Create() { return new CMiyabiState_Hit(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabi_HitNormal : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override {}
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabi_HitNormal* Create() { return new CMiyabi_HitNormal(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabi_HitHard : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override {}
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabi_HitHard* Create() { return new CMiyabi_HitHard(); }
    virtual void Free() override { __super::Free(); }
};

class CMiyabi_HitKnockOut : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override {}
    virtual void Exit(CMiyabi* pOwner) override {}

public:
    static CMiyabi_HitKnockOut* Create() { return new CMiyabi_HitKnockOut(); }
    virtual void Free() override { __super::Free(); }
};
NS_END