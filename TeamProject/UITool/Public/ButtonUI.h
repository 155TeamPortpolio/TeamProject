#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CButtonUI final : public CUIObject_Tool
{
public:
	enum class STATE { NORMAL, HOVERED, CLICKED, DISABLED, END };

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

	virtual void Enter_Hover() override;
	virtual void Exit_Hover() override;
	virtual void OnClick() override;

public:
	virtual void ToJson(json& data) override;
	virtual void FromJson(const json& data) override;

	virtual void SavePrefab(json& data) override;
	virtual void LoadPrefab(const json& data) override;

public:
	STATE		m_eState = {};
	_char		m_szEventMsg[MAX_PATH] = {};	// 클릭했을 때 내보내는 메시지

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