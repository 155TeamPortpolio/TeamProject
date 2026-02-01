#pragma once

#include "UI_WorldToScreen.h"

NS_BEGIN(Client)
class CUI_Gangta final : public CUI_WorldToScreen
{
private:
	CUI_Gangta() {}
	CUI_Gangta(const CUI_Gangta& rhs) : CUI_WorldToScreen(rhs) {}
	virtual ~CUI_Gangta() DEFAULT;

public:
	HRESULT Initialize_Prototype()          override;
	HRESULT Initialize(INIT_DESC* arg = {}) override;
	void	Awake()							override{}
	void    Priority_Update(_float dt)      override { __super::Priority_Update(dt); }
	void    Update(_float dt)			    override;
	void    Late_Update(_float dt)          override { __super::Late_Update(dt); }
	void    Render_GUI()                    override { __super::Render_GUI(); }

public:
	static CUI_Gangta* Create();
	CGameObject* Clone(INIT_DESC* pArg = {}) override;
};
NS_END