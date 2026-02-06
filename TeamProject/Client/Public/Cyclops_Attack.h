#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CCyclops;

class CCyclops_Attack : public IHState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Attack* Create() { return new CCyclops_Attack(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void AttackFromIndex(_int iMoveIndex);

private:
	HitDesc		m_HitDesc = {};
	_bool		m_isStopRotate = { false };
};

class CCyclops_Attack1 : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Attack1* Create() { return new CCyclops_Attack1(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Attack2 : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Attack2* Create() { return new CCyclops_Attack2(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Attack3_Hit : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Attack3_Hit* Create() { return new CCyclops_Attack3_Hit(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Attack3_End : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Attack3_End* Create() { return new CCyclops_Attack3_End(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Attack4: public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Attack4* Create() { return new CCyclops_Attack4(); }
	virtual void Free() override { __super::Free(); }
};

NS_END