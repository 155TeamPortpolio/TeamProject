#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CSoftDirectionalOutlineUI final : public CUIObject_Tool
{
private:
	CSoftDirectionalOutlineUI() {}
	CSoftDirectionalOutlineUI(const CSoftDirectionalOutlineUI& rhs) : CUIObject_Tool(rhs) {}
	virtual ~CSoftDirectionalOutlineUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)                override { __super::Update(dt); }
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

	_float2 m_vTexelSize = { 1.f, 1.f };		// 1 / textureSize
	_int m_iRadius = { 4 };						// 3(얇음) ~ 4(무난) ~ 6(네온)
	_float m_fGlowStrength = { 1.2f };			// 전체 강도 1(약간 약함) ~ 1.2(자연스럽) ~ 1.5이상(네온)
	_float2 m_vPaddingDir = _float2(1.0, 0.0);  // 퍼질 방향
	_float m_fGaussianPower = { 4.f };			// 자연스러운 감쇠 3(넓게 퍼짐) ~ 4(자연스러운 소프트) ~ 6(가장자리만 강함)

public:
	inline static const string m_strTypeTag = "SoftDirectionalOutline";
	inline static       _uint  m_iCount{};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override { __super::Free(); }
};

NS_END