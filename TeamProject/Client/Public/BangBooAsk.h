#pragma once
#include "ServiceNpc.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class CBangBooAsk :
    public CServiceNpc
{
private:
    CBangBooAsk();
    CBangBooAsk(const CBangBooAsk& rhs);
    virtual ~CBangBooAsk() DEFAULT;

public:
    CStateMachine<CBangBooAsk>* Get_StateMachine() { return m_pStateMachine; }

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
    CStateMachine<CBangBooAsk>* m_pStateMachine{};

public:
    static CBangBooAsk* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END