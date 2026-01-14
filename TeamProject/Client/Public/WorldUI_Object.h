#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CWorldUI_Object abstract : public CUI_Object
{
protected:
	CWorldUI_Object() {}
	CWorldUI_Object(const CWorldUI_Object& rhs) : CUI_Object(rhs) {}
	virtual ~CWorldUI_Object() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override {}
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

protected:
	_float2			m_vScreenSize = {};
	_float3			m_vWorldPos = {};

private:
	void Update_WorldToScreen();

public:
	virtual void Free() { __super::Free(); }
};

NS_END