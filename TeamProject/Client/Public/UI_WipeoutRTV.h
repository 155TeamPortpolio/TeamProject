#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_WipeoutRTV final : public CUI_Object
{
private:
	CUI_WipeoutRTV() {}
	CUI_WipeoutRTV(const CUI_WipeoutRTV& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_WipeoutRTV() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	/* Ä¿½ºÅÒ ·»´õÅ¸°Ù °ü·Ã º¯¼öµé */
	_float4x4		m_ViewMatrix = {};	
	_float4x4		m_ProjMatrix = {};	

private:
	void Render_RT(ID3D11DeviceContext* pContext);
	void Render_RTRecursive(class CGameObject* pObj, ID3D11DeviceContext* pContext);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END