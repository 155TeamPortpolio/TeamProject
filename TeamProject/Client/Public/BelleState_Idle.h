#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CBelle;
class CBelleState_Idle :
    public IHState<CBelle>
{
public:
    virtual void Enter(CBelle* pOwner) override;
    virtual void Update(CBelle* pOwner, _float dt) override {}
    virtual void Exit(CBelle* pOwner) override {}

public:
    static CBelleState_Idle* Create() { return new CBelleState_Idle(); }
    virtual void Free() override { __super::Free(); }
};

NS_END