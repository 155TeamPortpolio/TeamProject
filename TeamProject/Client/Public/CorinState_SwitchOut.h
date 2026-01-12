#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_SwitchOut final :
    public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_SwitchOut* Create() { return new CCorinState_SwitchOut(); }
    virtual void Free() override { __super::Free(); }
};

NS_END
