#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CStrikeJaeger;

class CStrikeJaeger_Move : public IHState<CStrikeJaeger>
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
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Move* Create() { return new CStrikeJaeger_Move(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void ChangeMovePatternFromIndex(_int iMoveIndex);
};

class CStrikeJaeger_Walk_Front : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Walk_Front* Create() { return new CStrikeJaeger_Walk_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CStrikeJaeger_Walk_Back : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Walk_Back* Create() { return new CStrikeJaeger_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CStrikeJaeger_Walk_Left : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Walk_Left* Create() { return new CStrikeJaeger_Walk_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CStrikeJaeger_Walk_Right : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Walk_Right* Create() { return new CStrikeJaeger_Walk_Right(); }
	virtual void Free() override { __super::Free(); }
};

class CStrikeJaeger_Walk_F_L : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Walk_F_L* Create() { return new CStrikeJaeger_Walk_F_L(); }
	virtual void Free() override { __super::Free(); }
};

class CStrikeJaeger_Walk_F_R: public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Walk_F_R* Create() { return new CStrikeJaeger_Walk_F_R(); }
	virtual void Free() override { __super::Free(); }
};

class CStrikeJaeger_Walk_R_F: public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Walk_R_F* Create() { return new CStrikeJaeger_Walk_R_F(); }
	virtual void Free() override { __super::Free(); }
};

NS_END