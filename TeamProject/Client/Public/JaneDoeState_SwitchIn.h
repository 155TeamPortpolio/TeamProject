#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_SwitchIn final:
    public IHState<CJaneDoe>
{
public:
    virtual void  Enter(CJaneDoe* pOwner) override;
    virtual void  Update(CJaneDoe* pOwner, _float dt) override;
    virtual void  Exit(CJaneDoe* pOwner) override;
    virtual _bool Handle_Transition(CJaneDoe* pOwner, const string& strState) override;

public:
    static CJaneDoeState_SwitchIn* Create();
    virtual void Free() override { __super::Free(); }
};

NS_END