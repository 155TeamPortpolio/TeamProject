#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_GachaTextReveal : public CGameObject
{
private:
    enum class GROUP { AGENT, ENGINE, END };

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
    void Create_TextGroups();

private:
    vector<class CUI_GachaTextGroup*>   m_Groups;

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END