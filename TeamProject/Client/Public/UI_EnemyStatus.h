#pragma once
#include "UI_WorldToScreen.h"
#include "Enemy_Struct.h"

NS_BEGIN(Client)

// ===============================
// CUI_EnemyStatus
// 몬스터 머리 위에 표시되는 상태 UI
// - HP / Groggy 게이지
// - Groggy 수치 텍스트
// - 타겟 락온 표시
// - 월드 위치 → 스크린 좌표 변환
// ===============================

class CUI_EnemyStatus final : public CUI_WorldToScreen
{
public:
	typedef struct tagEnemyStatusDesc : public UI_DESC {
		const _float4x4* pParentWorld = { nullptr };
		const _float4x4* pBoneLocal = { nullptr };
		const MONSTER_STATUS* pMonsterStatus = { nullptr };
		OBJECT_HANDLE tOwnerHandle = {};
	}ENEMYSTATUS_DESC;

private:
	enum class Child { LOCKON, GAUGE_HP_BACK, GAUGE_HP_FRONT, GAUGE_GROGGY, GROGGY_TEXT, END };
	inline static const string INSTANCENAMES[ENUM(Child::END)] = { "lockOn", "hpBack", "hpFront", "groggy", "groggyText" };

private:
	CUI_EnemyStatus() {}
	CUI_EnemyStatus(const CUI_EnemyStatus& rhs) : CUI_WorldToScreen(rhs) {}
	virtual ~CUI_EnemyStatus() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override {}
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	UI_HANDLE				m_handles[ENUM(Child::END)];

	const _float4x4*		m_pParentWorld = { nullptr };
	const _float4x4*		m_pBoneLocal = { nullptr };
	const MONSTER_STATUS*	m_pMonsterStatus = { nullptr };
	OBJECT_HANDLE			m_tOwnerHandle = {};

	static constexpr _float HPBACK_DELTA = 5.f;
	static constexpr _float HPBACK_LERP_SPEED = 7.f;
	GAUGE_DELAY_DESC		m_hpBack = {};

	_bool					m_isBlinking = {};
	_float					m_fBlinkAcc = {};
	static constexpr _float BLINK_SPEED_MIN = 10.f;
	static constexpr _float BLINK_SPEED_MAX = 50.f;

	const _float			m_fGroggyMax = { 100.f };	// 그로기 맥스는 무조건 100

private:
	void Set_TargetLock(TARGET_LOCK_DESC& desc);

	void Set_WorldPosition();

	void Update_HPBackGauge(_float fRatio, _float dt);

	void Set_Alive(Child child, _bool isAlive);
	void Set_Color(Child child, _float4 vColor);
	void Set_Animation(Child child, _int iIndex);
	void Set_GaugeFill(Child child, _float fFillAmount);
	void Set_GroggyText(_int iGroggy); 
	 
private:
	template<typename Func>
	void ForChild(Child child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

template<typename Func>
inline void CUI_EnemyStatus::ForChild(Child child, Func&& func)
{
	auto& handle = m_handles[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}