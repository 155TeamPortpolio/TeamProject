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
    virtual void Show(GachaGrade eGrade, const string& strCamSequenceKey) override;

public:
    virtual HRESULT Initialize_Prototype()     override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt) override {}
    virtual void    Update(_float dt)          override;
    virtual void    Late_Update(_float dt)     override {}

private:
    HRESULT Add_Texts();

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END