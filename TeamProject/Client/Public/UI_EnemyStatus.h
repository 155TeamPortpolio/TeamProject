#pragma once
#include "UI_WorldToScreen.h"

NS_BEGIN(Client)

class CUI_EnemyStatus final : public CUI_WorldToScreen
{
public:
	typedef struct tagEnemyStatusDesc : public UI_DESC {
		const _float4x4* pParentWorld = { nullptr };
		const _float4x4* pBoneLocal = { nullptr };
	}ENEMYSTATUS_DESC;

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

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END