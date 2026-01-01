#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CAnbi;

class CAnbiState_Idle final : public IHState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override {}
    virtual void Exit(CAnbi* pOwner) override {}

public:
    static CAnbiState_Idle* Create() { return new CAnbiState_Idle(); }
    virtual void Free() override { __super::Free(); }
};

NS_END