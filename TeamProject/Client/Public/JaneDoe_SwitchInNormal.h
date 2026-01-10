#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;

class CJaneDoe_SwitchInNormal :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoe_SwitchInNormal* Create() { return new CJaneDoe_SwitchInNormal(); }
    virtual void Free() override { __super::Free(); }
};

NS_END