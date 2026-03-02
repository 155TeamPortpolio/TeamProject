#pragma once
#include "Interactable.h"
#include "IInteract.h"

NS_BEGIN(Client)

class CBattleStarter final :
    public CInteractable
{
private:
    CBattleStarter();
    CBattleStarter(const CBattleStarter& rhs);
    virtual ~CBattleStarter() DEFAULT;

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

public:
    _bool m_bHasActivated = { false };

public:
    static CBattleStarter* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END