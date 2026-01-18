#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_BattleHUD final : public CUI_Object
{
	enum Child {
		ROLE1, ROLE2, ROLE3, 
		ICON1, ICON2, ICON3,
		HP_BACK1, HP_BACK2, HP_BACK3,
		HP_FRONT1, HP_FRONT2, HP_FRONT3,
		SPECIAL1, SPECIAL2, SPECIAL3,
		SPECIALARROW1, SPECIALARROW2, SPECIALARROW3,
		ULTIMATE1, ULTIMATE2, ULTIMATE3,
		ULTIMATEICON1, ULTIMATEICON2, ULTIMATEICON3,
		CUR_HP_TEXT, MAX_HP_TEXT,
		ACTION,
		END
	};

	inline static constexpr Child ROLE_CHILD[] = { ROLE1, ROLE2, ROLE3 };
	inline static constexpr Child ICON_CHILD[] = { ICON1, ICON2, ICON3 };
	inline static constexpr Child HPBACK_CHILD[] = { HP_BACK1, HP_BACK2, HP_BACK3 };
	inline static constexpr Child HPFRONT_CHILD[] = { HP_FRONT1, HP_FRONT2, HP_FRONT3 };
	inline static constexpr Child SPECIAL_CHILD[] = { SPECIAL1, SPECIAL2, SPECIAL3 };
	inline static constexpr _float SPECIAL_THRESHOLD[] = { 150.f, 48.f, 48.f };
	inline static constexpr Child SPECIALARROW_CHILD[] = { SPECIALARROW1, SPECIALARROW2, SPECIALARROW3 };
	inline static constexpr Child ULTIMATE_CHILD[] = { ULTIMATE1, ULTIMATE2, ULTIMATE3 };
	inline static constexpr Child ULTIMATEICON_CHILD[] = { ULTIMATEICON1, ULTIMATEICON2, ULTIMATEICON3 };

	inline static const string ICONTEXTURES[ENUM(CHARACTER::END)] = { "IconRoleGeneral24.png", "IconRoleGeneral09.png" };

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

	_bool				m_isUltimate[3] = {};
	// 150 48
private:
	const _int			m_iPlayerHPWidth = 5;
	const _int			m_iBossHPWidth = 2;

private:
	CUI_Object* Ready_Prefab();

	void Add_PartObject(CUI_Object* pRoot, const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, Child child, _float2 vOffset = _float2());
	void Cache_Handles(CUI_Object* pRoot);

	void Set_Values(UI_PLAYER_STATUS_DESC desc);

	void Set_Special(_int iIndex, _float fRatio, _float fThresRatio);
	void Set_UltimateIcon(_int iIndex, _float fRatio);
	
	_bool Is_Alive(Child child);
	void Set_Alive(Child child, _bool isAlive);
	void Set_Color(Child child, _float4 vColor);
	void Set_Animation(Child child, _int iIndex);
	void Set_Texture(Child child, const string& strTextureKey); 
	void Set_GaugeFill(Child child, _float fFillAmount);
	void Set_NumberText(Child child, _int iNum, _int iWidth);

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