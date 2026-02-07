#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_GachaTextGroup : public CGameObject
{
public:
    enum class STATE { INVISIBLE, SHOW, BLINK, END };

protected:
    CUI_GachaTextGroup() {}
    CUI_GachaTextGroup(const CUI_GachaTextGroup& rhs) : CGameObject(rhs) {}
    virtual ~CUI_GachaTextGroup() DEFAULT;

public:
    virtual void Show(GachaGrade eGrade);
        
public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Priority_Update(_float dt) override {}
    void    Update(_float dt)          override;
    void    Late_Update(_float dt)     override {}

protected:
    vector<class CUI_GachaText*> m_pTexts = {};

private:
    STATE m_eState = { STATE::END };

    _float m_fTimer = {};
    const _float m_fShowDuration = { 1.f };
    const _float m_fBlinkDuration = { 0.5f };

    _bool m_isVisible = {};
    _float m_fBlinkAcc = {};
    const _float m_fBlinkInterval = { 0.06f }; 

private:
    void Change_State(STATE eState);

    void Update_Show();
    void Update_Blink(_float dt);

public:
    virtual void Free() override { __super::Free(); }
};

NS_END