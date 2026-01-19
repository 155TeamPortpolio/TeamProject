#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Parry_Phase1 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Parry_Phase1* Create() { return new CSacrificeState_Parry_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

/* Sub States */
class CSacrificeState_Parry_R_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Parry_R_Phase1* Create() { return new CSacrificeState_Parry_R_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Parry_L_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Parry_L_Phase1* Create() { return new CSacrificeState_Parry_L_Phase1(); }
	virtual void Free() override { __super::Free(); }
};
NS_END
