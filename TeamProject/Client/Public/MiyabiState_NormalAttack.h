#pragma once
#include "IBaseState.h"

NS_BEGIN(Client)

class CMiyabi;

class CMiyabiState_NormalAttack : public IBaseState<CMiyabi>
{
public:
    virtual void Enter(CMiyabi* pOwner) override;
    virtual void Update(CMiyabi* pOwner, _float dt) override;
    virtual void Exit(CMiyabi* pOwner) override;

private:
    void Play_ComboAnimation(CMiyabi* pOwner);
    void Play_EndAnimation(CMiyabi* pOwner);

private:
    _uint m_iComboIndex = 0;
    _bool m_bComboReserved = false;
    _bool m_bIsEnd = false;

public:
    static CMiyabiState_NormalAttack* Create() { return new CMiyabiState_NormalAttack(); }
    virtual void Free() override { __super::Free(); }
};

NS_END