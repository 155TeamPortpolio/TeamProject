#pragma once

#include "UI_AtlasSprite.h"

NS_BEGIN(Client)

class CUI_DamageText final : public CUI_Object
{
public:
    struct DAMAGE_DESC : public UI_DESC
    {
        string atlasTextureKey;

        _uint frameCountX = 8;
        _uint frameCountY = 8;

        _float glyphHeightPx = 48.f;
        _float glyphSpacingPx = 2.f;

        _float lifeSec = 0;
        _float risePx = 0;

        _float4 color = {1.f, 1.f, 1.f, 1.f};

        _int damageValue = 0;
    };

private:
    CUI_DamageText() {}
    CUI_DamageText(const CUI_DamageText& rhs) : CUI_Object(rhs) {}
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
    void Layout_Glyphs();
    void Update_Anim(_float dt);

    _uint GetDigitFrameIdx(_uint digit) const;

private:
    CUI_AtlasSprite* GetGlyph(_uint i) const { return m_glyphs[i]; }

private:
    string m_atlasTextureKey;

    _uint  m_frameCountX = 8;
    _uint  m_frameCountY = 8;

    _float m_glyphHeightPx = 48.f;
    _float m_glyphSpacingPx = 2.f;

    _float m_lifeSec = 0.75f;
    _float m_risePx = 60.f;

    _float  m_time = 0.f;
    _float2 m_baseAnchorOffset = {};

    string m_digits;

    _float m_scaleNow = 1.f;
    _float m_compactRatio = 1.f;

private:
    vector<CUI_AtlasSprite*> m_glyphs;

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg = {}) override;
};

NS_END