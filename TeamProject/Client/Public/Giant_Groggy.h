#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CGiant;

class CClaymore_Groggy : public IHState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Groggy* Create() { return new CClaymore_Groggy(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CClaymore_Stun_Start : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Stun_Start* Create() { return new CClaymore_Stun_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Stun_Loop : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Stun_Loop* Create() { return new CClaymore_Stun_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Stun_End : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Stun_End* Create() { return new CClaymore_Stun_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END