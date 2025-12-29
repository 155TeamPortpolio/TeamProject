#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)
class CSacrificeHand;
class CSacrificeHandState_Attack_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_Phase1* Create() { return new CSacrificeHandState_Attack_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void BuildPattern(ATTACK_BLACK_BOARD& blackBoard);
};

/* Sub States */
class CSacrificeHandState_Attack_01_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_Phase1* Create() { return new CSacrificeHandState_Attack_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeHandState_Attack_02_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_02_Phase1* Create() { return new CSacrificeHandState_Attack_02_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeHandState_Attack_03_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_03_Phase1* Create() { return new CSacrificeHandState_Attack_03_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeHandState_Attack_04_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_04_Phase1* Create() { return new CSacrificeHandState_Attack_04_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeHandState_Attack_05_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_05_Phase1* Create() { return new CSacrificeHandState_Attack_05_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeHandState_Attack_06_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_06_Phase1* Create() { return new CSacrificeHandState_Attack_06_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeHandState_Attack_10_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_10_Phase1* Create() { return new CSacrificeHandState_Attack_10_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeHandState_Attack_11_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_11_Phase1* Create() { return new CSacrificeHandState_Attack_11_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeHandState_Attack_12_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_12_Phase1* Create() { return new CSacrificeHandState_Attack_12_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

NS_END

