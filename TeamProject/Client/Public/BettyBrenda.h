#pragma once
#include "ServiceNpc.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;
class CBettyBrenda :
    public CServiceNpc
{
private:
    CBettyBrenda();
    CBettyBrenda(const CBettyBrenda& rhs);
    virtual ~CBettyBrenda() DEFAULT;

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
    static CBettyBrenda* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    CUI_Object* m_pNameIndicator = {nullptr};
};

NS_END