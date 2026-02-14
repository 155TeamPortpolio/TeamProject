#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CGiant;

class CGiant_Groggy : public IHState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Groggy* Create() { return new CGiant_Groggy(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CGiant_Stun_Start_Front : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Stun_Start_Front* Create() { return new CGiant_Stun_Start_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Stun_Start_Back : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Stun_Start_Back* Create() { return new CGiant_Stun_Start_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Stun_Loop : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Stun_Loop* Create() { return new CGiant_Stun_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Stun_End : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Stun_End* Create() { return new CGiant_Stun_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END