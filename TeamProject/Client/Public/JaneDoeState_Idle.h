#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;

class CJaneDoeState_Idle final : public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Idle* Create() { return new CJaneDoeState_Idle(); }
    virtual void Free() override { __super::Free(); }
};

NS_END