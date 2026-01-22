#pragma once
#include "ServiceNpc.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class COfficeMeow :
    public CServiceNpc
{
private:
    COfficeMeow();
    COfficeMeow(const COfficeMeow& rhs);
    virtual ~COfficeMeow() DEFAULT;

public:
    CStateMachine<COfficeMeow>* Get_StateMachine() { return m_pStateMachine; }
    virtual void    Execute() override;

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
    CStateMachine<COfficeMeow>* m_pStateMachine{};

    _float m_fWelcomeCurrentCoolDown = 0.f;
    static constexpr _float WELCOME_COOLDOWN = 10.f;

public:
    static COfficeMeow* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END