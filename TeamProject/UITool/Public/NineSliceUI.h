#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CNineSliceUI final : public CUIObject_Tool
{
private:
	CNineSliceUI() {}
	CNineSliceUI(const CNineSliceUI& rhs) : CUIObject_Tool(rhs) {}
	virtual ~CNineSliceUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)                override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override;

public:
	virtual void	Save(nlohmann::ordered_json& data)       override;
	virtual void	Load(const nlohmann::ordered_json& data) override;

private:
	string		m_strTextureKey{};

	_bool		m_isFlipX{};
	_bool		m_isFlipY{};
	_float2		m_vFlip{};

	_bool		m_isNineSliceDirty = {};

	_float4		m_vBorderPx = { };	// left, right, top, bottom (픽셀)
	_float4		m_uvRangeX = {};	// L0, L1, R0, R1 (CPU에서 계산해서 넘김)
	_float4		m_uvRangeY = {};	 // T0, T1, B0, B1 (CPU에서 계산해서 넘김)
	_float2		m_vTextureSize = {};

private:
	_bool Render_GUI_NineSlice();

public:
	inline static const string m_strTypeTag = "NineSlice";
	inline static       _uint  m_iCount{};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override { __super::Free(); }
};

NS_END