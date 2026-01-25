#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CSpriteAnimationUI : public CUI_Object
{
private:
	CSpriteAnimationUI() {}
	CSpriteAnimationUI(const CSpriteAnimationUI& rhs) : CUI_Object(rhs) {}
	virtual ~CSpriteAnimationUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)                override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

public:
	virtual void    Load(const nlohmann::ordered_json& data) override;

private:
	_bool		m_isPlaying = { true };
	_bool		m_isLoop = {};

	_uint		m_iFrameCountX = { 1 };
	_uint		m_iFrameCountY = { 1 };
	_uint		m_iFrameCountTotal = { 1 };
	_float		m_fFrameSpeed = { 30.f };

	_float		m_fFrameAccTime = {};
	_uint		m_iCurrentFrameIndex = {};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override { __super::Free(); }
};

NS_END