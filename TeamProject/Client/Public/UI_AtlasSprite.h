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
        _uint  frameIndex = 0;

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
    void Set_Atlas(const string& textureKey, _uint frameCountX, _uint frameCountY);
    void Set_FrameIndex(_uint frameIndex);
    void Set_HeightPx(_float heightPx);

private:
    void Apply_Params();
    void Update_SizeByHeight();

private:
    string m_texKey;

    _uint  m_frameCountX = 1;
    _uint  m_frameCountY = 1;
    _uint  m_frameIndex = 0;

    _float m_heightPx = 32.f;

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg = {}) override;
    void Free() override { __super::Free(); }
};

NS_END