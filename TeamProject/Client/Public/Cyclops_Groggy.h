#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CCyclops;

class CCyclops_Groggy : public IHState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Groggy* Create() { return new CCyclops_Groggy(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CCyclops_Stun_Start : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Stun_Start* Create() { return new CCyclops_Stun_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Stun_Loop : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Stun_Loop* Create() { return new CCyclops_Stun_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Stun_End : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Stun_End* Create() { return new CCyclops_Stun_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END