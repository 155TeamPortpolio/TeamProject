#pragma once
#include "ServiceNpc.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class CJaeger2 :
    public CServiceNpc
{
private:
    CJaeger2();
    CJaeger2(const CJaeger2& rhs);
    virtual ~CJaeger2() DEFAULT;

public:
    virtual void    Execute() {};

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

private:
    virtual void    Success(_uint curSequenceID) {};

public:
    static CJaeger2* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END