#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)

class CUI_ScreenFade final : public CUI_Object
{
private:
	CUI_ScreenFade() {}
	CUI_ScreenFade(const CUI_ScreenFade& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_ScreenFade() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	void Ready_FadeIn();
	void Ready_FadeOut();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END