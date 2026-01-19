#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_Backstep : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override {};

public:
    static CCorinState_Backstep* Create() { return new CCorinState_Backstep(); }
    virtual void Free() override { __super::Free(); }
};

NS_END