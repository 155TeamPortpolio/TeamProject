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
	virtual void Enter_Hover() override;
	virtual void Exit_Hover() override;
	virtual void OnClick() override;

public:
	virtual void Load(const nlohmann::ordered_json& data) override;

private:
	STATE		m_eState = {};
	_char		m_szEventMsg[MAX_PATH] = {};	// 클릭했을 때 내보내는 메시지

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END