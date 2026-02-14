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
	void Decide_L_HitState(DIR eDir, _float fDot);
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

class CGiant_Hit_L_Back_Down : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit_L_Back_Down* Create() { return new CGiant_Hit_L_Back_Down(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Hit_L_Back_Up : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit_L_Back_Up* Create() { return new CGiant_Hit_L_Back_Up(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Hit_L_Back_Left : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit_L_Back_Left* Create() { return new CGiant_Hit_L_Back_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Hit_L_Back_Right : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit_L_Back_Right* Create() { return new CGiant_Hit_L_Back_Right(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Hit_L_Front_Down : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit_L_Front_Down* Create() { return new CGiant_Hit_L_Front_Down(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Hit_L_Front_Up : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit_L_Front_Up* Create() { return new CGiant_Hit_L_Front_Up(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Hit_L_Front_Left : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit_L_Front_Left* Create() { return new CGiant_Hit_L_Front_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Hit_L_Front_Right : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Hit_L_Front_Right* Create() { return new CGiant_Hit_L_Front_Right(); }
	virtual void Free() override { __super::Free(); }
};

NS_END