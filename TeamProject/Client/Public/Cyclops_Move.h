#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CCyclops;

class CCyclops_Move : public IHState<CCyclops>
{
private:
	enum MOVEINDEX : _int {
		Walk_Back = 1,
		Walk_Front,
		Walk_Left,
		Walk_Right,
		Walk_F_L,
		Walk_F_R,
		Walk_L_F,
	};

public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Move* Create() { return new CCyclops_Move(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void ChangeMovePatternFromIndex(_int iMoveIndex);
};

class CCyclops_Walk_Front : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Walk_Front* Create() { return new CCyclops_Walk_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Walk_Back : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Walk_Back* Create() { return new CCyclops_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Walk_Left : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Walk_Left* Create() { return new CCyclops_Walk_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Walk_Right : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Walk_Right* Create() { return new CCyclops_Walk_Right(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Walk_F_L : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Walk_F_L* Create() { return new CCyclops_Walk_F_L(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Walk_F_R: public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Walk_F_R* Create() { return new CCyclops_Walk_F_R(); }
	virtual void Free() override { __super::Free(); }
};

class CCyclops_Walk_L_F : public IBaseState<CCyclops>
{
public:
	virtual void Enter(CCyclops* pOwner) override;
	virtual void Update(CCyclops* pOwner, _float dt) override;
	virtual void Exit(CCyclops* pOwner) override;

public:
	static CCyclops_Walk_L_F* Create() { return new CCyclops_Walk_L_F(); }
	virtual void Free() override { __super::Free(); }
};

NS_END