#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CMeleeJaeger;

class CMeleeJaeger_Groggy : public IHState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Groggy* Create() { return new CMeleeJaeger_Groggy(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CMeleeJaeger_Stun_Start : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Stun_Start* Create() { return new CMeleeJaeger_Stun_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Stun_Loop : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Stun_Loop* Create() { return new CMeleeJaeger_Stun_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Stun_End : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Stun_End* Create() { return new CMeleeJaeger_Stun_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END