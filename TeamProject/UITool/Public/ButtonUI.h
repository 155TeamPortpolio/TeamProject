#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CButtonUI final : public CUIObject_Tool
{
public:
	enum class STATE { NORMAL, CLICKED, DISABLED, END };

private:
	CButtonUI();
	CButtonUI(const CButtonUI& rhs);
	virtual ~CButtonUI() DEFAULT;

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

public:
	_int		m_iState = {};
	string		m_strTextureKey;
	_int		m_iTextureKeyIndex = { 0 };		// gui에 콤보박스에서 텍스쳐 선택했을 때 인덱스

public:
	static _uint m_iCount;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END