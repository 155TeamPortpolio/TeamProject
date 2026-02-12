#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_RTVDraw : public CUI_Object
{
protected:
	CUI_RTVDraw() {}
	CUI_RTVDraw(const CUI_RTVDraw& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_RTVDraw() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

protected:
	void Update_RTV(const string& strTargetKey, _bool isClear = true);

private:
	_float4x4		m_ViewMatrix = {};
	_float4x4		m_ProjMatrix = {};

private:
	void Render_RT(ID3D11DeviceContext* pContext);
	void Render_RTRecursive(class CGameObject* pObj, ID3D11DeviceContext* pContext);

public:
	virtual void Free() { __super::Free(); }
};

NS_END