#pragma once
#include "GameObject.h"
#include "IInteract.h"
NS_BEGIN(Client)

class CNpcInteractZone :
    public CGameObject, public IInteract
{
private:
    CNpcInteractZone();
    CNpcInteractZone(const CNpcInteractZone& rhs);
    virtual ~CNpcInteractZone() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    virtual OBJECT_HANDLE       Get_InteractHandle() override;
    virtual void                Interact(CGameObject* pObject = nullptr) override;
    virtual void                OnTriggerEnter(CGameObject* pOther) override;
    virtual void                OnTriggerStay(CGameObject* pOther) override;
    virtual void                OnTriggerExit(CGameObject* pOther) override;

private:
    void Update_UI_Interaction(_bool bInteract);

public:
    static CNpcInteractZone* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END