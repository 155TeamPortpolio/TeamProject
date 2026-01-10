#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugBulkyEnforcer;

class CThugBulkyEnforcer_Death : public IHState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Death* Create() { return new CThugBulkyEnforcer_Death(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CThugBulkyEnforcer_Death_Front : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Death_Front* Create() { return new CThugBulkyEnforcer_Death_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Death_Back: public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Death_Back* Create() { return new CThugBulkyEnforcer_Death_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Death_Stay: public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Death_Stay* Create() { return new CThugBulkyEnforcer_Death_Stay(); }
	virtual void Free() override { __super::Free(); }
};

NS_END