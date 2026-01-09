#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_BattleHUD final : public CUI_Object
{
	enum PREFAB { 
		ICON1, ICON2, ICON3,
		HP_BACK1, HP_BACK2, HP_BACK3,
		HP_FRONT1, HP_FRONT2, HP_FRONT3,
		SPECIAL1, SPECIAL2, SPECIAL3,
		ULTIMATE1, ULTIMATE2, ULTIMATE3, 
		CUR_HP_TEXT, MAX_HP_TEXT,
		 
		BOSS_ICON, BOSS_HP_BACK, BOSS_HP_FRONT, BOSS_GROGGY, BOSS_GROGGY_TEXT,
		BTN_NORMAL, BTN_EVADE, BTN_SPECIAL, BTN_SWITCH, BTN_ULTIMATE, 
		END };

	struct tagGaugeBindDesc {
		PREFAB		ePrefab;
		GAUGE_OWNER	eGaugeOwner;
		GAUGE_TYPE	eGaugeType;
	};

	inline static constexpr PREFAB ICON_PREFABS[] = { ICON1, ICON2, ICON3 };
	inline static constexpr PREFAB HPBACK_PREFABS[] = { HP_BACK1, HP_BACK2, HP_BACK3 };
	inline static constexpr PREFAB HPFRONT_PREFABS[] = { HP_FRONT1, HP_FRONT2, HP_FRONT3 };
	inline static constexpr PREFAB SPECIAL_PREFABS[] = { SPECIAL1, SPECIAL2, SPECIAL3 };
	inline static constexpr PREFAB ULTIMATE_PREFABS[] = { ULTIMATE1, ULTIMATE2, ULTIMATE3 };
	inline static constexpr tagGaugeBindDesc GaugeBindings[] = {
		{ HP_BACK1, GAUGE_OWNER::ROLE1, GAUGE_TYPE::HP_BACK },
		{ HP_BACK2, GAUGE_OWNER::ROLE2, GAUGE_TYPE::HP_BACK },
		{ HP_BACK3, GAUGE_OWNER::ROLE3, GAUGE_TYPE::HP_BACK },

		{ HP_FRONT1, GAUGE_OWNER::ROLE1, GAUGE_TYPE::HP },
		{ HP_FRONT2, GAUGE_OWNER::ROLE2, GAUGE_TYPE::HP },
		{ HP_FRONT3, GAUGE_OWNER::ROLE3, GAUGE_TYPE::HP },

		{ SPECIAL1, GAUGE_OWNER::ROLE1, GAUGE_TYPE::SPECIAL },
		{ SPECIAL2, GAUGE_OWNER::ROLE2, GAUGE_TYPE::SPECIAL },
		{ SPECIAL3, GAUGE_OWNER::ROLE3, GAUGE_TYPE::SPECIAL },

		{ ULTIMATE1, GAUGE_OWNER::ROLE1, GAUGE_TYPE::ULTIMATE },
		{ ULTIMATE2, GAUGE_OWNER::ROLE2, GAUGE_TYPE::ULTIMATE },
		{ ULTIMATE3, GAUGE_OWNER::ROLE3, GAUGE_TYPE::ULTIMATE },

		{ BOSS_HP_FRONT, GAUGE_OWNER::BOSS, GAUGE_TYPE::HP },
		{ BOSS_GROGGY, GAUGE_OWNER::BOSS, GAUGE_TYPE::GROGGY },
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
	vector<UI_HANDLE>	m_hChildren;

private:
	void CacheHandle(CUI_Object* pRoot);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END