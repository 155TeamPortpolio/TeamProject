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
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)                override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

public:
	virtual void Load(const nlohmann::ordered_json& data) override;

private:
	_float m_fFillAmount = 1.f;
	_float m_fDirection  = 1.f;

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override { __super::Free(); }
};

NS_END