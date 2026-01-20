#pragma once
#include "UI_WorldToScreen.h"

NS_BEGIN(Client)

class CUI_NameIndicator final : public CUI_WorldToScreen
{
private:
	CUI_NameIndicator() {}
	CUI_NameIndicator(const CUI_NameIndicator& rhs) : CUI_WorldToScreen(rhs) {}
	virtual ~CUI_NameIndicator() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override {}
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END