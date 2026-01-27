#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_Lottery final : public CUI_Object
{
private:
	enum class CHILD { 
		BTN_BACK, BTN_SCRATCH, BTN_REFRESH, 
		OVERLAY_BACK,
		ICON_BACK, ICON_SCRATCH, 
		NEWS, NEWS1, NEWS2, 
		SCRATCH, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { 
		"btnBack", "btnScratch", "btnRefresh", 
		"overlayBack",
		"iconBack", "iconScratch", 
		"news", "news1", "news2", "" };

	enum class BTN { BTN_BACK, BTN_SCRATCH, BTN_REFRESH, END };
	inline static const string BTN_NAMES[ENUM(BTN::END)] = { "btnBack", "btnScratch", "btnRefresh" };

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

private:
	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};
	class CButtonUI* m_pButtons[ENUM(BTN::END)] = {};

private:
	void Cache();

	void OnClick_Back();
	void OnClick_RefreshNews();
	void OnClick_OpenScratch(); 

	void Set_ChildUIActive(CHILD child, void* pArg = nullptr);
	void Set_ChildUIDeActive(CHILD child, void* pArg = nullptr);
	void Set_ChildAnimation(CHILD child, _int iIndex);

	_bool Is_ChildAlive(CHILD child);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END