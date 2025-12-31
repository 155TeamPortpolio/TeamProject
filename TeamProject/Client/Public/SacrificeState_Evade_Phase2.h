#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Evade_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Evade_Phase2* Create() { return new CSacrificeState_Evade_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
};

/* Sub States */
class CSacrificeState_Evade_B_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Evade_B_Phase2* Create() { return new CSacrificeState_Evade_B_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Evade_R_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Evade_R_Phase2* Create() { return new CSacrificeState_Evade_R_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Evade_L_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Evade_L_Phase2* Create() { return new CSacrificeState_Evade_L_Phase2(); }
	virtual void Free() override { __super::Free(); }

};
NS_END
