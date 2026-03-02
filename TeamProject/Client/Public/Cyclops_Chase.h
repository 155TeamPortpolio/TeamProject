#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CCyclops;

class CCyclops_Chase : public IHState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Chase* Create() { return new CCyclops_Chase(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CCyclops_Run_Start : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Run_Start* Create() { return new CCyclops_Run_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Run_Loop : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Run_Loop* Create() { return new CCyclops_Run_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Run_End : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Run_End* Create() { return new CCyclops_Run_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END