#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_RenderTargetScreen final : public CGameObject
{
private:
	CUI_RenderTargetScreen() {}
	CUI_RenderTargetScreen(const CUI_RenderTargetScreen& rhs) : CGameObject(rhs) {}
	virtual ~CUI_RenderTargetScreen() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()     override;
	virtual HRESULT Initialize(INIT_DESC* pArg) override;
	virtual void    Priority_Update(_float dt) override {}
	virtual void    Update(_float dt)          override;
	virtual void    Late_Update(_float dt)     override {}

private:
	_uint m_iMtrlInstIdx = {};

	_float2 m_vViewPortSize = {};

	_float4x4 m_ViewMatrix = {};
	_float4x4 m_ProjMatrix = {};

	const string strRTVTag = "renderTargetScreen";

private:
	HRESULT Ready_Components();
	void Ready_RTV();
	void Ready_ViewProj();
	void Ready_RenderState(); 
	HRESULT Ready_RTVDrawObjects();

	void Create_RTV();
	void Bind_RTV();
	HRESULT Create_RTVDrawObject(const string& strPrototypeTag, const string& strInstanceName);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END