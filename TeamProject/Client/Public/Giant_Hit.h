#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CGiant;

class CGiant_Hit : public IHState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit* Create() { return new CGiant_Hit(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CGiant_Hit_H_Front : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit_H_Front* Create() { return new CGiant_Hit_H_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Hit_H_Back : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit_H_Back* Create() { return new CGiant_Hit_H_Back(); }
	virtual void Free() override { __super::Free(); }
};

NS_END