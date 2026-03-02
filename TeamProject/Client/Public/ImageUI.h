#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CImageUI final : public CUI_Object
{
private:
    CImageUI() {}
    CImageUI(const CImageUI& rhs) : CUI_Object(rhs) {}
    virtual ~CImageUI() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()           override;
    virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
    virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
    virtual void    Update(_float dt)                override;
    virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
    virtual void    Render_GUI()                     override { __super::Render_GUI(); }

public:
    virtual void Load(const nlohmann::ordered_json& data) override;

private:
    string  m_colorTextureKey = "empty.png";
    _float  m_colorTexMix = 1.f;
    _uint   m_colorTexModeU = 0;

    _uint   m_colorUVUseU = 0;
    Vector2 m_colorUVOffset = Vector2(0.f, 0.f);
    Vector2 m_colorUVScale = Vector2(1.f, 1.f);

    _bool   m_colorUVAutoScroll = false;
    Vector2 m_colorUVSpeed = Vector2(0.f, 0.f);

public:
    static  CGameObject* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
    virtual void Free() { __super::Free(); }
};


NS_END