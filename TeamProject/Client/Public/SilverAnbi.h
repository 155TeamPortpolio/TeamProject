#pragma once
#include "ServiceNpc.h"

NS_BEGIN(Client)
template<typename Type>
class CStateMachine;
class CSilverAnbi :
    public CServiceNpc
{
private:
    CSilverAnbi();
    CSilverAnbi(const CSilverAnbi& rhs);
    virtual ~CSilverAnbi() DEFAULT;

public:
    virtual void    Execute() override;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    static CSilverAnbi* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END