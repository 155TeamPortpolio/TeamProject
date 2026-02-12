#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CMeleeJaeger;

class CMeleeJaeger_Attack : public IHState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Attack* Create() { return new CMeleeJaeger_Attack(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void AttackFromIndex(_int iMoveIndex);

private:
	HitDesc		m_HitDesc = {};
};

class CMeleeJaeger_Attack1 : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Attack1* Create() { return new CMeleeJaeger_Attack1(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Attack2 : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Attack2* Create() { return new CMeleeJaeger_Attack2(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Attack3 : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Attack3* Create() { return new CMeleeJaeger_Attack3(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Attack4: public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Attack4* Create() { return new CMeleeJaeger_Attack4(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Attack5 : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Attack5* Create() { return new CMeleeJaeger_Attack5(); }
	virtual void Free() override { __super::Free(); }
};


NS_END