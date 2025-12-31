#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CAnbi;

class CAnbiState_Attack : public IHState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override;
    virtual void Exit(CAnbi* pOwner) override;
    virtual _bool Handle_Transition(CAnbi* pOwner, const string& strState) override;

public:
    void Move_Motion(CAnbi* pOwner, _float dt);

public:
    static CAnbiState_Attack* Create() { return new CAnbiState_Attack(); }
    virtual void Free() override { __super::Free(); }
};

NS_END