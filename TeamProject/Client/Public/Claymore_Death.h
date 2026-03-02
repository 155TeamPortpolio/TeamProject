#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CClaymore;

class CClaymore_Death : public IHState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Death* Create() { return new CClaymore_Death(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CClaymore_Death_Front : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Death_Front* Create() { return new CClaymore_Death_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Death_Back : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Death_Back* Create() { return new CClaymore_Death_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Death_Stay : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Death_Stay* Create() { return new CClaymore_Death_Stay(); }
	virtual void Free() override { __super::Free(); }
};

NS_END