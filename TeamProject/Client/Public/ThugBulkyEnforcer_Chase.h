#pragma once
#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugBulkyEnforcer;

class CThugBulkyEnforcer_Chase: public IHState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Chase* Create() { return new CThugBulkyEnforcer_Chase(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CThugBulkyEnforcer_Run_Start : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Run_Start* Create() { return new CThugBulkyEnforcer_Run_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Run : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Run* Create() { return new CThugBulkyEnforcer_Run(); }
	virtual void Free() override { __super::Free(); }

private:
	_float2		m_vFIndPlayerPosTime = {};
};

class CThugBulkyEnforcer_Run_End : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Run_End* Create() { return new CThugBulkyEnforcer_Run_End(); }
	virtual void Free() override { __super::Free(); }
};


NS_END