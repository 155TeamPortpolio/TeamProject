#include "pch.h"
#include "UI_DamageText.h"
// Engine
#include "ObjectContainer.h"
#include "GameInstance.h"
#include "LevelMgr.h"

HRESULT CUI_DamageText::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_DamageText::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    m_atlasTextureKey = "DamageText.png";
    m_frameCountX = 8;
    m_frameCountY = 8;

    m_glyphHeightPx = 96.f;
    m_glyphSpacingPx = 2.f;

    m_lifeSec = 0.80f;
    m_risePx = 0.f;

    Set_Color(_float4(1.f, 1.f, 1.f, 1.f));

    m_time = 0.f;
    m_baseAnchorOffset = Get_AnchorOffset();

    Set_DamageValue(1234);

    return S_OK;
}

void CUI_DamageText::Update(_float dt)
{
    __super::Update(dt);
    Update_Anim(dt);
    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_DamageText::Set_DamageValue(_int damageValue)
{
    if (damageValue < 0) 
        damageValue = 0;

    Set_TextNumber(to_string(damageValue));
}

void CUI_DamageText::Set_TextNumber(const string& digits)
{
    m_digits = digits;

    Ensure_GlyphCount((_uint)m_digits.size());

    for (_uint i = 0; i < (_uint)m_digits.size(); ++i)
    {
        const char c = m_digits[i];
        _uint frameIndex = 0;

        if (c >= '0' && c <= '9') frameIndex = GetDigitFrameIdx((_uint)(c - '0'));

        auto glyph = GetGlyph(i);
        glyph->Set_Atlas(m_atlasTextureKey, m_frameCountX, m_frameCountY);
        glyph->Set_HeightPx(m_glyphHeightPx);
        glyph->Set_FrameIndex(frameIndex);
        glyph->Set_Color(m_vColor);
    }

    for (_uint i = (_uint)m_digits.size(); i < (_uint)m_glyphs.size(); ++i)
    {
        auto glyph = GetGlyph(i);
        glyph->Set_Alpha(0.f);
    }

    Layout_Glyphs();
}

void CUI_DamageText::Ensure_GlyphCount(_uint count)
{
    auto pContainer = Get_Component<CObjectContainer>();
    const string& levelKey = LevelManager()->Get_NowLevelKey();

    while ((_uint)m_glyphs.size() < count)
    {
        CUI_AtlasSprite::ATLAS_DESC* pDesc = new CUI_AtlasSprite::ATLAS_DESC;
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

void CUI_DamageText::Layout_Glyphs()
{
    const _uint count = (_uint)m_digits.size();

    _float sumW = 0.f;
    for (_uint i = 0; i < count; ++i) sumW += GetGlyph(i)->Get_PxSize().x;

    const _float avgW = sumW / max(1u, count);

    const _float normalOverlap = 0.65f;
    const _float compactOverlap = 0.88f;

    const _float overlap = Math::Lerp(normalOverlap, compactOverlap, m_compactRatio);
    const _float spacing = m_glyphSpacingPx - (avgW * overlap);

    _float totalW = 0.f;
    for (_uint i = 0; i < count; ++i)
    {
        totalW += GetGlyph(i)->Get_PxSize().x;
        if (i + 1 < count) totalW += spacing;
    }

    _float x = -totalW * 0.5f;

    for (_uint i = 0; i < count; ++i)
    {
        auto glyph = GetGlyph(i);

        glyph->Set_Alpha(m_vColor.w);
        glyph->Set_AnchorOffset({x, 0.f});

        x += glyph->Get_PxSize().x + spacing;
    }

    Set_Size({max(1.f, totalW), m_glyphHeightPx});
}


void CUI_DamageText::Update_Anim(_float dt)
{
    m_time += dt;

    const _float fadeInSec = 0.10f;
    const _float holdSec = 0.25f;
    const _float fadeOutSec = max(0.01f, m_lifeSec - (fadeInSec + holdSec));

    const _float t1 = fadeInSec;
    const _float t2 = fadeInSec + holdSec;
    const _float t3 = fadeInSec + holdSec + fadeOutSec;

    _float alpha = 1.f;
    _float scale = 0.50f;

    if (m_time < t1)
    {
        _float u = m_time / t1;

        alpha = u;
        scale = Math::Lerp(1.00f, 0.50f, u);

        m_compactRatio = 1.f;
    }
    else if (m_time < t2)
    {
        _float u = (m_time - t1) / (t2 - t1);

        alpha = 1.f;
        scale = 0.50f;

        m_compactRatio = 1.f - u;
    }
    else
    {
        _float u = (m_time - t2) / (t3 - t2);
        u = clamp(u, 0.f, 1.f);

        alpha = 1.f - u;
        scale = Math::Lerp(0.50f, 0.35f, u);

        m_compactRatio = 0.60f;
    }

    m_scaleNow = scale;

    Set_Alpha(alpha);
    m_vScale = {scale, scale};

    Layout_Glyphs();

    if (m_time >= t3)
        Set_Alive(false);
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