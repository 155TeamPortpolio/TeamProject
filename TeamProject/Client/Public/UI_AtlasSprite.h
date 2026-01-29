#pragma once

#include "UI_Object.h"
NS_BEGIN(Client)

class CUI_AtlasSprite final : public CUI_Object
{
public:
    struct ATLAS_DESC : public UI_DESC
    {
        string texKey;
        _uint  frameCountX = 1;
        _uint  frameCountY = 1;
        _uint  frameIdx = 0;
        _float heightPx = 32.f;
    };

private:
    CUI_AtlasSprite() {}
    CUI_AtlasSprite(const CUI_AtlasSprite& rhs) : CUI_Object(rhs) {}
    virtual ~CUI_AtlasSprite() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg = {}) override;
    void    Update(_float dt) override;

public:
    void Set_ColorAtlas(const string& texKey, _uint frameCountX, _uint frameCountY);
    void Set_ColorFrameIndex(_uint frameIdx);
    void Set_UseColorAtlas(_bool enable);

    void Set_ShearK(_float k);
    void Set_FlashMix(_float mix);
    void Set_ColorMix(_float mix);

    void Set_Atlas(const string& texKey, _uint frameCountX, _uint frameCountY);
    void Set_FrameIndex(_uint frameIdx);
    void Set_HeightPx(_float heightPx);

private:
    void Apply_Params();
    void Update_SizeByHeight();
    void OnPooledAcquire(INIT_DESC* pArg = {}) override;

private:
    string m_texKey{};
    _uint  m_frameCountX{};
    _uint  m_frameCountY{};
    _uint  m_frameIdx{};
    _float m_heightPx{};

    _uint  m_useColorAtlas{};
    string m_colorTexKey{};
    _uint  m_colorFrameCountX{};
    _uint  m_colorFrameCountY{};
    _uint  m_colorFrameIdx{};

    _float m_shearK{};
    _float m_flashMix{};
    _float m_colorMix{};

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg = {}) override;
};

NS_END
