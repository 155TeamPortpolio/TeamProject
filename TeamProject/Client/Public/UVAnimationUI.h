#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUVAnimationUI final : public CUI_Object
{
private:
	CUVAnimationUI() {}
	CUVAnimationUI(const CUVAnimationUI& rhs) : CUI_Object(rhs) {}
	virtual ~CUVAnimationUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()                override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;
	virtual void    Priority_Update(_float dt)            override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)                     override;
	virtual void    Late_Update(_float dt)                override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                          override { __super::Render_GUI(); }

public:
	virtual void    Load(const nlohmann::ordered_json& data) override;

private:
	_float2		m_vUVOffset = {};
	_float2		m_vUVOffsetSpeed = {};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free() override { __super::Free(); }
};

NS_END