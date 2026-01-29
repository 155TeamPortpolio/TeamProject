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
		function<void(CUI_Object* pObj)>	onSelect = {};
	}RAMENMENU_DESC;

private:
	enum class SPRITE { BUTTON, ICON_MENU, ICON_ATT1, ICON_ATT2, END };
	inline static const string SPRITE_INSTANCENAMES[ENUM(SPRITE::END)] = { "button", "iconMenu", "iconAttribute1", "iconAttribute2" };

	enum class TEXT { NAME, PRICE, END };
	inline static const string TEXT_INSTANCENAMES[ENUM(SPRITE::END)] = { "name", "price" };

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
	function<void(CUI_Object* pObj)>	m_onSelect = {};
	 
	class CSprite2D*	m_pSprites[ENUM(SPRITE::END)] = {};
	class CTextSlot*	m_pTexts[ENUM(TEXT::END)] = {};
	class CButtonUI*	m_pButton = {};
	class CUI_Object*	m_pOverlay = {};

private:
	void Cache();

	void Change_Sprite(SPRITE sprite, _uint iIndex);
	void Set_Text(TEXT text, const _wstring& strText);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END