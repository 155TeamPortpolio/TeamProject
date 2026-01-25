#pragma once

#include "UI_AtlasSprite.h"

NS_BEGIN(Client)

class CUI_DamageText final : public CUI_Object
{
private:
    CUI_DamageText() {}
    CUI_DamageText(const CUI_DamageText& rhs);
    virtual ~CUI_DamageText() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg = {}) override;
    void    Update(_float dt) override;

public:
    void Set_DamageValue(_int damageValue);
    void Set_TextNumber(const string& digits);

private:
    void Ensure_GlyphCount(_uint count);

    void Rebuild_BaseLayout();
    void Apply_LayoutScaled();
    void Update_Anim(_float dt);

    _uint GetDigitFrameIdx(_uint digit) const;
    CUI_AtlasSprite* GetGlyph(_uint i) const { return m_glyphs[i]; }

private:
    string m_atlasTextureKey;

    _float m_glyphAspect = 1.f;

    _uint  m_frameCountX = 8;
    _uint  m_frameCountY = 8;

    _float m_glyphHeightPx = 96.f;
    _float m_glyphSpacingPx = 2.f;

    _float m_lifeSec = 0.80f;
    _float m_risePx = 0.f;

    _float m_overlapHold = 0.86f;

    _float m_time = 0.f;
    _float m_scaleNow = 1.f;

    Vector2 m_baseAnchorOffset = Vector2(0.f, 0.f);

    string m_digits;

    _float m_baseTotalW = 1.f;
    vector<Vector2> m_baseOffsets;

private:
    vector<CUI_AtlasSprite*> m_glyphs;

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg = {}) override;
};

NS_END
