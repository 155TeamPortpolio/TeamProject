#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
class CTextSlot;
NS_END 

NS_BEGIN(Client)

class CUI_RamenMenu final : public CUI_Object
{
public:
	typedef struct tagRamenMenuDesc : public UI_DESC {
		RAMEN_DESC		tRamenDesc = {};
		function<void(CUI_Object* pObj)>	onSelect = {};
	}RAMENMENU_DESC;

private:
	enum class CHILD { ACTIVE, ICON_MENU, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "active", "iconMenu" };

	enum class TEXTSLOT { NAME, PRICE, END };
	inline static const string TEXT_INSTANCENAMES[ENUM(TEXTSLOT::END)] = { "name", "price" };

	inline static const unordered_map<string, string> ICON_MENU_TEXTURES = {
		{"ramen_white_veggie", "IconRamen01.png"},
		{"ramen_white_zucchini", "IconRamen02.png"},
		{"ramen_white_fried_chashu", "IconRamen03.png"},
		{"ramen_white_redpepper_meat", "IconRamen04.png"},
		{"ramen_white_greenpepper_meat", "IconRamen05.png"},
		{"ramen_white_seafood", "IconRamen06.png"},
		{"ramen_black_mushroom", "IconRamen07.png"},
		{"ramen_black_smoked_chashu", "IconRamen08.png"},
		{"ramen_black_redpepper_chicken", "IconRamen09.png"},
		{"ramen_black_greenpepper_chicken", "IconRamen10.png"},
		{"ramen_black_cold_seafood", "IconRamen11.png"},
		{"ramen_black_bibim", "IconRamen12.png"},
		{"rmane_black_bone", "IconRamen13.png"},
	};

private:
	CUI_RamenMenu() {}
	CUI_RamenMenu(const CUI_RamenMenu& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_RamenMenu() DEFAULT;

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
	RAMEN_DESC		m_tRamenDesc = {};
	function<void(CUI_Object* pObj)>	m_onSelect = {};

	class CUI_Object*	m_pChildren[ENUM(CHILD::END)] = {};
	class CTextSlot*	m_pTexts[ENUM(TEXTSLOT::END)] = {};
	class CButtonUI*	m_pButton = {};

	vector<CUI_Object*> m_Attributes;

private:
	void Cache();
	void Create_AttributeIcons();

	void Set_ChildAlive(CHILD child, _bool isAlive);
	void Set_ChildAlpha(CHILD child, _float fAlpha);
	void Set_ChildTexture(CHILD child, const string& strTextureKey);
	void Set_Text(TEXTSLOT text, const _wstring& strText);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END