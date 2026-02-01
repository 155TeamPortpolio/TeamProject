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
		wstring strMenu = {};
		vector<RAMEN_ATTRIBUTE> attributes;
	}ACTIVE_DESC;

private:
	enum class ATTR { LABEL, NAME1, VALUE1, NAME2, VALUE2, NAME3, VALUE3, END };

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

	class CTextSlot* m_pLabelTextSlot = {};

	class CUI_Object* m_pAttrObjects[ENUM(ATTR::END)] = {};
	class CTextSlot* m_pAttrTextSlots[ENUM(ATTR::END)] = {};

private:
	void Cache();
	void Create_ConfirmButton();
	void Create_AttrTexts();

	void Create_AttrText(CUI_Object** ppOutObj, class CTextSlot** ppOutTextSlot, _bool isHighlighted = false);

	void Change_State(STATE eSate);
	void OnClick_Confirm();

	void Refresh_Layout();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END