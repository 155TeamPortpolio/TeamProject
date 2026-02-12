#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CMeleeJaeger;

class CMeleeJaeger_Hit : public IHState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit* Create() { return new CMeleeJaeger_Hit(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void Decide_L_HitState(DIR eDir, _float fDot);
};

class CMeleeJaeger_Hit_Knock : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_Knock* Create() { return new CMeleeJaeger_Hit_Knock(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Hit_H_Front : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_H_Front* Create() { return new CMeleeJaeger_Hit_H_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Hit_H_Back : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_H_Back* Create() { return new CMeleeJaeger_Hit_H_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Hit_L_Back_Down : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_L_Back_Down* Create() { return new CMeleeJaeger_Hit_L_Back_Down(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Hit_L_Back_Up : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_L_Back_Up* Create() { return new CMeleeJaeger_Hit_L_Back_Up(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Hit_L_Back_Left : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_L_Back_Left* Create() { return new CMeleeJaeger_Hit_L_Back_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Hit_L_Back_Right : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_L_Back_Right* Create() { return new CMeleeJaeger_Hit_L_Back_Right(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Hit_L_Front_Down : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_L_Front_Down* Create() { return new CMeleeJaeger_Hit_L_Front_Down(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Hit_L_Front_Up : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_L_Front_Up* Create() { return new CMeleeJaeger_Hit_L_Front_Up(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Hit_L_Front_Left : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_L_Front_Left* Create() { return new CMeleeJaeger_Hit_L_Front_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CMeleeJaeger_Hit_L_Front_Right : public IBaseState<CMeleeJaeger>
{
public:
	virtual void Enter(CMeleeJaeger* pOwner) override;
	virtual void Update(CMeleeJaeger* pOwner, _float dt) override;
	virtual void Exit(CMeleeJaeger* pOwner) override;

public:
	static CMeleeJaeger_Hit_L_Front_Right* Create() { return new CMeleeJaeger_Hit_L_Front_Right(); }
	virtual void Free() override { __super::Free(); }
};

NS_END