#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_PartyCardRTDraw final : public CUI_Object
{
public:
	typedef struct tagCardDesc : public UI_DESC {
		string strRenderTargetKey = "";
	}CARD_DESC;

private:
	CUI_PartyCardRTDraw() {}
	CUI_PartyCardRTDraw(const CUI_PartyCardRTDraw& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_PartyCardRTDraw() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr) override;

private:
	_bool m_isFadeIn = {};

	_float m_fFadeTimer = {};
	const _float m_fFadeDuration = { 0.4f };

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END