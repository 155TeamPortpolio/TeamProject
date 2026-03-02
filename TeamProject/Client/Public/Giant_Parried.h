#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CGiant;

class CGiant_Parried : public IHState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Parried* Create() { return new CGiant_Parried(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CGiant_Parried_Left : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Parried_Left* Create() { return new CGiant_Parried_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Parried_Right : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Parried_Right* Create() { return new CGiant_Parried_Right(); }
	virtual void Free() override { __super::Free(); }
};

NS_END