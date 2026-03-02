#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CCharacterParryCollider final : public CGameObject
{
private:
	CCharacterParryCollider();
	CCharacterParryCollider(const CCharacterParryCollider& rhs);
	virtual ~CCharacterParryCollider() DEFAULT;

public:
	vector<OBJECT_HANDLE>& Get_ParryTargets() { return m_ParryableTargets; }

public:
	HRESULT         Initialize_Prototype() override;
	HRESULT         Initialize(INIT_DESC* pArg) override;
	void	        Awake() override;
	void            Priority_Update(_float dt) override;
	void            Update(_float dt) override;
	void            Late_Update(_float dt) override;
	virtual void    Render_GUI() override;
	virtual void    OnTriggerEnter(CGameObject* pOther) override;
	virtual void    OnTriggerStay(CGameObject* pOther) override;
	virtual void    OnTriggerExit(CGameObject* pOther) override;

public:
	_vector3    Get_PrevPos() { return m_vPrevPos; }
	_bool		Can_Parry();
	_bool		Can_Perfect();

private:
	vector<OBJECT_HANDLE> m_ParryableTargets;
	vector<OBJECT_HANDLE> m_EvadableTargets;
	_vector3 m_vPrevPos;

public:
	static CCharacterParryCollider* Create();
	CGameObject* Clone(INIT_DESC* pArg) override;
	virtual void Free() override { __super::Free(); }
};

NS_END