#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CGiant;

class CClaymore_Death : public IHState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Death* Create() { return new CClaymore_Death(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CClaymore_Death_Front : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Death_Front* Create() { return new CClaymore_Death_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Death_Back : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Death_Back* Create() { return new CClaymore_Death_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Death_Stay : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CClaymore_Death_Stay* Create() { return new CClaymore_Death_Stay(); }
	virtual void Free() override { __super::Free(); }
};

NS_END