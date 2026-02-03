#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_Dash : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override {}

private:
    _bool m_bEvadeType = false;

public:
    static CMiyabiState_Dash* Create() { return new CMiyabiState_Dash(); }
    virtual void Free() override { __super::Free(); }
};

NS_END