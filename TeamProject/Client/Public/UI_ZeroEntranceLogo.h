#pragma once
#include "GameObject.h"
#include "IInteract.h"

NS_BEGIN(Client)

class CUI_ZeroEntranceLogo : public CGameObject, public IInteract
{
private:
    CUI_ZeroEntranceLogo();
    CUI_ZeroEntranceLogo(const CUI_ZeroEntranceLogo& rhs);
    virtual ~CUI_ZeroEntranceLogo() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

    virtual void    OnTriggerEnter(CGameObject* pOther) override;
    virtual void    OnTriggerStay(CGameObject* pOher)   override;
    virtual void    OnTriggerExit(CGameObject* pOther)  override;

    virtual void Interact(class CGameObject* pObject = nullptr) override;
    virtual OBJECT_HANDLE Get_InteractHandle() override;

private:
    const _float  m_fNameIndicatorOffsetY = { 1.5f };
    const wstring m_strName = L"제로공동";

private:
    void Ready_Collider();
    void Ready_NameIndicator();

    void Update_UI_Interaction(_bool bInteract);

    void Update_YBillboard();

public:	
    static  CGameObject* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
    virtual void Free() override;
};

NS_END