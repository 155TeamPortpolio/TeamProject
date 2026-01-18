#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugBulkyEnforcer;

class CThugBulkyEnforcer_Hit : public IHState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit* Create() { return new CThugBulkyEnforcer_Hit(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void Decide_L_HitState(DIR eDir, _float fDot);
};

class CThugBulkyEnforcer_Hit_H_Front : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit_H_Front* Create() { return new CThugBulkyEnforcer_Hit_H_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Hit_H_Back : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit_H_Back* Create() { return new CThugBulkyEnforcer_Hit_H_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Hit_L_Back_Down : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit_L_Back_Down* Create() { return new CThugBulkyEnforcer_Hit_L_Back_Down(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Hit_L_Back_Up : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit_L_Back_Up* Create() { return new CThugBulkyEnforcer_Hit_L_Back_Up(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Hit_L_Back_Left : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit_L_Back_Left* Create() { return new CThugBulkyEnforcer_Hit_L_Back_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Hit_L_Back_Right : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit_L_Back_Right* Create() { return new CThugBulkyEnforcer_Hit_L_Back_Right(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Hit_L_Front_Down : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit_L_Front_Down* Create() { return new CThugBulkyEnforcer_Hit_L_Front_Down(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Hit_L_Front_Up : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit_L_Front_Up* Create() { return new CThugBulkyEnforcer_Hit_L_Front_Up(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Hit_L_Front_Left : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit_L_Front_Left* Create() { return new CThugBulkyEnforcer_Hit_L_Front_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Hit_L_Front_Right : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Hit_L_Front_Right* Create() { return new CThugBulkyEnforcer_Hit_L_Front_Right(); }
	virtual void Free() override { __super::Free(); }
};

NS_END