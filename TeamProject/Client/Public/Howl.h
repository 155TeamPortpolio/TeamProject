#pragma once
#include "ServiceNpc.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class CHowl :
    public CServiceNpc
{
private:
    CHowl();
    CHowl(const CHowl& rhs);
    virtual ~CHowl() DEFAULT;

public:
    CStateMachine<CHowl>* Get_StateMachine() { return m_pStateMachine; }

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
    HRESULT Initialize_Transitions();

private:
    void         Update_States(_float dt);

private:
    CStateMachine<CHowl>* m_pStateMachine{};

public:
    static CHowl* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END