#pragma once
#include "UI_EntranceLogo.h"

NS_BEGIN(Client)

class CUI_ZeroEntranceLogo : public CUI_EntranceLogo
{
private:
    CUI_ZeroEntranceLogo();
    CUI_ZeroEntranceLogo(const CUI_ZeroEntranceLogo& rhs);
    virtual ~CUI_ZeroEntranceLogo() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

    virtual void Interact(class CGameObject* pObject = nullptr) override;

public:	
    static  CGameObject* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
    virtual void Free() override;
};

NS_END