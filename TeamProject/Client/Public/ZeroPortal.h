#pragma once
#include "Interactable.h"
#include "IInteract.h"

NS_BEGIN(Client)

class CZeroPortal final :
    public CInteractable, public IInteract
{
public:
    typedef struct tagZeroPortalDesc : public Engine::GAMEOBJECT_DESC {
        string NextMapTag{};
    }ZEROPORTAL_DESC;

private:
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

    virtual void    Interact(CGameObject* pObject = nullptr) override;

private:
    void Extend(_float dt);
    void Contract(_float dt);

private:
    _bool  m_bVisible = { false };
    string m_NextMapTag{};
    _float m_Time = {};
    class CStage* m_pTargetStage = { nullptr };

private:
    _float m_fDuration = .5f;
    _float m_fElapsedTime = 0.f;
    _vector3 m_vTargetSize = {2.f,2.f,2.f};
public:
    static CZeroPortal* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END