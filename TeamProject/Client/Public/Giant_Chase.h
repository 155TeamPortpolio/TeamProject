#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CGiant;

class CClaymore_Chase : public IHState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Chase* Create() { return new CClaymore_Chase(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CClaymore_Run_Start : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Run_Start* Create() { return new CClaymore_Run_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Run_Loop : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Run_Loop* Create() { return new CClaymore_Run_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Run_End : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Run_End* Create() { return new CClaymore_Run_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END