#pragma once

#include "UI_AtlasSprite.h"
#include "UI_WorldToScreen.h"

NS_BEGIN(Client)

struct DAMAGE_DESC : public UI_DESC
{
    Vector3       pos{};
    OBJECT_HANDLE followHandle{};
    Vector3       followOffset{};
    _int          damage{};
    CHARACTER     charaName{};
    _bool         isEnemy{};
};

class CUI_DamageText final : public CUI_WorldToScreen
{
private:
    CUI_DamageText() {}
    CUI_DamageText(const CUI_DamageText& rhs);
    virtual ~CUI_DamageText() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* arg = {}) override;
    void    Update(_float dt) override;
    void    UI_Active(void* arg = {}) override;
    void    UI_DeActive(void* arg = {}) override;

private:
    void  SetDamage(_int damage);
    void  Ensure_GlyphCount(_uint count);
    void  Rebuild_BaseLayout();
    void  Apply_LayoutScaled();
    void  Update_Anim(_float dt);
    void  OnPooledAcquire(INIT_DESC* pArg = {}) override;
    void  OnPooledRelease() override {}

    _uint GetDigitFrameIdx(_uint digit) const;
    CUI_AtlasSprite* GetGlyph(_uint i) const { return m_glyphs[i]; }

    _uint  GetBangFrameIdx() const;
    _float GetCharWidthPx(char ch) const;
    _float GetPairExtraTightPx(char left, char right) const;

private:
    _float  m_glyphAspect = 1.f;
    _float  m_time = 0.f;
    Vector2 m_baseAnchorOffset = Vector2(0.f, 0.f);
    _float3 m_worldPos{};
    string  m_digits;
    _float  m_baseTotalW = 1.f;
    vector<Vector2> m_baseOffsets;

    OBJECT_HANDLE m_followHandle{};
    Vector3       m_followOffset{};
    Vector2       m_spawnOffsetPx{};

    _uint  m_colorFrameIdx = 0;
    _float m_damageScale = 1.f;
    _float m_shearK = 0.f;

    vector<CUI_AtlasSprite*> m_glyphs;

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg = {}) override;
};

NS_END