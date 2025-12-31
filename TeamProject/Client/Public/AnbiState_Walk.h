#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CAnbi;

class CAnbiState_Walk final : public IHState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override;
    virtual void Exit(CAnbi* pOwner) override {}

public:
    static CAnbiState_Walk* Create() { return new CAnbiState_Walk(); }
    virtual void Free() override { __super::Free(); }
};

class CAnbiState_Walk_Start : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override;
    virtual void Exit(CAnbi* pOwner) override {}

public:
    static CAnbiState_Walk_Start* Create() { return new CAnbiState_Walk_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CAnbiState_Walk_Loop : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override;
    virtual void Exit(CAnbi* pOwner) override {}

public:
    static CAnbiState_Walk_Loop* Create() { return new CAnbiState_Walk_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CAnbiState_Walk_End : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override {}
    virtual void Exit(CAnbi* pOwner) override {}

public:
    static CAnbiState_Walk_End* Create() { return new CAnbiState_Walk_End(); }
    virtual void Free() override { __super::Free(); }
};
NS_END