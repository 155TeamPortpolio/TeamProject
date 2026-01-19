#pragma once
#include "IHState.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Groggy_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Groggy_Phase2* Create() { return new CSacrificeState_Groggy_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	void BuildPattern(CSacrifice* pOwner);

};

/* Sub States */
class CSacrificeState_Groggy_Start_Front_Phase2 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Groggy_Start_Front_Phase2* Create() { return new CSacrificeState_Groggy_Start_Front_Phase2(); }
	virtual void Free() override { __super::Free(); }

};
class CSacrificeState_Groggy_Start_Back_Phase2 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Groggy_Start_Back_Phase2* Create() { return new CSacrificeState_Groggy_Start_Back_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Groggy_Loop_Phase2 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Groggy_Loop_Phase2* Create() { return new CSacrificeState_Groggy_Loop_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Groggy_End_Phase2 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Groggy_End_Phase2* Create() { return new CSacrificeState_Groggy_End_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

NS_END