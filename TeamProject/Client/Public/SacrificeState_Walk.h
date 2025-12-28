#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Walk : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Walk* Create() { return new CSacrificeState_Walk(); }
	virtual void Free() override { __super::Free(); }
};

/* Sub States */
class CSacrificeState_Walk_F : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Walk_F* Create() { return new CSacrificeState_Walk_F(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Walk_L : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Walk_L* Create() { return new CSacrificeState_Walk_L(); }
	virtual void Free() override { __super::Free(); }
};
class CSacrificeState_Walk_R : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Walk_R* Create() { return new CSacrificeState_Walk_R(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Walk_B : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Walk_B* Create() { return new CSacrificeState_Walk_B(); }
	virtual void Free() override { __super::Free(); }
};


NS_END