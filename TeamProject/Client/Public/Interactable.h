#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CInteractable abstract :
    public CGameObject
{
public:
    typedef struct tagInteractableDesc : public Engine::GAMEOBJECT_DESC {
    }INTERACTABLE_DESC;

protected:
    CInteractable();
    CInteractable(const CInteractable& rhs);
    virtual ~CInteractable() DEFAULT;

    _bool Get_isInteractable() { return m_bIsInteractable; }
public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    void            Awake()                     override {};
    virtual void    Priority_Update(_float dt)  override {};
    virtual void    Update(_float dt)           override {};
    virtual void    Late_Update(_float dt)      override {};

    virtual void    OnTriggerEnter(CGameObject* pOther) override {};
    virtual void    OnTriggerStay(CGameObject* pOher)   override {};
    virtual void    OnTriggerExit(CGameObject* pOther)  override {};

    virtual void    Interact() PURE;
    
protected:
    class CGameInstance* m_pGameInstance = { nullptr }; 
    _bool  m_bIsInteractable = { false };
    
public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};

NS_END