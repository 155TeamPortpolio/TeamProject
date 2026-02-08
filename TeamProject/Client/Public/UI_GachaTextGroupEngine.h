#pragma once
#include "UI_GachaTextGroup.h"

NS_BEGIN(Client)

class CUI_GachaTextGroupEngine final : public CUI_GachaTextGroup
{
public:
    enum class TYPE { CENTER, END };

private:
    CUI_GachaTextGroupEngine() {}
    CUI_GachaTextGroupEngine(const CUI_GachaTextGroupEngine& rhs) : CUI_GachaTextGroup(rhs) {}
    virtual ~CUI_GachaTextGroupEngine() DEFAULT;

public:
    virtual void Show(GachaGrade eGrade) override;

public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Priority_Update(_float dt) override {}
    void    Update(_float dt)          override;
    void    Late_Update(_float dt)     override {}

private:
    HRESULT Add_Texts();

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END