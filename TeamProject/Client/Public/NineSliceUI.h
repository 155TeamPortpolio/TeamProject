#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CNineSliceUI final : public CUI_Object
{
private:
	CNineSliceUI() {}
	CNineSliceUI(const CNineSliceUI& rhs) : CUI_Object(rhs) {}
	virtual ~CNineSliceUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

public:
	virtual void Load(const nlohmann::ordered_json& data) override;

private:
	_float4		m_vBorderPx = { };	// left, right, top, bottom (픽셀)
	_float4		m_uvRangeX = {};	// L0, L1, R0, R1 (CPU에서 계산해서 넘김)
	_float4		m_uvRangeY = {};	 // T0, T1, B0, B1 (CPU에서 계산해서 넘김)
	_float2		m_vTextureSize = {};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END