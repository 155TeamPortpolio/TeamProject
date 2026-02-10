#pragma once
#include "GameObject.h"
class CMiyabi_Ghost final : public CGameObject
{
private:
	CMiyabi_Ghost();
	CMiyabi_Ghost(const CMiyabi_Ghost& rhs);
	virtual ~CMiyabi_Ghost() DEFAULT;

public:
	HRESULT         Initialize_Prototype() override;
	HRESULT         Initialize(INIT_DESC* pArg) override;
	void	        Awake() override;
	void            Priority_Update(_float dt) override;
	void            Update(_float dt) override;
	void            Late_Update(_float dt) override;

public:
	void            Set_FollowTarget(CTransform* pTarget) { m_pTargetTransform = pTarget; }

private:
	void			FollowTarget(_float dt);

private:
	CTransform*		m_pTargetTransform = nullptr;
	_float3         m_vOffset = { -0.5f, 1.55f, -0.2f };   // 미야비 기준 로컬 오프셋
	_float          m_fFollowSpeed = 3.f;                  // 추적 보간 속도
	_float			m_fDistanceScale = 0.5f;

public:
	static CMiyabi_Ghost* Create();
	CGameObject* Clone(INIT_DESC* pArg) override;
	virtual void Free() override;
};

