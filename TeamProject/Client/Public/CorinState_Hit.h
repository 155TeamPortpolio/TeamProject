#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_Hit : public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_Hit* Create() { return new CCorinState_Hit(); }
    virtual void Free() override { __super::Free(); }
};

class CCorin_HitNormal : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override {}
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorin_HitNormal* Create() { return new CCorin_HitNormal(); }
    virtual void Free() override { __super::Free(); }
};

class CCorin_HitHard : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override {}
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorin_HitHard* Create() { return new CCorin_HitHard(); }
    virtual void Free() override { __super::Free(); }
};

class CCorin_HitKnockOut : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override {}
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorin_HitKnockOut* Create() { return new CCorin_HitKnockOut(); }
    virtual void Free() override { __super::Free(); }
};

NS_END