#pragma once
#include "UI_WorldToScreen.h"

NS_BEGIN(Client)

class CUI_Gangta final : public CUI_WorldToScreen
{
private:
    enum CHILD
    {
        GANG_OUT, GANG_IN, TA_OUT, TA_IN, GANG_OUTLINE, TA_OUTLINE,
        FACTORY_OUT_01, FACTORY_OUT_02, FACTORY_IN_01, FACTORY_IN_02,
        END
    };

    inline static const string CHILD_INSTNAMES[ENUM(CHILD::END)] =
    {
        "Gang_Out", "Gang_In", "Ta_Out", "Ta_In", "Gang_Outline", "Ta_Outline",
        "Factory_Out_01", "Factory_Out_02",
        "Factory_In_01", "Factory_In_02",
    };

private:
    CUI_Gangta() {}
    CUI_Gangta(const CUI_Gangta& rhs);
    virtual ~CUI_Gangta() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* arg = {}) override;

    void    Awake() override {}
    void    Update(_float dt) override;

    void    UI_Active(void* arg) override;
    void    UI_DeActive(void* arg) override;

    _bool   Is_AnimFinished() override;

private:
    void  Cache_Children();
    void  SetAllChildAnim(_int idx) const;

private:
    void   Ripple_Begin();
    void   Ripple_Update(_float dt);
    void   Ripple_End();

    _float Ripple_Envelope(_float t) const;
    _float Ripple_Pulse(_float t)    const;

    void   Ripple_ApplyGroup(_int groupIndex, _float localTime);
    void   Ripple_ApplyChild(CHILD eChild, _float localTime);

private:
    CUI_Object* m_children[ENUM(CHILD::END)]{};

    _float      m_rippleTime = 0.f;
    _bool       m_rippleEnabled = true;

    Vector2     m_baseScaleSelf = Vector2(1.f, 1.f);
    Vector2     m_baseScaleChild[ENUM(CHILD::END)]{};
    Vector2     m_baseAnchorChild[ENUM(CHILD::END)]{};
    _bool       m_rippleStarted = false;

public:
    static CUI_Gangta* Create();
    CGameObject* Clone(INIT_DESC* pArg = {}) override;
};

NS_END
