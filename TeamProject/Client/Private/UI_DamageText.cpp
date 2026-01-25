#include "pch.h"
#include "UI_DamageText.h"

#include "ObjectContainer.h"
#include "GameInstance.h"
#include "LevelMgr.h"

namespace
{
    static const string kAtlasTexKey = "DamageText.png";

    constexpr _uint  kFrameCountX = 8;
    constexpr _uint  kFrameCountY = 8;

    constexpr _float kGlyphHeightPx = 96.f;
    constexpr _float kGlyphSpacingPx = 2.f;
    constexpr _float kOverlapHold = 0.3f;
    constexpr _float kRisePx = 0.f;

    constexpr _float kInTotalSec = 0.75f;
    constexpr _float kHoldSec = 2.00f;
    constexpr _float kOutTotalSec = 1.00f;

    constexpr _float kDigitInSec = 0.20f;
    constexpr _float kDigitOutSec = 0.20f;

    constexpr _float kAlphaOutSecRatio = 1.f;

    constexpr _float kScaleStart = 1.85f;
    constexpr _float kScaleHold = 0.68f;
    constexpr _float kScaleEnd = 0.42f;

    constexpr EaseType kScaleEase = EaseType::OutCubic;
    constexpr EaseType kAlphaInEase = EaseType::InCubic;
    constexpr EaseType kAlphaOutEase = EaseType::OutQuad;

    struct DamageTextTiming
    {
        _float digitInSec = 0.f;
        _float digitInStaggerSec = 0.f;
        _float digitOutSec = 0.f;
        _float digitOutStaggerSec = 0.f;

        _float exitStartSec = 0.f;
        _float endSec = 0.f;

        _float alphaOutSec = 0.f;
    };

    static DamageTextTiming CalcTiming(_uint count)
    {
        DamageTextTiming t{};

        if (count > 1u)
        {
            t.digitInSec = kDigitInSec;
            t.digitInStaggerSec = (kInTotalSec - t.digitInSec) / (_float)(count - 1u);
        }
        else
        {
            t.digitInSec = kInTotalSec;
            t.digitInStaggerSec = 0.f;
        }

        if (count > 1u)
        {
            t.digitOutSec = kDigitOutSec;
            t.digitOutStaggerSec = (kOutTotalSec - t.digitOutSec) / (_float)(count - 1u);
        }
        else
        {
            t.digitOutSec = kOutTotalSec;
            t.digitOutStaggerSec = 0.f;
        }

        t.exitStartSec = kInTotalSec + kHoldSec;
        t.endSec = t.exitStartSec + kOutTotalSec;

        t.alphaOutSec = t.digitOutSec * kAlphaOutSecRatio;

        return t;
    }
}


CUI_DamageText::CUI_DamageText(const CUI_DamageText& rhs) : CUI_WorldToScreen(rhs)
{
    m_glyphAspect = 1.f;
    m_time = 0.f;
    m_baseAnchorOffset = Vector2(0.f, 0.f);

    m_digits.clear();
    m_baseTotalW = 1.f;
    m_baseOffsets.clear();
    m_glyphs.clear();
}

HRESULT CUI_DamageText::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CObjectContainer>();
    return S_OK;
}

HRESULT CUI_DamageText::Initialize(INIT_DESC* arg)
{
    __super::Initialize(arg);

    m_digits.clear();
    m_baseOffsets.clear();
    m_glyphs.clear();

    m_time = 0.f;
    m_glyphAspect = 1.f;
    m_worldPos = _float3(0.f, 0.f, 0.f);

    auto base = Get_AnchorOffset();
    m_baseAnchorOffset = Vector2(base.x, base.y);

    Set_Color(_float4(1.f, 1.f, 1.f, 1.f));

    return S_OK;
}

