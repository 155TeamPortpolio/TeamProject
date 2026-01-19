#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CThugBulkyEnforcer;

class CThugBulkyEnforcer_Idle : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Idle* Create() { return new CThugBulkyEnforcer_Idle(); }
	virtual void Free() override { __super::Free(); }
};

/* Sub States */
//class CSacrificeState_Idle_Phase1 : public IBaseState<CSacrifice>
//{
//public:
//	virtual void Enter(CSacrifice* pOwner) override;
//	virtual void Update(CSacrifice* pOwner, _float dt) override;
//	virtual void Exit(CSacrifice* pOwner) override;
//
//public:
//	static CSacrificeState_Idle_Phase1* Create() { return new CSacrificeState_Idle_Phase1(); }
//	virtual void Free() override { __super::Free(); }
//};


NS_END