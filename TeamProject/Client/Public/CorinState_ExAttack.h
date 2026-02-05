#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_ExAttack : public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_ExAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CCorinState_ExAttack_Start : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_ExAttack_Start* Create() { return new CCorinState_ExAttack_Start(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CCorin* pOwner);

};

class CCorinState_ExAttack_Loop : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {};

public:
    static CCorinState_ExAttack_Loop* Create() { return new CCorinState_ExAttack_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_ExAttack_Loop_Walk : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_ExAttack_Loop_Walk* Create() { return new CCorinState_ExAttack_Loop_Walk(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_ExAttack_Explode : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {};

public:
    static CCorinState_ExAttack_Explode* Create() { return new CCorinState_ExAttack_Explode(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_ExAttack_End : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override {}
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_ExAttack_End* Create() { return new CCorinState_ExAttack_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END