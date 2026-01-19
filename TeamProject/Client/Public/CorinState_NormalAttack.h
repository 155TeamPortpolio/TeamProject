#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CCorin;

class CCorinState_NormalAttack : public IHState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    void Set_ComboIndex(_uint iIndex) { m_iComboIndex = iIndex; }
    _uint Get_ComboIndex() const { return m_iComboIndex; }

private:
    _uint m_iComboIndex = 0;

public:
    static CCorinState_NormalAttack* Create() { return new CCorinState_NormalAttack(); }
    virtual void Free() override { __super::Free(); }
};

// Sub States
#pragma region SubStates
class CCorinState_Attack_01 : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_Attack_01* Create() { return new CCorinState_Attack_01(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Attack_02 : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_Attack_02* Create() { return new CCorinState_Attack_02(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Attack_03 : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_Attack_03* Create() { return new CCorinState_Attack_03(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Attack_04 : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_Attack_04* Create() { return new CCorinState_Attack_04(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Attack_05 : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override;
    virtual void Exit(CCorin* pOwner) override;

public:
    static CCorinState_Attack_05* Create() { return new CCorinState_Attack_05(); }
    virtual void Free() override { __super::Free(); }
};

class CCorinState_Attack_End : public IBaseState<CCorin>
{
public:
    virtual void Enter(CCorin* pOwner) override;
    virtual void Update(CCorin* pOwner, _float dt) override {}
    virtual void Exit(CCorin* pOwner) override {}

public:
    static CCorinState_Attack_End* Create() { return new CCorinState_Attack_End(); }
    virtual void Free() override { __super::Free(); }
};
#pragma endregion

NS_END