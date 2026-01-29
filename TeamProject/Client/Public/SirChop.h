#pragma once
#include "ServiceNpc.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class CSirChop :
    public CServiceNpc
{
private:
    CSirChop();
    CSirChop(const CSirChop& rhs);
    virtual ~CSirChop() DEFAULT;

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
    static CSirChop* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END