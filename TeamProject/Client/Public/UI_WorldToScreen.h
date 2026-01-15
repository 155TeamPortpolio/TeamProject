#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_WorldToScreen abstract : public CUI_Object
{
protected:
	CUI_WorldToScreen() {}
	CUI_WorldToScreen(const CUI_WorldToScreen& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_WorldToScreen() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override {}
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	void Update_WorldToScreen();

public:
	virtual void Free() { __super::Free(); }
};

NS_END