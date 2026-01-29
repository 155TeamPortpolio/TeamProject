#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugPoacher;

class CThugPoacher_Chase : public IHState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Chase* Create() { return new CThugPoacher_Chase(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CThugPoacher_Run_Start : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Run_Start* Create() { return new CThugPoacher_Run_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Run_Loop : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Run_Loop* Create() { return new CThugPoacher_Run_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CThugPoacher_Run_End : public IBaseState<CThugPoacher>
{
public:
	virtual void Enter(CThugPoacher* pOwner) override;
	virtual void Update(CThugPoacher* pOwner, _float dt) override;
	virtual void Exit(CThugPoacher* pOwner) override;

public:
	static CThugPoacher_Run_End* Create() { return new CThugPoacher_Run_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END