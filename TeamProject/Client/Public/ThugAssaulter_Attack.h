#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugAssaulter;

class CThugAssaulter_Attack : public IHState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Attack* Create() { return new CThugAssaulter_Attack(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void AttackFromIndex(_int iMoveIndex);
};

class CThugAssaulter_Attack1 : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Attack1* Create() { return new CThugAssaulter_Attack1(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Attack2 : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Attack2* Create() { return new CThugAssaulter_Attack2(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Attack3 : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Attack3* Create() { return new CThugAssaulter_Attack3(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Attack4 : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Attack4* Create() { return new CThugAssaulter_Attack4(); }
	virtual void Free() override { __super::Free(); }
};

NS_END