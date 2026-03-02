#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)

class CUI_HUD abstract : public CUI_Object
{
public:
	typedef struct tagTransitionDesc {
		_bool isFade = {};
	}UI_TRANSITION_DESC;

protected:
	CUI_HUD() {}
	CUI_HUD(const CUI_HUD& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_HUD() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override {}
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

public:
	virtual void Free() { __super::Free(); }
};

NS_END