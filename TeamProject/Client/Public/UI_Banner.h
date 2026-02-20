#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_Banner final : public CUI_Object
{
public:
	typedef struct tagBannerDesc : public UI_DESC {
		wstring strTitle = L"";
		wstring strSubtitle = L"";
		function<void()> onClickConfirm = {};
		function<void()> onClickCancel = {};
	}BANNER_DESC;

private:
	enum class STATE { INVISIBLE, VISIBLE, END };

private:
	CUI_Banner() {}
	CUI_Banner(const CUI_Banner& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Banner() DEFAULT;

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

private:
	STATE m_eState = { STATE::END };
	class CTextSlot* m_pTitleTextSlot = { };
	class CTextSlot* m_pSubtitleTextSlot = { };

	function<void()> m_onClickConfirm = {};
	function<void()> m_onClickCancel = {};

private:
	void Cache();
	void Create_CancelButton();
	void Create_ConfirmButton(); 

	void Change_State(STATE eSate);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END