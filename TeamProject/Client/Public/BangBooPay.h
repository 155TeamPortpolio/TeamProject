#pragma once
#include "ServiceNpc.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class CBangBooPay :
    public CServiceNpc
{
private:
    CBangBooPay();
    CBangBooPay(const CBangBooPay& rhs);
    virtual ~CBangBooPay() DEFAULT;

public:
    CStateMachine<CBangBooPay>* Get_StateMachine() { return m_pStateMachine; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();

private:
    CStateMachine<CBangBooPay>* m_pStateMachine{};

public:
    static CBangBooPay* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END