#pragma once

#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CMaskUI final : public CUIObject_Tool
{
private:
    CMaskUI() {}
    CMaskUI(const CMaskUI& rhs) : CUIObject_Tool(rhs) {}
    virtual ~CMaskUI() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()                   override;
    virtual HRESULT Initialize(INIT_DESC* pArg = {})         override;
    virtual void    Priority_Update(_float dt)               override { __super::Priority_Update(dt); }
    virtual void    Update(_float dt)                        override { __super::Update(dt); }
    virtual void    Late_Update(_float dt)                   override { __super::Late_Update(dt); }
    virtual void    Render_GUI()                             override;

public:
    virtual void    Save(nlohmann::ordered_json& data)       override;
    virtual void    Load(const nlohmann::ordered_json& data) override;

public:
    inline static const string m_strTypeTag = "Mask";
    inline static       _uint  m_iCount{};

private:
    string m_strTextureKey{};
    _bool  m_previewVisible = true;
    _float m_previewAlpha   = 0.5f;

public:
    static  CGameObject* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
    virtual void Free() override { __super::Free(); }
};

NS_END