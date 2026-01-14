#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_Attack :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

private:
    _float      m_fHoldTime = 0.f;

public:
    static CJaneDoeState_Attack* Create() { return new CJaneDoeState_Attack(); }
    virtual void Free() override { __super::Free(); }
};

NS_END