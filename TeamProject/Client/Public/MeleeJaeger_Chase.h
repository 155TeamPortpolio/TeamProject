#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CMeleeJaeger;

class CMeleeJaeger_Chase : public IHState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Chase* Create() { return new CMeleeJaeger_Chase(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CMeleeJaeger_Run_Start : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Run_Start* Create() { return new CMeleeJaeger_Run_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Run_Loop : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Run_Loop* Create() { return new CMeleeJaeger_Run_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Run_End : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Run_End* Create() { return new CMeleeJaeger_Run_End(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Chase_Walk_Front : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Chase_Walk_Front* Create() { return new CMeleeJaeger_Chase_Walk_Front(); }
	virtual void Free() override { __super::Free(); }
};

NS_END