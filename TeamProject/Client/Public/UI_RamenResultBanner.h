#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_RamenResultBanner final : public CUI_Object
{
public:
	typedef struct tagResultBanner : public UI_DESC {
		function<void()>	onClickConfirm = {};
	}RESULT_BANNER_DESC;

	typedef struct tagActiveDesc {
		wstring	strMenu = {};
	}ACTIVE_DESC;

private:
	enum class TEXTSLOT { LABEL1, LABEL2, END };
	inline static const string INSTANCENAMES[ENUM(TEXTSLOT::END)] = { "label1", "label2" };

	enum class STATE { INVISIBLE, VISIBLE, END };

private:
	CUI_RamenResultBanner() {}
	CUI_RamenResultBanner(const CUI_RamenResultBanner& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_RamenResultBanner() DEFAULT;

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

	class CTextSlot* m_pTextSlots[ENUM(TEXTSLOT::END)] = {};

private:
	void Cache();
	void Create_ConfirmButton();

	void Change_State(STATE eSate);
	void OnClick_Confirm();

	void Set_Text(TEXTSLOT textSlot, const _wstring& strText);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END