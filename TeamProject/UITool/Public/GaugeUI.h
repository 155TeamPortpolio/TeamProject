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
	virtual void ToJson(json& data) override;
	virtual void FromJson(const json& data) override;

	virtual void SavePrefab(json& data) override;
	virtual void LoadPrefab(const json& data) override;

private:
	_bool		m_isRadial = {};

	_float		m_fDirection = {};				// 0 : 오른쪽에서 왼쪽 / 1 : 왼쪽에서 오른쪽
	_float		m_fFillAmount = { 1.f };

	_int		m_iTextureKeyIndex = { 0 };		// gui에 콤보박스에서 텍스쳐 선택했을 때 인덱스

public:
	static const string m_strTypeTag;
	static _uint m_iCount;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END