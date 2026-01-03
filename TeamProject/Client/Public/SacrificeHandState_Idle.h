#pragma once
#include "IBaseState.h"

NS_BEGIN(Client)
class CSacrificeHand;
class CSacrificeHandState_Idle : public IBaseState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Idle* Create() { return new CSacrificeHandState_Idle(); }
	virtual void Free() override { __super::Free(); }
};
NS_END
