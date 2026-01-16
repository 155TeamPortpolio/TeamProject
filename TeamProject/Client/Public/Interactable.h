#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CInteractable abstract :
    public CGameObject
{
protected:
    CInteractable();
    CInteractable(const CInteractable& rhs);
    virtual ~CInteractable() DEFAULT;

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
    string m_strAnimName = "";
    string m_strName = "";

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};

NS_END