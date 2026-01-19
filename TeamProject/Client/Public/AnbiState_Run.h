#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CAnbi;

class CAnbiState_Run final : public IHState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override;
    virtual void Exit(CAnbi* pOwner) override {}

public:
    static CAnbiState_Run* Create() { return new CAnbiState_Run(); }
    virtual void Free() override { __super::Free(); }
};

class CAnbiState_Run_Start : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override;
    virtual void Exit(CAnbi* pOwner) override {}

public:
    static CAnbiState_Run_Start* Create() { return new CAnbiState_Run_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CAnbiState_Run_Loop : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override;
    virtual void Exit(CAnbi* pOwner) override {}

public:
    static CAnbiState_Run_Loop* Create() { return new CAnbiState_Run_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CAnbiState_Run_End : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override;
    virtual void Exit(CAnbi* pOwner) override {}

public:
    static CAnbiState_Run_End* Create() { return new CAnbiState_Run_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END