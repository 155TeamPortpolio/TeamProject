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
    virtual void    Update(_float dt)                        override;
    virtual void    Render_GUI()                             override;

public:
    virtual void    Save(nlohmann::ordered_json& data)       override{}
    virtual void    Load(const nlohmann::ordered_json& data) override{}

public:
    inline static const string m_strTypeTag = "MaskUI";
    inline static       _uint  m_iCount{};

private:
    string m_strTextureKey{};
    _bool  m_previewVisible = true;

public:
    static  CGameObject* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
    virtual void Free() override { __super::Free(); }
};

NS_END