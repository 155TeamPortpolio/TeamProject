#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_GachaTextReveal : public CGameObject
{
private:
    CUI_GachaTextReveal() {}
    CUI_GachaTextReveal(const CUI_GachaTextReveal& rhs) : CGameObject(rhs) {}
    virtual ~CUI_GachaTextReveal() DEFAULT;

public:
    void Show(const GACHA_RESULT_DESC& desc);
    
public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Priority_Update(_float dt) override {}
    void    Update(_float dt)          override;
    void    Late_Update(_float dt)     override {}

private:
    HRESULT Add_TextGroups();
    HRESULT Add_TextGroup(const string& strLevelTag, const string& strPrototypeTag, GachaType eType, CGameObject* pObj);

private:
    class CUI_GachaTextGroup* m_pGroups[ENUM(GachaType::End)] = {};

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END