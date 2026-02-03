#pragma once
#include "Interactable.h"
#include "IInteract.h"

NS_BEGIN(Client)

class CPortal final :
    public CInteractable, public IInteract
{
public:
    typedef struct tagPortalDesc : public Engine::GAMEOBJECT_DESC {
        string NextNameTag{};
    }PORTAL_DESC;

private:
    CPortal();
    CPortal(const CPortal& rhs);
    virtual ~CPortal() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    void            Awake() override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

    virtual void    OnTriggerEnter(CGameObject* pOther) override;
    virtual void    OnTriggerStay(CGameObject* pOher)   override;
    virtual void    OnTriggerExit(CGameObject* pOther)  override;

    virtual void                Interact(CGameObject* pObject = nullptr) override;
    virtual OBJECT_HANDLE       Get_InteractHandle() override;
private:
    string m_NextLevelTag{};

public:
    static CPortal* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END