#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)
class CSacrificeHand;
class CSacrificeHandState_Attack_Phase2 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_Phase2* Create() { return new CSacrificeHandState_Attack_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void BuildPattern(ATTACK_BLACK_BOARD& blackBoard);
};

/* Sub States */
class CSacrificeState_OverDrive_Release_Attack01_Phase2 : public IBaseState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeState_OverDrive_Release_Attack01_Phase2* Create() { return new CSacrificeState_OverDrive_Release_Attack01_Phase2(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_OverDrive_Release_Attack02_Phase2 : public IBaseState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeState_OverDrive_Release_Attack02_Phase2* Create() { return new CSacrificeState_OverDrive_Release_Attack02_Phase2(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_OverDrive_Release_Attack03_Phase2 : public IBaseState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeState_OverDrive_Release_Attack03_Phase2* Create() { return new CSacrificeState_OverDrive_Release_Attack03_Phase2(); }
	virtual void Free() override { __super::Free(); }
};

NS_END
