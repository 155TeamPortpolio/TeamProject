#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Hit_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override {};
	virtual void Update(CSacrifice* pOwner, _float dt) override {};
	virtual void Exit(CSacrifice* pOwner) override {};

public:
	static CSacrificeState_Hit_Phase2* Create() { return new CSacrificeState_Hit_Phase2(); }
	virtual void Free() override { __super::Free(); }

};
NS_END
