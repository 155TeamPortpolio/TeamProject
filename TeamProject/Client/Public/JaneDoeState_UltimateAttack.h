#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_UltimateAttack :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_UltimateAttack* Create() { return new CJaneDoeState_UltimateAttack(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_UltimateAttack_Start : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_UltimateAttack_Start* Create() { return new CJaneDoeState_UltimateAttack_Start(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_UltimateAttack_Loop : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

private:
    _float m_fDamageTimer = 0.f;
    const _float m_fDamageInterval = 0.15f;
    _bool m_bDamageActive = true;

public:
    static CJaneDoeState_UltimateAttack_Loop* Create() { return new CJaneDoeState_UltimateAttack_Loop(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_UltimateAttack_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_UltimateAttack_End* Create() { return new CJaneDoeState_UltimateAttack_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END