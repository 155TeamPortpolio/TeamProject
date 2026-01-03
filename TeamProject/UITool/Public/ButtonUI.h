#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CButtonUI final : public CUIObject_Tool
{
public:
	enum class STATE { NORMAL, HOVERED, CLICKED, DISABLED, END };

	typedef struct tagButtonEvent {
		wstring msg = L"";
	}BTN_EVENT;

private:
	CButtonUI() {}
	CButtonUI(const CButtonUI& rhs) : CUIObject_Tool(rhs) {}
	virtual ~CButtonUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()                override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;
	virtual void Awake() override;
	virtual void Priority_Update(_float dt) override {}
	virtual void Update(_float dt)          override;
	virtual void Late_Update(_float dt)     override {}

	virtual void Render_GUI()  override;

	virtual void Enter_Hover() override;
	virtual void Exit_Hover()  override;
	virtual void OnClick()     override;

public:
	virtual void Save(nlohmann::ordered_json& data) override;
	virtual void Load(const nlohmann::ordered_json& data) override;

private:
	STATE		m_eState = {};
	_char		m_szEventMsg[MAX_PATH] = {};	// 클릭했을 때 내보내는 메시지

private:
	string		m_strTextureKey = {};

	_bool		m_isFlipX = {};
	_bool		m_isFlipY = {};
	_float2		m_vFlip = {};

public:
	static const string m_strTypeTag;
	static _uint m_iCount;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free() { __super::Free(); }
};

NS_END