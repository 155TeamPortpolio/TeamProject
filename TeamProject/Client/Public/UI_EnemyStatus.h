#pragma once
#include "UI_WorldToScreen.h"
#include "Enemy_Struct.h"

NS_BEGIN(Client)

class CUI_EnemyStatus final : public CUI_WorldToScreen
{
public:
	typedef struct tagEnemyStatusDesc : public UI_DESC {
		const _float4x4* pParentWorld = { nullptr };
		const _float4x4* pBoneLocal = { nullptr };
		const MONSTER_STATUS* pMonsterStatus = { nullptr };
	}ENEMYSTATUS_DESC;

private:
	enum class Child { ROOT, HP_GUAGE, GROGGY_GAUGE, GROGGY_TEXT, END };
	inline static const string INSTANCENAMES[ENUM(Child::END)] = { "enemy_status", "hpGauge", "groggyGauge", "groggyText" };

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
	const _float4x4* m_pParentWorld = { nullptr };
	const _float4x4* m_pBoneLocal = { nullptr };
	const MONSTER_STATUS* m_pMonsterStatus = { nullptr };

	UI_HANDLE		m_handles[ENUM(Child::END)];

private:
	void Set_WorldPosition();

	void Set_GroggyText(_int iGroggy);

	void Set_Gauge(Child child, _float fFillAmount);

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