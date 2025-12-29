#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CImageUI final : public CUIObject_Tool
{
private:
	CImageUI() {}
	CImageUI(const CImageUI& rhs) : CUIObject_Tool(rhs) {}
	virtual ~CImageUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override {}
	virtual void    Update(_float dt)                override;
	virtual void    Late_Update(_float dt)           override {}
	virtual void    Render_GUI()                     override;

public:
	virtual void FillElementData(UI_ELEMENT_DATA& data) override;
	virtual void ReadElementData(const UI_ELEMENT_DATA& data) override;

private:
	string		m_strTextureKey = {};

public:
	static const string m_strTypeTag;
	static _uint m_iCount;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free() { __super::Free(); }
};

NS_END