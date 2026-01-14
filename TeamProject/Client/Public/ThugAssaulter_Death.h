#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugAssaulter;

class CThugAssaulter_Death : public IHState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Death* Create() { return new CThugAssaulter_Death(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CThugAssaulter_Death_Front: public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Death_Front* Create() { return new CThugAssaulter_Death_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Death_Back : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Death_Back* Create() { return new CThugAssaulter_Death_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Death_Stay : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Death_Stay* Create() { return new CThugAssaulter_Death_Stay(); }
	virtual void Free() override { __super::Free(); }
};

NS_END