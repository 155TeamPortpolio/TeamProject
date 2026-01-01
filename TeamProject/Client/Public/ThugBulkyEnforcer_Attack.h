#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugBulkyEnforcer;

class CThugBulkyEnforcer_Attack : public IHState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack* Create() { return new CThugBulkyEnforcer_Attack(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void BuildPattern(ATTACK_BLACK_BOARD& blackBoard);
};

class CThugBulkyEnforcer_Attack1 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack1* Create() { return new CThugBulkyEnforcer_Attack1(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Attack2 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack2* Create() { return new CThugBulkyEnforcer_Attack2(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Attack3 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack3* Create() { return new CThugBulkyEnforcer_Attack3(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Attack4 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack4* Create() { return new CThugBulkyEnforcer_Attack4(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Attack5_1 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack5_1* Create() { return new CThugBulkyEnforcer_Attack5_1(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Attack5_2 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack5_2* Create() { return new CThugBulkyEnforcer_Attack5_2(); }
	virtual void Free() override { __super::Free(); }
};


NS_END