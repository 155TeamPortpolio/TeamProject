#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CGiant;

class CGiant_Move : public IHState<CGiant>
{
private:
	enum MOVEINDEX : _int {
		Walk_Front = 1,
		Walk_Back
	};

public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Move* Create() { return new CGiant_Move(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void ChangeMovePatternFromIndex(_int iMoveIndex);
};

class CGiant_Walk_Front : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Walk_Front* Create() { return new CGiant_Walk_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Walk_Back : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Walk_Back* Create() { return new CGiant_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

NS_END