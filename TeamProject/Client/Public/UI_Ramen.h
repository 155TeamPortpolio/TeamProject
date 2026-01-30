#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_Ramen final : public CUI_Object
{
private:
	enum CHILD { BTN_BACK, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "" };
	inline static const _int MAX_MENU_COUNT = 4;

	enum class BTN { BTN_BACK, BTN_ORDER, END };
	inline static const string BTN_NAMES[ENUM(BTN::END)] = { "btnBack", "btnOrder" };

private:
	CUI_Ramen() {}
	CUI_Ramen(const CUI_Ramen& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Ramen() DEFAULT;

public:
	void Select_Menu(CUI_Object* pSelected);

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override;
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg)			 override;
	virtual void	UI_DeActive(void* pArg)			 override;

private:
	CUI_Object*		m_pMenus[MAX_MENU_COUNT] = {};
	CUI_Object*		m_pSelectedMenu = {};

	class CButtonUI* m_pButtons[ENUM(BTN::END)] = {};

private:
	void Cache();
	void Create_Menus();

	void OnClick_Back();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END