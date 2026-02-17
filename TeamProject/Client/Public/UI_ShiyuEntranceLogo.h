#pragma once
#include "UI_EntranceLogo.h"

NS_BEGIN(Client)

class CUI_ShiyuEntranceLogo : public CUI_EntranceLogo
{
private:
    CUI_ShiyuEntranceLogo();
    CUI_ShiyuEntranceLogo(const CUI_ShiyuEntranceLogo& rhs);
    virtual ~CUI_ShiyuEntranceLogo() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    static  CGameObject* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
    virtual void Free() override;
};

NS_END