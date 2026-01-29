#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_AssaultAttack final :
    public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_AssaultAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Assault_Start : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Assault_Start* Create() { return new CCorinState_Assault_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Assault_End : public IBaseState<CCorin>
{
public:
    virtual void  Enter(CCorin* pOwner) override;
    virtual void  Update(CCorin* pOwner, _float dt) override;
    virtual void  Exit(CCorin* pOwner) override {}
    virtual _bool Handle_Transition(CCorin* pOwner, const string& strState) override;

public:
    static CCorinState_Assault_End* Create() { return new CCorinState_Assault_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END