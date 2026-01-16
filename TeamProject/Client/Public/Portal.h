#pragma once
#include "Interactable.h"

NS_BEGIN(Client)

class CPortal final :
    public CInteractable
{
protected:
    CPortal();
    CPortal(const CPortal& rhs);
    virtual ~CPortal() DEFAULT;

public:
    const string& Get_AnimName() const { return m_strAnimName; }

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt)  override {};
    virtual void    Update(_float dt)           override {};
    virtual void    Late_Update(_float dt)      override {};

    virtual void    OnTriggerEnter(CGameObject* pOther) override {};
    virtual void    OnTriggerStay(CGameObject* pOher)   override {};
    virtual void    OnTriggerExit(CGameObject* pOther)  override {};

protected:
    string m_NextLevelTag{};

public:
    static CPortal* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END