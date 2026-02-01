#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_LotteryResultBanner final : public CUI_Object
{
public:
	typedef struct tagResultDesc {
		_int iDenny = {};
	}RESULT_DESC;

private:
	enum class TEXTSLOT { DENY, END };
	inline static const string INSTANCENAMES[ENUM(TEXTSLOT::END)] = { "deny" };

	enum class STATE { INVISIBLE, VISIBLE, END };

private:
	CUI_LotteryResultBanner() {}
	CUI_LotteryResultBanner(const CUI_LotteryResultBanner& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_LotteryResultBanner() DEFAULT;

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