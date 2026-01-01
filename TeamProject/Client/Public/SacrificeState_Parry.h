#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Parry :public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Parry* Create() { return new CSacrificeState_Parry(); }
	virtual void Free() override { __super::Free(); }
};
NS_END
