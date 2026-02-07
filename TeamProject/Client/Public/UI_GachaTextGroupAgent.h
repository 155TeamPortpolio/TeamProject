#pragma once
#include "UI_GachaTextGroup.h"

NS_BEGIN(Client)

class CUI_GachaTextGroupAgent final : public CUI_GachaTextGroup
{
public:
    enum class TYPE { LT, LC, LB, RT, RB, END };

private:
    CUI_GachaTextGroupAgent() {}
    CUI_GachaTextGroupAgent(const CUI_GachaTextGroupAgent& rhs) : CUI_GachaTextGroup(rhs) {}
    virtual ~CUI_GachaTextGroupAgent() DEFAULT;

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
    HRESULT Add_Text(TYPE eType, const string& strModelKey, const _float3& vRotation, const _float3& vPosition, _float fScaleOffset = 0.f, _float fPosXOffset = 0.f);

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END