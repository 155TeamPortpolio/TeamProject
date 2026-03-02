#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_RamenOrderBanner final : public CUI_Object
{
public:
	typedef struct tagOrderBannerDesc : public UI_DESC {
		function<void()>	onOrderComfirm = {};
	}ORDER_BANNER_DESC;

	typedef struct tagActiveDesc {
		wstring strMenu = {}; 
	}ACTIVE_DESC;

private:
	enum class STATE { INVISIBLE, VISIBLE, END };

private:
	CUI_RamenOrderBanner() {}
	CUI_RamenOrderBanner(const CUI_RamenOrderBanner& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_RamenOrderBanner() DEFAULT;

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
	STATE			m_eState = { STATE::END };
	class CTextSlot* m_pLabelTextSlot = { };

private:
	void Create_CancelButton();
	void Create_ConfirmButton();
	void Cache();

	void Change_State(STATE eSate);
	void OnClick_Cancel();
	void OnClick_Confirm();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END