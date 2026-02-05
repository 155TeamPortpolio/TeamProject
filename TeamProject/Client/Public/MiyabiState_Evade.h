#pragma once
#include "IHState.h"
NS_BEGIN(Client)

class CMiyabi;
class CMiyabiState_Evade :
    public IHState<CMiyabi>
{
public:
    virtual void  Enter(CMiyabi* pOwner) override;
    virtual void  Update(CMiyabi* pOwner, _float dt) override;
    virtual void  Exit(CMiyabi* pOwner) override;

private:
    _uint    m_iMask = { 0xFFFFFFFF };

public:
    static CMiyabiState_Evade* Create();
    virtual void Free() override { __super::Free(); }
};

NS_END