#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrificeHand;
class CSacrificeHandState_Attack : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack* Create() { return new CSacrificeHandState_Attack(); }
	virtual void Free() override { __super::Free(); }
};
NS_END
