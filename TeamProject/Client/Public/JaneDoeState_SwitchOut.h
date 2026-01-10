#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_SwitchOut final :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override {}
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_SwitchOut* Create() { return new CJaneDoeState_SwitchOut(); }
    virtual void Free() override { __super::Free(); }
};

NS_END