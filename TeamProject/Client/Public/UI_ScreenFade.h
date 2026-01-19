#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)

class CUI_ScreenFade final : public CUI_Object
{
public:
	typedef struct tagFadeDesc {
		_float fDuration = { 0.5f };
	}FADE_DESC;

private:
	CUI_ScreenFade() {}
	CUI_ScreenFade(const CUI_ScreenFade& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_ScreenFade() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr)  override;
	virtual void	UI_DeActive(void* pArg = nullptr) override;

private:
	_int			m_iFadeInIndex = { -1 };
	_int			m_iFadeOutIndex = { -1 };

	static constexpr _float m_fInitDuration = { 0.5f }; 

private:
	void Ready_FadeIn();
	void Ready_FadeOut();
	_int Register_AnimClip(UI_ANIM_CLIP& clip);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END