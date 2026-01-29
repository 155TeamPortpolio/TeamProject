#pragma once

#include "GameObject.h"

NS_BEGIN(Client)

class COrbitCamCollider : public CGameObject
{
private:
	COrbitCamCollider() {}
	COrbitCamCollider(const COrbitCamCollider& rhs) : CGameObject(rhs) {}
	virtual ~COrbitCamCollider() DEFAULT;

public:
	HRESULT Initialize_Prototype()           override;
	HRESULT Initialize(INIT_DESC* arg)       override;
	void	Awake()                          override{}
	void    Priority_Update(_float dt)       override{}
	void    Update(_float dt)                override{}
	void    Late_Update(_float dt)           override;

	void    OnTriggerEnter(CGameObject* obj) override;
	void    OnTriggerStay(CGameObject* obj)  override;
	void    OnTriggerExit(CGameObject* obj)  override;

private:
	vector<OBJECT_HANDLE> m_cur;
	vector<OBJECT_HANDLE> m_prev;

public:
	static COrbitCamCollider* Create();
	CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END