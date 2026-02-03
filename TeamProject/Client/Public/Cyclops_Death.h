#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CCyclops;

class CCyclops_Death : public IHState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Death* Create() { return new CCyclops_Death(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CCyclops_Death_Front : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Death_Front* Create() { return new CCyclops_Death_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Death_Back : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Death_Back* Create() { return new CCyclops_Death_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Death_Stay : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Death_Stay* Create() { return new CCyclops_Death_Stay(); }
	virtual void Free() override { __super::Free(); }
};

NS_END