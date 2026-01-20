#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CHowl;
class CHowlState_Idle :
    public IHState<CHowl>
{
public:
    virtual void Enter(CHowl* pOwner) override;
    virtual void Update(CHowl* pOwner, _float dt) override;
    virtual void Exit(CHowl* pOwner) override {}

public:
    static CHowlState_Idle* Create() { return new CHowlState_Idle(); }
    virtual void Free() override { __super::Free(); }

};

NS_END