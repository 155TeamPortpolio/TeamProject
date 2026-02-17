#pragma once
#include "GameObject.h"
#include "IInteract.h"

NS_BEGIN(Client)

class CUI_EntranceLogo : public CGameObject, public IInteract
{
protected:
    CUI_EntranceLogo();
    CUI_EntranceLogo(const CUI_EntranceLogo& rhs);
    virtual ~CUI_EntranceLogo() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

    virtual void    OnTriggerEnter(CGameObject* pOther) override;
    virtual void    OnTriggerStay(CGameObject* pOher)   override;
    virtual void    OnTriggerExit(CGameObject* pOther)  override;

    virtual void Interact(class CGameObject* pObject = nullptr) override;
    virtual OBJECT_HANDLE Get_InteractHandle() override;

protected:
    _float  m_fNameIndicatorOffsetY = { 1.5f };
    wstring m_strName = L"";

private:
    void Ready_Collider();
    void Ready_NameIndicator();

    void Update_UI_Interaction(_bool bInteract);

    void Update_YBillboard();

public:
    virtual void Free() override;
};

NS_END