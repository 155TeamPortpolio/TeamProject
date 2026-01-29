#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugPoacher;

class CThugPoacher_Attack : public IHState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Attack* Create() { return new CThugPoacher_Attack(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void AttackFromIndex(_int iMoveIndex);
};

class CThugPoacher_Attack1 : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Attack1* Create() { return new CThugPoacher_Attack1(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Attack2 : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Attack2* Create() { return new CThugPoacher_Attack2(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Attack3 : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Attack3* Create() { return new CThugPoacher_Attack3(); }
	virtual void Free() override { __super::Free(); }
};

NS_END