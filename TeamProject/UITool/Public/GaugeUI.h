#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CGaugeUI final : public CUIObject_Tool
{
private:
	CGaugeUI();
	CGaugeUI(const CGaugeUI& rhs);
	virtual ~CGaugeUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;

	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

	virtual void Render_GUI() override;

public:
	virtual void FillElementData(UI_ELEMENT_DATA& data) override;
	virtual void ReadElementData(const UI_ELEMENT_DATA& data) override;

private:
	_bool		m_isRadial = {};

	_float		m_fDirection = {};				// 0 : 오른쪽에서 왼쪽 / 1 : 왼쪽에서 오른쪽
	_float		m_fFillAmount = { 1.f };

private:
	string		m_strTextureKey = {};

public:
	static const string m_strTypeTag;
	static _uint m_iCount;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END