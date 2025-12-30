#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Evade : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Evade* Create() { return new CSacrificeState_Evade(); }
	virtual void Free() override { __super::Free(); }
};
NS_END
