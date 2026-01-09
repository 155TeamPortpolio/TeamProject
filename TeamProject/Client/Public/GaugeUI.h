#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CGaugeUI final : public CUI_Object
{
private:
	CGaugeUI() {}
	CGaugeUI(const CGaugeUI& rhs) : CUI_Object(rhs) {}
	virtual ~CGaugeUI() DEFAULT;

public:
	void Set_GaugeDesc(GAUGE_OWNER eOwner, GAUGE_TYPE eType);

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)                override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override;

public:
	virtual void Load(const nlohmann::ordered_json& data) override;

private:
	_float m_fFillAmount = 1.f;
	_float m_fDirection  = 1.f;

	GAUGE_OWNER	m_eOwner = { GAUGE_OWNER::END };
	GAUGE_TYPE	m_eType = { GAUGE_TYPE::END };

private:
	void Set_Gauge(const GAUGE_DESC& desc);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override { __super::Free(); }
};

NS_END