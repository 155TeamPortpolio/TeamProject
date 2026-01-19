#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;

class CJaneDoeState_Dash : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

private:
    _bool m_bEvadeType = false;

public:
    static CJaneDoeState_Dash* Create() { return new CJaneDoeState_Dash(); }
    virtual void Free() override { __super::Free(); }
};

NS_END