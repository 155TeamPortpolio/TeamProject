#pragma once
#include "UI_Object.h"

NS_BEGIN(UITool)

class CTextUI final : public CUI_Object
{
private:
	CTextUI();
	CTextUI(const CTextUI& rhs);
	virtual ~CTextUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;

	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

	virtual void Render_GUI() override;

private:
	_int		m_iFontKeyIndex = {};
	_char		m_szText[MAX_PATH] = {};
	_float4		m_vColor = { 1.f, 1.f, 1.f, 1.f };

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END