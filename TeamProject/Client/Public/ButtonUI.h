#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CButtonUI final : public CUI_Object
{
public:
	enum class STATE { NORMAL, HOVERED, CLICKED, DISABLED, END };

	typedef struct tagButtonEvent {
		wstring msg = L"";
	}BTN_EVENT;

private:
	CButtonUI() {}
	CButtonUI(const CButtonUI& rhs) : CUI_Object(rhs) {}
	virtual ~CButtonUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Awake()                          override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)                override { __super::Update(dt); }
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

public:
	virtual void    Enter_Hover() override;
	virtual void    Exit_Hover()  override;
	virtual void    OnClick()     override;

public:
	virtual void    Load(const nlohmann::ordered_json& data) override;

private:
	STATE m_eState{};
	_char m_szEventMsg[MAX_PATH]{};	

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override { __super::Free(); }
};

NS_END