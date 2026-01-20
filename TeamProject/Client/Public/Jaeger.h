#pragma once
#include "ServiceNpc.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class CJaeger :
    public CServiceNpc
{
private:
    CJaeger();
    CJaeger(const CJaeger& rhs);
    virtual ~CJaeger() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    static CJaeger* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END