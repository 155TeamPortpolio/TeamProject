#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_SwitchInNormal :
    public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_SwitchInNormal* Create() { return new CCorinState_SwitchInNormal(); }
    virtual void Free() override { __super::Free(); }
};

NS_END