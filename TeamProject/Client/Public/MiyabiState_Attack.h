#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_Attack : public IHState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

private:
    _float m_fHoldTime = 0.f;

public:
    static CMiyabiState_Attack* Create() { return new CMiyabiState_Attack(); }
    virtual void Free() override { __super::Free(); }
};

NS_END