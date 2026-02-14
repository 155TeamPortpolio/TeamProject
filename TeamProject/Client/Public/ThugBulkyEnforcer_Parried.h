#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugBulkyEnforcer;

class CThugBulkyEnforcer_Parried : public IHState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Parried* Create() { return new CThugBulkyEnforcer_Parried(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CThugBulkyEnforcer_Parry_Left : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Parry_Left* Create() { return new CThugBulkyEnforcer_Parry_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Parry_Right : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Parry_Right* Create() { return new CThugBulkyEnforcer_Parry_Right(); }
	virtual void Free() override { __super::Free(); }
};

NS_END