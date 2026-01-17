#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CBelle;
class CBelleState_Walk :
    public IHState<CBelle>
{
public:
    virtual void Enter(CBelle* pOwner) override;
    virtual void Update(CBelle* pOwner, _float dt) override;
    virtual void Exit(CBelle* pOwner) override;

public:
    void Set_LastFoot(string str) { m_LastFoot = str; }
    string Get_LastFoot() { return m_LastFoot; }

private:
    string m_LastFoot = "R";

public:
    static CBelleState_Walk* Create() { return new CBelleState_Walk(); }
    virtual void Free() override { __super::Free(); }
};

class CBelleState_Walk_Start : public IBaseState<CBelle>
{
public:
    virtual void Enter(CBelle* pOwner) override;
    virtual void Update(CBelle* pOwner, _float dt) override;
    virtual void Exit(CBelle* pOwner) override {}

public:
    static CBelleState_Walk_Start* Create() { return new CBelleState_Walk_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CBelleState_Walk_Loop : public IBaseState<CBelle>
{
public:
    virtual void Enter(CBelle* pOwner) override;
    virtual void Update(CBelle* pOwner, _float dt) override;
    virtual void Exit(CBelle* pOwner) override {}

public:
    static CBelleState_Walk_Loop* Create() { return new CBelleState_Walk_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CBelleState_Walk_End : public IBaseState<CBelle>
{
public:
    virtual void Enter(CBelle* pOwner) override;
    virtual void Update(CBelle* pOwner, _float dt) override;
    virtual void Exit(CBelle* pOwner) override {}

public:
    static CBelleState_Walk_End* Create() { return new CBelleState_Walk_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END