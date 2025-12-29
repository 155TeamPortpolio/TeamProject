#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrificeHand;
class CSacrificeHandState_Idle : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Idle* Create() { return new CSacrificeHandState_Idle(); }
	virtual void Free() override { __super::Free(); }
};

/* Sub States */
class CSacrificeHandState_Idle_Air : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Idle_Air* Create() { return new CSacrificeHandState_Idle_Air(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeHandState_Idle_Ground : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Idle_Ground* Create() { return new CSacrificeHandState_Idle_Ground(); }
	virtual void Free() override { __super::Free(); }
};
NS_END
