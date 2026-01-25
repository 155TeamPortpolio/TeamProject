#include "pch.h"
#include "UI_DamageText.h"

#include "ObjectContainer.h"
#include "GameInstance.h"
#include "LevelMgr.h"

CUI_DamageText::CUI_DamageText(const CUI_DamageText& rhs) : CUI_Object(rhs)
{
    m_glyphs.clear();
    m_baseOffsets.clear();
    m_digits.clear();

    m_baseTotalW = 1.f;
    m_time = 0.f;
    m_scaleNow = 1.f;
}

HRESULT CUI_DamageText::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CObjectContainer>();
    return S_OK;
}

HRESULT CUI_DamageText::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    m_glyphs.clear();
    m_baseOffsets.clear();
    m_digits.clear();

    {
        m_atlasTextureKey = "DamageText.png";

        m_frameCountX = 8;
        m_frameCountY = 8;

        m_glyphHeightPx = 96.f;
        m_glyphSpacingPx = 2.f;

        m_lifeSec = 1.5f;
        m_risePx = 0.f;

        m_overlapHold = 0.3f;
    }

    m_time = 0.f;
    m_scaleNow = 1.f;

    auto base = Get_AnchorOffset();
    m_baseAnchorOffset = Vector2(base.x, base.y);

    Set_Color(_float4(1.f, 1.f, 1.f, 1.f));

    Set_DamageValue(123456789);

    return S_OK;
}


void CUI_DamageText::Update(_float dt)
{
    static constexpr _float timeScale = 0.2f;

    __super::Update(dt * timeScale);

    Update_Anim(dt * timeScale);
    Apply_LayoutScaled();

    Get_Component<CObjectContainer>()->UpdateChild(dt * timeScale);
}

void CUI_DamageText::Set_DamageValue(_int damageValue)
{
    if (damageValue < 0) damageValue = 0;
    Set_TextNumber(to_string(damageValue));
}

void CUI_DamageText::Set_TextNumber(const string& digits)
{
    m_digits = digits;

    const _uint count = (_uint)m_digits.size();
    Ensure_GlyphCount(count);

    for (_uint i = 0; i < count; ++i)
    {
        const char c = m_digits[i];
        _uint frameIndex = 0;
        if (c >= '0' && c <= '9') frameIndex = GetDigitFrameIdx((_uint)(c - '0'));

        auto glyph = GetGlyph(i);
        glyph->Set_Atlas(m_atlasTextureKey, m_frameCountX, m_frameCountY);
        glyph->Set_HeightPx(m_glyphHeightPx);
        glyph->Set_FrameIndex(frameIndex);
        glyph->Set_Color(m_vColor);
        glyph->Set_Alpha(0.f);
    }

    for (_uint i = count; i < (_uint)m_glyphs.size(); ++i)
        GetGlyph(i)->Set_Alpha(0.f);

    m_time = 0.f;
    m_scaleNow = 1.f;

    auto base = Get_AnchorOffset();
    m_baseAnchorOffset = Vector2(base.x, base.y);

    if (count > 0u)
    {
        const auto px = GetGlyph(0)->Get_PxSize();
        m_glyphAspect = px.y > 0.f ? (px.x / px.y) : 1.f;
    }
    else
        m_glyphAspect = 1.f;

    Rebuild_BaseLayout();
    Apply_LayoutScaled();
}

void CUI_DamageText::Ensure_GlyphCount(_uint count)
{
    auto pContainer = Get_Component<CObjectContainer>();
    const string& levelKey = LevelManager()->Get_NowLevelKey();

    while ((_uint)m_glyphs.size() < count)
    {
        auto pDesc = new CUI_AtlasSprite::ATLAS_DESC;
        pDesc->textureKey = m_atlasTextureKey;
        pDesc->frameCountX = m_frameCountX;
        pDesc->frameCountY = m_frameCountY;
        pDesc->frameIndex = 0;
        pDesc->heightPx = m_glyphHeightPx;

        auto builder = Builder::Create_UIObject({levelKey, "Proto_GameObject_AtlasSprite"});
        builder.Add_UIDesc(pDesc);

        CUI_Object* obj = builder.Build("damageGlyph" + to_string((_uint)m_glyphs.size()));
        auto glyph = dynamic_cast<CUI_AtlasSprite*>(obj);

        if (!glyph) return;

        pContainer->Add_Child(glyph);
        m_glyphs.push_back(glyph);

        glyph->Align_To(ANCHOR::Left | ANCHOR::Top);
        glyph->Set_Pivot({0.f, 0.f});
    }
}

