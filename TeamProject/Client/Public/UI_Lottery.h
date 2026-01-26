#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_Lottery final : public CUI_Object
{
private:
	CUI_Lottery() {}
	CUI_Lottery(const CUI_Lottery& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Lottery() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override;
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg)			 override;

private:

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END