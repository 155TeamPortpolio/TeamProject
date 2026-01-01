#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CThugBulkyEnforcer;

class CThugBulkyEnforcer_Born : public IBaseState<CThugBulkyEnforcer>
{
	public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Born* Create() { return new CThugBulkyEnforcer_Born(); }
	virtual void Free() override { __super::Free(); }
};

NS_END