void CUI_DamageText::Rebuild_BaseLayout()
{
    const _uint count = (_uint)m_digits.size();

    m_baseOffsets.clear();
    m_baseTotalW = 1.f;

    if (count == 0u) return;

    m_baseOffsets.resize(count);

    const _float glyphW = m_glyphHeightPx * m_glyphAspect;

    const _float overlapPx = glyphW * (1.f - m_overlapHold);
    const _float spacing = m_glyphSpacingPx - overlapPx;

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

    if (count == 0u)
    {
        Set_Size(Vector2(1.f, 1.f));
        return;
    }

    const _float digitInStaggerSec = 0.035f;
    const _float digitInSec = 0.10f;

    const _float digitOutStaggerSec = 0.035f;
    const _float digitOutSec = 0.10f;

    const _float inTotalSec = (count - 1u) * digitInStaggerSec + digitInSec;
    const _float outTotalSec = (count - 1u) * digitOutStaggerSec + digitOutSec;

    _float holdSec = m_lifeSec - (inTotalSec + outTotalSec);
    if (holdSec < 0.f) holdSec = 0.f;

    const _float exitStartSec = inTotalSec + holdSec;

    _float rise = 0.f;
    if (m_risePx > 0.f)
    {
        const _float u = clamp(m_time / m_lifeSec, 0.f, 1.f);
        rise = m_risePx * u;
    }

    Set_AnchorOffset(Vector2(m_baseAnchorOffset.x, m_baseAnchorOffset.y - rise));

    const _float glyphW = m_glyphHeightPx * m_glyphAspect;
    const _float glyphH = m_glyphHeightPx;

    const _float scaleStart = 1.08f;
    const _float scaleHold = 0.68f;
    const _float scaleEnd = 0.42f;

    for (_uint i = 0; i < count; ++i)
    {
        const _uint phase = i;

        const _float inStart = phase * digitInStaggerSec;

        _float s = scaleStart;
        if (m_time >= inStart)
        {
            _float u = (m_time - inStart) / digitInSec;
            u = clamp(u, 0.f, 1.f);
            u = Math::ApplyEase(EaseType::OutCubic, u);
            s = Math::Lerp(scaleStart, scaleHold, u);
        }

        const _float outStart = exitStartSec + phase * digitOutStaggerSec;
        if (m_time >= outStart)
        {
            _float u = (m_time - outStart) / digitOutSec;
            u = clamp(u, 0.f, 1.f);
            u = Math::ApplyEase(EaseType::OutCubic, u);
            s = Math::Lerp(scaleHold, scaleEnd, u);
        }

        const Vector2 base = m_baseOffsets[i];

        const _float dx = (glyphW - glyphW * s) * 0.5f;
        const _float dy = (glyphH - glyphH * s) * 0.5f;

        auto glyph = GetGlyph(i);
        glyph->Set_AnchorOffset(Vector2(base.x + dx, base.y + dy));
        glyph->Set_HeightPx(m_glyphHeightPx * s);
    }

    Set_Size(Vector2(m_baseTotalW, m_glyphHeightPx));
}



void CUI_DamageText::Update_Anim(_float dt)
{
    const _uint count = (_uint)m_digits.size();
    if (count == 0u) return;

    m_time += dt;

    const _float digitInStaggerSec = 0.035f;
    const _float digitInSec = 0.10f;

    const _float digitOutStaggerSec = 0.050f;
    const _float digitOutSec = 0.22f;

    const _float inTotalSec = (count - 1u) * digitInStaggerSec + digitInSec;
    const _float outTotalSec = (count - 1u) * digitOutStaggerSec + digitOutSec;

    _float holdSec = m_lifeSec - (inTotalSec + outTotalSec);
    if (holdSec < 0.f) holdSec = 0.f;

    const _float exitStartSec = inTotalSec + holdSec;
    const _float endSec = exitStartSec + outTotalSec;

    for (_uint i = 0; i < count; ++i)
    {
        const _uint phase = i;

        const _float inStart = phase * digitInStaggerSec;

        _float alphaIn = 0.f;
        if (m_time >= inStart)
        {
            _float u = (m_time - inStart) / digitInSec;
            u = clamp(u, 0.f, 1.f);
            alphaIn = Math::ApplyEase(EaseType::OutCubic, u);
        }

        const _float outStart = exitStartSec + phase * digitOutStaggerSec;

        _float alphaOut = 1.f;
        if (m_time >= outStart)
        {
            _float u = (m_time - outStart) / digitOutSec;
            u = clamp(u, 0.f, 1.f);
            alphaOut = 1.f - Math::ApplyEase(EaseType::OutCubic, u);
        }

        const _float alpha = clamp(alphaIn * alphaOut, 0.f, 1.f);
        GetGlyph(i)->Set_Alpha(alpha * m_vColor.w);
    }

    if (m_time >= endSec) Set_Alive(false);
}

_uint CUI_DamageText::GetDigitFrameIdx(_uint digit) const
{
    const _uint col = (digit >= 8) ? 1u : 0u;
    const _uint rowBottom = (digit >= 8) ? (digit - 8u) : digit;
    const _uint rowTop = (m_frameCountY - 1u) - rowBottom;
    return rowTop * m_frameCountX + col;
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