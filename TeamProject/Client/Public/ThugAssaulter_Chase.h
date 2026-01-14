#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugAssaulter;

class CThugAssaulter_Chase : public IHState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Chase* Create() { return new CThugAssaulter_Chase(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CThugAssaulter_Run_Start: public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Run_Start* Create() { return new CThugAssaulter_Run_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Run_Loop : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Run_Loop* Create() { return new CThugAssaulter_Run_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Run_End: public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Run_End* Create() { return new CThugAssaulter_Run_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END