void CUI_DamageText::Update(_float dt)
{
    __super::Update(dt);

    if (m_digits.empty()) return;

    Update_WorldToScreen(m_worldPos);

    auto base = Get_AnchorOffset();
    m_baseAnchorOffset = Vector2(base.x, base.y);

    Update_Anim(dt);
    Apply_LayoutScaled();

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_DamageText::UI_Active(void* arg)
{
    auto desc = static_cast<DAMAGE_DESC*>(arg);

    m_time = 0.f;
    m_worldPos = _float3(desc->pos.x, desc->pos.y, desc->pos.z);

    SetRenderLayer(RENDER_LAYER::Default);
    Update_WorldToScreen(m_worldPos);

    auto base = Get_AnchorOffset();
    m_baseAnchorOffset = Vector2(base.x, base.y);

    SetDamage(desc->damage);
}

void CUI_DamageText::UI_DeActive(void* arg)
{
    for (_uint i = 0; i < (_uint)m_glyphs.size(); ++i)
        GetGlyph(i)->Set_Alpha(0.f);

    m_digits.clear();
    m_baseOffsets.clear();
    m_baseTotalW = 1.f;

    m_time = 0.f;
    SetRenderLayer(RENDER_LAYER::None);
}

void CUI_DamageText::SetDamage(_int damage)
{
    m_digits = to_string(damage);

    const _uint count = (_uint)m_digits.size();
    Ensure_GlyphCount(count);

    for (_uint i = 0; i < count; ++i)
    {
        const _uint digit = (_uint)(m_digits[i] - '0');
        const _uint frameIndex = GetDigitFrameIdx(digit);

        auto glyph = GetGlyph(i);
        glyph->Set_FrameIndex(frameIndex);
        glyph->Set_Color(m_vColor);
        glyph->Set_Alpha(0.f);
    }

    for (_uint i = count; i < (_uint)m_glyphs.size(); ++i)
        GetGlyph(i)->Set_Alpha(0.f);

    {
        const auto px = GetGlyph(0)->Get_PxSize();
        m_glyphAspect = px.x / px.y;
    }

    Rebuild_BaseLayout();
    Apply_LayoutScaled();
}

void CUI_DamageText::Rebuild_BaseLayout()
{
    const _uint count = (_uint)m_digits.size();

    m_baseOffsets.clear();
    m_baseOffsets.resize(count);

    const _float glyphW = kGlyphHeightPx * m_glyphAspect;

    const _float overlapPx = glyphW * (1.f - kOverlapHold);
    const _float spacing = kGlyphSpacingPx - overlapPx;

    _float totalW = glyphW * (_float)count;
    if (count > 1u) totalW += spacing * (_float)(count - 1u);

    m_baseTotalW = max(1.f, totalW);

    _float x = -m_baseTotalW * 0.5f;
    for (_uint i = 0; i < count; ++i)
    {
        m_baseOffsets[i] = Vector2(x, 0.f);
        x += glyphW + spacing;
    }
}

void CUI_DamageText::Apply_LayoutScaled()
{
    const _uint count = (_uint)m_digits.size();
    const auto t = CalcTiming(count);

    _float rise = 0.f;
    if (kRisePx > 0.f)
    {
        const _float u = clamp(m_time / t.endSec, 0.f, 1.f);
        rise = kRisePx * u;
    }

    Set_AnchorOffset(Vector2(m_baseAnchorOffset.x, m_baseAnchorOffset.y - rise));

    const _float glyphW = kGlyphHeightPx * m_glyphAspect;

    for (_uint i = 0; i < count; ++i)
    {
        const _float inStart = (_float)i * t.digitInStaggerSec;

        _float s = kScaleStart;
        if (m_time >= inStart)
        {
            _float u = (m_time - inStart) / t.digitInSec;
            u = clamp(u, 0.f, 1.f);
            u = Math::ApplyEase(kScaleEase, u);
            s = Math::Lerp(kScaleStart, kScaleHold, u);
        }

        const _float outStart = t.exitStartSec + (_float)i * t.digitOutStaggerSec;
        if (m_time >= outStart)
        {
            _float u = (m_time - outStart) / t.digitOutSec;
            u = clamp(u, 0.f, 1.f);
            u = Math::ApplyEase(kScaleEase, u);
            s = Math::Lerp(kScaleHold, kScaleEnd, u);
        }

        const Vector2 base = m_baseOffsets[i];

        const _float dx = (glyphW - glyphW * s) * 0.5f;
        const _float dy = (kGlyphHeightPx - kGlyphHeightPx * s) * 0.5f;

        auto glyph = GetGlyph(i);
        glyph->Set_AnchorOffset(Vector2(base.x + dx, base.y + dy));
        glyph->Set_HeightPx(kGlyphHeightPx * s);
    }

    Set_Size(Vector2(m_baseTotalW, kGlyphHeightPx));
}

void CUI_DamageText::Update_Anim(_float dt)
{
    const _uint count = (_uint)m_digits.size();
    if (count == 0u) return;

    m_time += dt;

    const auto t = CalcTiming(count);

    for (_uint i = 0; i < count; ++i)
    {
        const _float inStart = (_float)i * t.digitInStaggerSec;

        _float alphaIn = 0.f;
        if (m_time >= inStart)
        {
            _float u = (m_time - inStart) / t.digitInSec;
            u = clamp(u, 0.f, 1.f);
            alphaIn = Math::ApplyEase(kAlphaInEase, u);
        }

        const _float outStart = t.exitStartSec + (_float)i * t.digitOutStaggerSec;

        _float alphaOut = 1.f;
        if (m_time >= outStart)
        {
            _float u = (m_time - outStart) / t.alphaOutSec;
            u = clamp(u, 0.f, 1.f);
            alphaOut = 1.f - Math::ApplyEase(kAlphaOutEase, u);
        }

        const _float alpha = clamp(alphaIn * alphaOut, 0.f, 1.f);
        GetGlyph(i)->Set_Alpha(alpha * m_vColor.w);
    }

    if (m_time >= t.endSec) UI_DeActive();
}

void CUI_DamageText::Ensure_GlyphCount(_uint count)
{
    auto container = Get_Component<CObjectContainer>();
    while ((_uint)m_glyphs.size() < count)
    {
        auto pDesc = new CUI_AtlasSprite::ATLAS_DESC;
        pDesc->texKey = kAtlasTexKey;
        pDesc->frameCountX = kFrameCountX;
        pDesc->frameCountY = kFrameCountY;
        pDesc->frameIdx = 0;
        pDesc->heightPx = kGlyphHeightPx;

        auto builder = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_AtlasSprite"});
        builder.Add_UIDesc(pDesc);

        CUI_Object* obj = builder.Build("damageGlyph" + to_string((_uint)m_glyphs.size()));
        auto glyph = static_cast<CUI_AtlasSprite*>(obj);

        container->Add_Child(glyph);
        m_glyphs.push_back(glyph);

        glyph->Align_To(ANCHOR::Left | ANCHOR::Top);
        glyph->Set_Pivot({0.f, 0.f});
    }
}

_uint CUI_DamageText::GetDigitFrameIdx(_uint digit) const
{
    const _uint col = (digit >= 8) ? 1u : 0u;
    const _uint rowBottom = (digit >= 8) ? (digit - 8u) : digit;
    const _uint rowTop = (kFrameCountY - 1u) - rowBottom;
    return rowTop * kFrameCountX + col;
}

CGameObject* CUI_DamageText::Create()
{
    auto inst = new CUI_DamageText();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_DamageText");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CUI_DamageText::Clone(INIT_DESC* pArg)
{
    auto inst = new CUI_DamageText(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_DamageText");
        Safe_Release(inst);
    }
    return inst;
}