#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CMeleeJaeger;

class CMeleeJaeger_Move : public IHState<CMeleeJaeger>
{
private:
	enum MOVEINDEX : _int {
		Walk_Front = 1,
		Walk_Back,
		Walk_Left,
		Walk_Right,
		Walk_F_R,
		Walk_R_F,
		Evade
	};

public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Move* Create() { return new CMeleeJaeger_Move(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void ChangeMovePatternFromIndex(_int iMoveIndex);
};

class CMeleeJaeger_Walk_Front : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Walk_Front* Create() { return new CMeleeJaeger_Walk_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Walk_Back : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Walk_Back* Create() { return new CMeleeJaeger_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Walk_Left : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Walk_Left* Create() { return new CMeleeJaeger_Walk_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Walk_Right : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Walk_Right* Create() { return new CMeleeJaeger_Walk_Right(); }
	virtual void Free() override { __super::Free(); }
};

//class CMeleeJaeger_Walk_F_L : public IBaseState<CMeleeJaeger>
//{
//public:
//	virtual void Enter(CMeleeJaeger* pOwner) override;
//	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
//	virtual void Exit(CMeleeJaeger* pOwner) override;
//
//public:
//	static CMeleeJaeger_Walk_F_L* Create() { return new CMeleeJaeger_Walk_F_L(); }
//	virtual void Free() override { __super::Free(); }
//};

class CMeleeJaeger_Walk_F_R: public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Walk_F_R* Create() { return new CMeleeJaeger_Walk_F_R(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Walk_R_F: public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Walk_R_F* Create() { return new CMeleeJaeger_Walk_R_F(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Evade : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Evade* Create() { return new CMeleeJaeger_Evade(); }
	virtual void Free() override { __super::Free(); }
};

NS_END