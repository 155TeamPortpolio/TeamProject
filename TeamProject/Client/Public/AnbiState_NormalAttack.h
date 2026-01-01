#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CAnbi;

class CAnbiState_NormalAttack : public IHState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override;
    virtual void Exit(CAnbi* pOwner) override;

public:
    void Set_ComboIndex(_uint iIndex) { m_iComboIndex = iIndex; }
    _uint Get_ComboIndex() const { return m_iComboIndex; }

private:
    _uint m_iComboIndex = 0;

public:
    static CAnbiState_NormalAttack* Create() { return new CAnbiState_NormalAttack(); }
    virtual void Free() override { __super::Free(); }
};

// Sub States
#pragma region SubStates
class CAnbiState_Attack_01 : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override {}
    virtual void Exit(CAnbi* pOwner) override;

public:
    static CAnbiState_Attack_01* Create() { return new CAnbiState_Attack_01(); }
    virtual void Free() override { __super::Free(); }
};

class CAnbiState_Attack_02 : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override {}
    virtual void Exit(CAnbi* pOwner) override;

public:
    static CAnbiState_Attack_02* Create() { return new CAnbiState_Attack_02(); }
    virtual void Free() override { __super::Free(); }
};

class CAnbiState_Attack_03 : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override {}
    virtual void Exit(CAnbi* pOwner) override;

public:
    static CAnbiState_Attack_03* Create() { return new CAnbiState_Attack_03(); }
    virtual void Free() override { __super::Free(); }
};

class CAnbiState_Attack_04 : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override {}
    virtual void Exit(CAnbi* pOwner) override;

public:
    static CAnbiState_Attack_04* Create() { return new CAnbiState_Attack_04(); }
    virtual void Free() override { __super::Free(); }
};

class CAnbiState_Attack_End : public IBaseState<CAnbi>
{
public:
    virtual void Enter(CAnbi* pOwner) override;
    virtual void Update(CAnbi* pOwner, _float dt) override;
    virtual void Exit(CAnbi* pOwner) override {}

public:
    static CAnbiState_Attack_End* Create() { return new CAnbiState_Attack_End(); }
    virtual void Free() override { __super::Free(); }
};
#pragma endregion

NS_END