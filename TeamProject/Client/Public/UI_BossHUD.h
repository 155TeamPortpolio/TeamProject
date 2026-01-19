#pragma once
#include "UI_Object.h"
#include "Enemy_Struct.h"

NS_BEGIN(Client)

class CUI_BossHUD final : public CUI_Object
{
public:
	typedef struct tagBossHUDDesc : public UI_DESC {
		const MONSTER_STATUS* pMonsterStatus = { nullptr };
	}BOSS_HUD_DESC;

private:
	enum Child { ICON, GAUGE_HP_BACK, GAUGE_HP_FRONT, GAUGE_GROGGY, TEXT_GROGGY, END };
	inline static const string INSTANCENAMES[ENUM(Child::END)] = { "icon", "hpBack", "hpFront", "groggy", "groggyText" };

private:
	CUI_BossHUD() {}
	CUI_BossHUD(const CUI_BossHUD& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_BossHUD() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	UI_HANDLE				m_handles[ENUM(Child::END)];

	const MONSTER_STATUS*	m_pMonsterStatus = { nullptr };

	static constexpr _float HPBACK_DELTA = 5.f;
	static constexpr _float HPBACK_LERP_SPEED = 7.f;
	GAUGE_DELAY_DESC		m_hpBack = {};

	_bool					m_isBlinking = {};
	_float					m_fBlinkAcc = {};
	static constexpr _float BLINK_SPEED_MIN = 10.f; 
	static constexpr _float BLINK_SPEED_MAX = 50.f; 

	const _float			m_fGroggyMax = { 100.f };	// 그로기 맥스는 무조건 100

private:
	void Update_HPBackGauge(_float fRatio, _float dt);
	void Apply_Blink(_float fRatio, _float dt);

	void Set_Color(Child child, _float4 vColor);
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
inline void CUI_BossHUD::ForChild(Child child, Func&& func)
{
	auto& handle = m_handles[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}