#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CClaymore;

class CClaymore_Chase : public IHState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Chase* Create() { return new CClaymore_Chase(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CClaymore_Run_Start : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Run_Start* Create() { return new CClaymore_Run_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Run_Loop : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Run_Loop* Create() { return new CClaymore_Run_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Run_End : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Run_End* Create() { return new CClaymore_Run_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END