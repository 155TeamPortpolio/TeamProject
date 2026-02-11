#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_ZeroEntranceLogo : public CGameObject
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

public:	
    static  CGameObject* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
    virtual void Free() override;
};

NS_END