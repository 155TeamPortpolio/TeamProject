#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Evade_Phase1 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Evade_Phase1* Create() { return new CSacrificeState_Evade_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

/* Sub States */
class CSacrificeState_Evade_B_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Evade_B_Phase1* Create() { return new CSacrificeState_Evade_B_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Evade_R_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Evade_R_Phase1* Create() { return new CSacrificeState_Evade_R_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Evade_L_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Evade_L_Phase1* Create() { return new CSacrificeState_Evade_L_Phase1(); }
	virtual void Free() override { __super::Free(); }
};
NS_END
