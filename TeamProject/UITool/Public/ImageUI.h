#pragma once
#include "UI_Object.h"

NS_BEGIN(UITool)

class CImageUI final : public CUI_Object
{
private:
	CImageUI();
	CImageUI(const CImageUI& rhs);
	virtual ~CImageUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;

	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

	virtual void Render_GUI() override;

private:
	_int		m_iTextureKeyIndex = {};

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END