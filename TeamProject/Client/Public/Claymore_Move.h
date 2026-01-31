#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CClaymore;

class CClaymore_Move : public IHState<CClaymore>
{
private:
	enum MOVEINDEX : _int {
		Walk_Front = 1,
		Walk_Back,
		Walk_Left,
		Walk_Right,
		Walk_F_L,
		Walk_F_R,
		Walk_R_F,
	};

public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Move* Create() { return new CClaymore_Move(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void ChangeMovePatternFromIndex(_int iMoveIndex);
};

class CClaymore_Walk_Front : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Walk_Front* Create() { return new CClaymore_Walk_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Walk_Back : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Walk_Back* Create() { return new CClaymore_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Walk_Left : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Walk_Left* Create() { return new CClaymore_Walk_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Walk_Right : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Walk_Right* Create() { return new CClaymore_Walk_Right(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Walk_F_L : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Walk_F_L* Create() { return new CClaymore_Walk_F_L(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Walk_F_R: public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Walk_F_R* Create() { return new CClaymore_Walk_F_R(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Walk_R_F: public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Walk_R_F* Create() { return new CClaymore_Walk_R_F(); }
	virtual void Free() override { __super::Free(); }
};

NS_END