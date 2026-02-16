#pragma once
#include "UI_AnomalyBase.h"

NS_BEGIN(Client)

class CUI_AnomalyStack final : public CUI_AnomalyBase
{
private:
	CUI_AnomalyStack() {}
	CUI_AnomalyStack(const CUI_AnomalyStack& rhs) : CUI_AnomalyBase(rhs) {}
	virtual ~CUI_AnomalyStack() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	vector<class CUI_AnomalyStackSlot*> m_pSlots = {};

private:
	HRESULT Ready_Slots();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END