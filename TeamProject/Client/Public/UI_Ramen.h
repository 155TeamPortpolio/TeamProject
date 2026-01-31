#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_Ramen final : public CUI_Object
{
private:
	inline static const _int MAX_MENU_COUNT = 9;

	enum class CHILD { TEXT_PRICE, ORDER, ICON_ORDER, DISABLE_ORDER, CLICK_ORDER, TEXT_ORDER, BTN_ORDER, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "textPrice", "order", "iconOrder", "disableOrder", "clickOrder", "textOrder", "btnOrder" };

private:
	CUI_Ramen() {}
	CUI_Ramen(const CUI_Ramen& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Ramen() DEFAULT;

public:
	void Select_Menu(CUI_Object* pSelected, _int iPrice);

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

	CUI_Object* m_pChildren[ENUM(CHILD::END)] = {};
	class CTextSlot* m_pTextPrice = {};
	class CButtonUI* m_pButtonOrder = {};

	_int			m_iMoney = {};
	_int			m_iPrice = {};

	_bool			m_isAffordable = {};

private: 
	void Create_ButtonBack();
	void Create_Menus();
	void Cache();

	void OnClick_Order();
	void Set_TextPrice(_int iMoney, _int iPrice);
	void Update_Affordable();

	void Set_ChildAnimation(CHILD child, _int iIndex);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END