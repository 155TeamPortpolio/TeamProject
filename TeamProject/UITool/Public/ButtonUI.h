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
	_int			m_iState = {};
	string			m_strTextureKeys[static_cast<_int>(STATE::END)];
	_int			m_iTextureKeyIndices[static_cast<_int>(STATE::END)] = {};

public:
	static _uint m_iCount;

private:
	void Render_GUI_Texture(STATE eState, const char* label, const vector<const _char*>& szTextureKeys, const string& levelKey);

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END