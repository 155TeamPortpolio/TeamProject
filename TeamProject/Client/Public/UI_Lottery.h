#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_Lottery final : public CUI_Object
{
public:
	enum STATE { READY, USED, END };

private:
	enum class CHILD { 
		BTN_REFRESH, BTN_SCRATCH,
		OVERLAY, OVERLAY_REFRESH,
		ICON_REFRESH, ICON_SCRATCH, 
		NEWSPAPER, SCRATCH, END };

	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { 
		"btnRefresh", "btnScratch", 
		"overlay", "overlayRefresh",
		"iconRefresh", "iconScratch", 
		"", ""};

	enum class BTN { BTN_SCRATCH, BTN_REFRESH, END };
	inline static const string BTN_NAMES[ENUM(BTN::END)] = { "btnScratch", "btnRefresh" };

private:
	CUI_Lottery() {}
	CUI_Lottery(const CUI_Lottery& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Lottery() DEFAULT;

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
	_uint			m_iState = { STATE::END };

	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};
	class CButtonUI* m_pButtons[ENUM(BTN::END)] = {};

private:
	void Cache(); 
	void Create_Newspaper();
	void Create_ScratchCard();
	void Create_BackButton();

	void Change_State(STATE eState);

	void OnClick_Back();
	void OnClick_RefreshNews();
	void OnClick_OpenScratch(); 

	void Set_ChildUIActive(CHILD child, void* pArg = nullptr);
	void Set_ChildUIDeActive(CHILD child, void* pArg = nullptr);
	void Set_ChildAnimation(CHILD child, _int iIndex);
	void Change_ChildTexture(CHILD child, const string& strTextureKey);

	_bool Is_ChildAlive(CHILD child);
	_bool Is_ChildAnimationFinished(CHILD child);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END