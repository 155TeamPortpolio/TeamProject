#pragma once
#include "ServiceNpc.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class CJaeger3 :
    public CServiceNpc
{
private:
    CJaeger3();
    CJaeger3(const CJaeger3& rhs);
    virtual ~CJaeger3() DEFAULT;

public:
    virtual void    Execute() override;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

private:
    virtual void    Success(_uint curSequenceID) override;

public:
    static CJaeger3* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END