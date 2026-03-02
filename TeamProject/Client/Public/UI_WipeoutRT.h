#pragma once
#include "UI_ScreenRT.h"

NS_BEGIN(Client)

class CUI_WipeoutRT final : public CUI_ScreenRT
{
private:
	enum class STATE { ACTIVE, INACTIVE, END };

	enum class GROUP { GROUP1, GROUP2, GROUP3, GROUP4, GROUP5, END };
	inline static const string INSTANCENAMES[ENUM(GROUP::END)] = { "group1", "group2", "group3", "group4", "group5" };

private:
	CUI_WipeoutRT() {}
	CUI_WipeoutRT(const CUI_WipeoutRT& rhs) : CUI_ScreenRT(rhs) {}
	virtual ~CUI_WipeoutRT() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void UI_Active(void* pArg = nullptr) override;

private:
	CUI_Object* m_pGroups[ENUM(GROUP::END)] = {};
	GROUP m_eCurrentGroup = { GROUP::END };
	STATE m_eState = { STATE::END };

	_uint m_iBlinkCount = {};

	_float m_fTimer = {};
	const _float m_fDuration = { 5.6f };

private:
	void Cache();

	void Update_GroupState();

	void Change_State(STATE eState);
	_bool Change_Group(GROUP eGroup, _int iIndex = 0);

	void Set_GroupAlive(GROUP group, _bool isAlive);
	_bool Set_GroupAnimation(GROUP group, _int iIndex);
	_bool Is_GroupAnimationFinished(GROUP group);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END