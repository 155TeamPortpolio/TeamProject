#include "pch.h"
#include "UI_Gangta.h"
// Engine
#include "ObjectContainer.h"
#include "GameInstance.h"
#include "UI_Seoriyeol.h"

namespace
{
    static constexpr _float kRippleStartDelaySec = 0.40f;
    static constexpr _float kRippleAttackSec     = 0.12f;
    static constexpr _float kRippleTauSec        = 0.30f;
    static constexpr _float kRippleFreq          = 2.5f;
    static constexpr _float kRippleAmp           = 0.04f;
    static constexpr _float kRippleOffsetPx      = 10.f;
    static constexpr _float kGroupStepDelaySec   = 0.06f;
    static constexpr _float kAxisMixX            = 0.15f;
    static constexpr _float kAxisMixY            = 0.85f;
    static constexpr _float kMinScaleClamp       = 0.70f;
}

HRESULT CUI_Gangta::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CObjectContainer>();
    return S_OK;
}

HRESULT CUI_Gangta::Initialize(INIT_DESC* arg)
{
    __super::Initialize(arg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("Gangta.json")));
    Cache_Children();

    Ripple_Begin();

    return S_OK;
}

void CUI_Gangta::Update(_float dt)
{
    dt *= 1.f;

    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    Ripple_Update(dt);

    if (Is_AnimFinished())
        UI_DeActive({});
}

void CUI_Gangta::UI_Active(void* arg)
{
    SetAllChildAnim(0);
    Ripple_Begin();
}

void CUI_Gangta::UI_DeActive(void* arg)
{
    Ripple_End();
    UIManager()->Remove_UIObject(this);
}

_bool CUI_Gangta::Is_AnimFinished()
{
    for (auto child : m_children)
        if (!child->Is_AnimFinished())
            return false;

    return true;
}

void CUI_Gangta::Cache_Children()
{
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
        m_children[i] = static_cast<CUI_Object*>(Get_Component<CObjectContainer>()->Find_Descendant(CHILD_INSTNAMES[i]));
}

void CUI_Gangta::SetAllChildAnim(_int idx) const
{
    for (auto child : m_children)
        child->Set_Animation(idx);
}

void CUI_Gangta::Ripple_Begin()
{
    m_rippleTime = 0.f;
    m_rippleEnabled = true;
    m_rippleStarted = false;

    m_baseScaleSelf = m_vScale;

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        m_baseScaleChild[i] = m_children[i]->Get_Scale();
        auto a = m_children[i]->Get_AnchorOffset();
        m_baseAnchorChild[i] = Vector2(a.x, a.y);
    }
}

void CUI_Gangta::Ripple_Update(_float dt)
{
    if (!m_rippleEnabled)
        return;

    m_rippleTime += dt;

    if (!m_rippleStarted)
    {
        if (m_rippleTime < kRippleStartDelaySec)
            return;

        m_rippleStarted = true;

        m_baseScaleSelf = m_vScale;

        for (_int i = 0; i < ENUM(CHILD::END); ++i)
        {
            m_baseScaleChild[i] = m_children[i]->Get_Scale();
            auto a = m_children[i]->Get_AnchorOffset();
            m_baseAnchorChild[i] = Vector2(a.x, a.y);
        }
    }

    const _float t = m_rippleTime - kRippleStartDelaySec;

    Ripple_ApplyGroup(0, t - kGroupStepDelaySec * 0.f);
    Ripple_ApplyGroup(1, t - kGroupStepDelaySec * 1.f);
    Ripple_ApplyGroup(2, t - kGroupStepDelaySec * 2.f);
    Ripple_ApplyGroup(3, t - kGroupStepDelaySec * 3.f);
}


void CUI_Gangta::Ripple_End()
{
    m_rippleEnabled = false;

    Set_Scale(m_baseScaleSelf);

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        m_children[i]->Set_Scale(m_baseScaleChild[i]);
        m_children[i]->Set_AnchorOffset(m_baseAnchorChild[i]);
    }
}

_float CUI_Gangta::Ripple_Envelope(_float t) const
{
    if (t <= 0.f) return 0.f;

    _float attack = t / kRippleAttackSec;
    if (attack < 0.f) attack = 0.f;
    if (attack > 1.f) attack = 1.f;
    attack = attack * attack * (3.f - 2.f * attack);

    _float decay = expf(-t / kRippleTauSec);
    decay = sqrtf(decay);

    return attack * decay;
}

_float CUI_Gangta::Ripple_Pulse(_float t) const
{
    if (t <= 0.f) return 0.f;

    const _float x = t * XM_2PI * kRippleFreq;

    _float p = (1.f - cosf(x)) * 0.5f;
    if (p < 0.f) p = 0.f;
    p = sqrtf(p);

    return p;
}

void CUI_Gangta::Ripple_ApplyGroup(_int groupIndex, _float localTime)
{
    if (localTime <= 0.f)
        return;

    if (groupIndex == 0)
    {
        Ripple_ApplyChild(CHILD::GANG_OUT, localTime);
        Ripple_ApplyChild(CHILD::GANG_IN, localTime);
        Ripple_ApplyChild(CHILD::GANG_OUTLINE, localTime);
        return;
    }

    if (groupIndex == 1)
    {
        Ripple_ApplyChild(CHILD::TA_OUT, localTime);
        Ripple_ApplyChild(CHILD::TA_IN, localTime);
        Ripple_ApplyChild(CHILD::TA_OUTLINE, localTime);
        return;
    }

    if (groupIndex == 2)
    {
        Ripple_ApplyChild(CHILD::FACTORY_OUT_01, localTime);
        Ripple_ApplyChild(CHILD::FACTORY_IN_01, localTime);
        return;
    }

    Ripple_ApplyChild(CHILD::FACTORY_OUT_02, localTime);
    Ripple_ApplyChild(CHILD::FACTORY_IN_02, localTime);
}

void CUI_Gangta::Ripple_ApplyChild(CHILD eChild, _float localTime)
{
    const _float env = Ripple_Envelope(localTime);
    const _float pulse = Ripple_Pulse(localTime);
    const _float amp = kRippleAmp * env * pulse;

    Vector2 s = m_baseScaleChild[ENUM(eChild)];

    _float sx = 1.f - amp * kAxisMixX;
    _float sy = 1.f - amp * kAxisMixY;

    if (sx < kMinScaleClamp) sx = kMinScaleClamp;
    if (sy < kMinScaleClamp) sy = kMinScaleClamp;

    s.x *= sx;
    s.y *= sy;

    const _float phase = localTime * XM_2PI * kRippleFreq;
    const _float wobble = sinf(phase) * (kRippleOffsetPx * env);

    Vector2 a = m_baseAnchorChild[ENUM(eChild)];
    a.y -= wobble;

    m_children[ENUM(eChild)]->Set_Scale(s);
    m_children[ENUM(eChild)]->Set_AnchorOffset(a);
}

CUI_Gangta* CUI_Gangta::Create()
{
    auto inst = new CUI_Gangta();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Gangta");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CUI_Gangta::Clone(INIT_DESC* pArg)
{
    auto inst = new CUI_Gangta(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Gangta");
        Safe_Release(inst);
    }
    return inst;
}