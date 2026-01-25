#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_ScratchCard final : public CUI_Object
{
private:
	CUI_ScratchCard() {}
	CUI_ScratchCard(const CUI_ScratchCard& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_ScratchCard() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
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