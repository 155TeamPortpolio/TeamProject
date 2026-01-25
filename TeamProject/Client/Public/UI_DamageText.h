#pragma once

#include "UI_AtlasSprite.h"
#include "UI_WorldToScreen.h"

NS_BEGIN(Client)

class CUI_DamageText final : public CUI_WorldToScreen
{
public:
    struct DAMAGE_DESC : public UI_DESC
    {
        Vector3 pos{};
        _int    damage = 0;
    };

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
    void SetDamage(_int damage);

private:
    void Ensure_GlyphCount(_uint count);

    void Rebuild_BaseLayout();
    void Apply_LayoutScaled();
    void Update_Anim(_float dt);
    virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr) {}		// 풀에서 꺼낼 때
    virtual void OnPooledRelease() {}								// 풀로 돌아갈 때

    _uint GetDigitFrameIdx(_uint digit) const;
    CUI_AtlasSprite* GetGlyph(_uint i) const { return m_glyphs[i]; }

private:
    _float  m_glyphAspect = 1.f;

    _float  m_time = 0.f;
    Vector2 m_baseAnchorOffset = Vector2(0.f, 0.f);

    _float3 m_worldPos{};

    string m_digits;

    _float m_baseTotalW = 1.f;
    vector<Vector2> m_baseOffsets;

    vector<CUI_AtlasSprite*> m_glyphs;

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg = {}) override;
};

NS_END