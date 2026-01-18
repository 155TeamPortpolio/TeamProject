#pragma once
#include "Interactable.h"

NS_BEGIN(Client)

struct LevelSwitched {

};

class CZeroPortal final :
    public CInteractable
{
public:
    typedef struct tagZeroPortalDesc : public Engine::GAMEOBJECT_DESC {
        string NextMapTag{};
    }ZEROPORTAL_DESC;

protected:
    CZeroPortal();
    CZeroPortal(const CZeroPortal& rhs);
    virtual ~CZeroPortal() DEFAULT;

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

    virtual void    Interact() override;

protected:
    string m_NextMapTag{};

public:
    static CZeroPortal* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END