#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_BattleHUD final : public CUI_Object
{
	enum Child {
		ICON1, ICON2, ICON3,
		HP_BACK1, HP_BACK2, HP_BACK3,
		HP_FRONT1, HP_FRONT2, HP_FRONT3,
		SPECIAL1, SPECIAL2, SPECIAL3,
		ULTIMATE1, ULTIMATE2, ULTIMATE3, 
		CUR_HP_TEXT, MAX_HP_TEXT,
		 
		BOSS_ICON, BOSS_HP_BACK, BOSS_HP_FRONT, BOSS_GROGGY, BOSS_GROGGY_TEXT,
		ACTION,
		END };

	struct tagGaugeBindDesc {
		Child			eChild;
		UI_STATUS_OWNER	eOwner;
		UI_STATUS_TYPE	eType;
	};

	inline static constexpr Child ICON_CHILD[] = { ICON1, ICON2, ICON3 };
	inline static constexpr Child HPBACK_CHILD[] = { HP_BACK1, HP_BACK2, HP_BACK3 };
	inline static constexpr Child HPFRONT_CHILD[] = { HP_FRONT1, HP_FRONT2, HP_FRONT3 };
	inline static constexpr Child SPECIAL_CHILD[] = { SPECIAL1, SPECIAL2, SPECIAL3 };
	inline static constexpr Child ULTIMATE_CHILD[] = { ULTIMATE1, ULTIMATE2, ULTIMATE3 };
	inline static constexpr tagGaugeBindDesc GaugeBindings[] = {
		{ HP_BACK1, UI_STATUS_OWNER::ROLE1, UI_STATUS_TYPE::HP_BACK },
		{ HP_BACK2, UI_STATUS_OWNER::ROLE2, UI_STATUS_TYPE::HP_BACK },
		{ HP_BACK3, UI_STATUS_OWNER::ROLE3, UI_STATUS_TYPE::HP_BACK },

		{ HP_FRONT1, UI_STATUS_OWNER::ROLE1, UI_STATUS_TYPE::HP },
		{ HP_FRONT2, UI_STATUS_OWNER::ROLE2, UI_STATUS_TYPE::HP },
		{ HP_FRONT3, UI_STATUS_OWNER::ROLE3, UI_STATUS_TYPE::HP },

		{ SPECIAL1, UI_STATUS_OWNER::ROLE1, UI_STATUS_TYPE::SPECIAL },
		{ SPECIAL2, UI_STATUS_OWNER::ROLE2, UI_STATUS_TYPE::SPECIAL },
		{ SPECIAL3, UI_STATUS_OWNER::ROLE3, UI_STATUS_TYPE::SPECIAL },

		{ ULTIMATE1, UI_STATUS_OWNER::ROLE1, UI_STATUS_TYPE::ULTIMATE },
		{ ULTIMATE2, UI_STATUS_OWNER::ROLE2, UI_STATUS_TYPE::ULTIMATE },
		{ ULTIMATE3, UI_STATUS_OWNER::ROLE3, UI_STATUS_TYPE::ULTIMATE },

		{ BOSS_HP_FRONT, UI_STATUS_OWNER::BOSS, UI_STATUS_TYPE::HP },
		{ BOSS_GROGGY, UI_STATUS_OWNER::BOSS, UI_STATUS_TYPE::GROGGY },
	};

private:
	CUI_BattleHUD() {}
	CUI_BattleHUD(const CUI_BattleHUD& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_BattleHUD() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	UI_HANDLE			m_hRoot;
	vector<UI_HANDLE>	m_handles; 

private:
	CUI_Object* Ready_Prefab(const string& strLevelKey);

	void Add_PartObject(CUI_Object* pRoot, const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, Child child, _float2 vOffset = _float2());
	void Cache_Handles(CUI_Object* pRoot);

	void Set_Values(UI_PLAYER_STATUS_DESC desc);

	void Set_Text(Child child, const wstring& strText);
	void Set_FillAmount(Child child, _float fFillAmount);

	template<typename Func>
	void ForChild(Child child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

template<typename Func>
inline void CUI_BattleHUD::ForChild(Child child, Func&& func)
{
	auto& handle = m_handles[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}