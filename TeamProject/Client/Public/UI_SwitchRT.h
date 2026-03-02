#pragma once
#include "UI_ScreenRT.h"

NS_BEGIN(Client)

class CUI_SwitchRT final : public CUI_ScreenRT
{
private:
	CUI_SwitchRT() {}
	CUI_SwitchRT(const CUI_SwitchRT& rhs) : CUI_ScreenRT(rhs) {}
	virtual ~CUI_SwitchRT() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void UI_Active(void* pArg = nullptr) override;
	virtual void UI_DeActive(void* pArg = nullptr) override;

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END