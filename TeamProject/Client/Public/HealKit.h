#pragma once
#include "Interactable.h"
#include "IInteract.h"

NS_BEGIN(Client)

class CHealKit final :
    public CInteractable
{
public:
    enum class ITEMTYPE { HP, ENERGY, END };
    typedef struct tagHealKitDesc : public Engine::GAMEOBJECT_DESC {
        ITEMTYPE eItemType{};
    }HEALKIT_DESC;

private:
    CHealKit();
    CHealKit(const CHealKit& rhs);
    virtual ~CHealKit() DEFAULT;

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

private:
    void Recovery_Health();
    void Recovery_Energy();
    void Setting_Child();

private:
    _float m_fResetTimer  = { 5.f };
    _float m_fElapsedTime = { 10.f };
    _bool  m_bActiveItem  = { true };
    ITEMTYPE m_eItemType  = { ITEMTYPE::END };

public:
    static CHealKit* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END