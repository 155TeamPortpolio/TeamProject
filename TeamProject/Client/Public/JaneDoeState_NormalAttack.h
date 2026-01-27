#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CJaneDoe;
class CJaneDoeState_NormalAttack :
    public IHState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    void Set_ComboIndex(_uint iIndex) { m_iComboIndex = iIndex; }
    _uint Get_ComboIndex() const { return m_iComboIndex; }

private:
    _uint m_iComboIndex = 0;

public:
    static CJaneDoeState_NormalAttack* Create() { return new CJaneDoeState_NormalAttack(); }
    virtual void Free() override { __super::Free(); }
};

class CJaneDoeState_Attack_01 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Attack_01* Create() { return new CJaneDoeState_Attack_01(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CJaneDoe* pOwner);

};

class CJaneDoeState_Attack_02 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Attack_02* Create() { return new CJaneDoeState_Attack_02(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CJaneDoe* pOwner);

};

class CJaneDoeState_Attack_03 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Attack_03* Create() { return new CJaneDoeState_Attack_03(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CJaneDoe* pOwner);

};

class CJaneDoeState_Attack_04 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Attack_04* Create() { return new CJaneDoeState_Attack_04(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CJaneDoe* pOwner);

};

class CJaneDoeState_Attack_05 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Attack_05* Create() { return new CJaneDoeState_Attack_05(); }
    virtual void Free() override { __super::Free(); }

private:
    void Update_Effects(CJaneDoe* pOwner);

};

class CJaneDoeState_Attack_06 : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override;
    virtual void Exit(CJaneDoe* pOwner) override;

public:
    static CJaneDoeState_Attack_06* Create() { return new CJaneDoeState_Attack_06(); }
    virtual void Free() override { __super::Free(); }

private:
    _uint m_iMask = {}; // 충돌마스크 복구용

private:
    void Update_Effects(CJaneDoe* pOwner);

};

class CJaneDoeState_Attack_End : public IBaseState<CJaneDoe>
{
public:
    virtual void Enter(CJaneDoe* pOwner) override;
    virtual void Update(CJaneDoe* pOwner, _float dt) override {}
    virtual void Exit(CJaneDoe* pOwner) override {}

public:
    static CJaneDoeState_Attack_End* Create() { return new CJaneDoeState_Attack_End(); }
    virtual void Free() override { __super::Free(); }

};

NS_END