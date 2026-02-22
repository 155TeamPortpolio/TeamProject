#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_WipeoutOverlay final : public CUI_Object
{
private:
	enum class STATE { VISIBLE, FINISHED, INVISIBLE, END };

private:
	CUI_WipeoutOverlay() {}
	CUI_WipeoutOverlay(const CUI_WipeoutOverlay& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_WipeoutOverlay() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr) override;
	virtual void	UI_DeActive(void* pArg = nullptr) override;

	virtual _bool Is_AnimFinished() override;

private:
	STATE m_eState = { STATE::END };
	CUI_Object* m_pSpriteAnimation = {}; 

private:
	void Change_State(STATE eState);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END