#pragma once
#include "UI_RTVDraw.h"

NS_BEGIN(Client)

class CUI_Clear final : public CUI_RTVDraw
{
private:
	CUI_Clear() {}
	CUI_Clear(const CUI_Clear& rhs) : CUI_RTVDraw(rhs) {}
	virtual ~CUI_Clear() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void UI_Active(void* pArg = nullptr) override;

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END