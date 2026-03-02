#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CClaymore;

class CClaymore_Groggy : public IHState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Groggy* Create() { return new CClaymore_Groggy(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CClaymore_Stun_Start : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Stun_Start* Create() { return new CClaymore_Stun_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Stun_Loop : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Stun_Loop* Create() { return new CClaymore_Stun_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Stun_End : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Stun_End* Create() { return new CClaymore_Stun_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END