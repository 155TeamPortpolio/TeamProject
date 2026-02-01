#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_ExAttack :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_ExAttack* Create();
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_ExAttack_Start : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_ExAttack_Start* Create() { return new CJaneDoeState_ExAttack_Start(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CJaneDoe* pOwner);

};

class CJaneDoeState_ExAttack_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_ExAttack_End* Create() { return new CJaneDoeState_ExAttack_End(); }
    virtual void Free() override { __super::Free(); }
};

NS_END