#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugAssaulter;

class CThugAssaulter_Hit : public IHState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit* Create() { return new CThugAssaulter_Hit(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void Decide_L_HitState(DIR eDir, _float fDot);
};

class CThugAssaulter_Hit_Knock : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_Knock* Create() { return new CThugAssaulter_Hit_Knock(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Hit_H_Front: public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_H_Front* Create() { return new CThugAssaulter_Hit_H_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Hit_H_Back : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_H_Back* Create() { return new CThugAssaulter_Hit_H_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Hit_L_Back_Down: public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_L_Back_Down* Create() { return new CThugAssaulter_Hit_L_Back_Down(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Hit_L_Back_Up : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_L_Back_Up* Create() { return new CThugAssaulter_Hit_L_Back_Up(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Hit_L_Back_Left : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_L_Back_Left* Create() { return new CThugAssaulter_Hit_L_Back_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Hit_L_Back_Right : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_L_Back_Right* Create() { return new CThugAssaulter_Hit_L_Back_Right(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Hit_L_Front_Down: public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_L_Front_Down* Create() { return new CThugAssaulter_Hit_L_Front_Down(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Hit_L_Front_Up : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_L_Front_Up* Create() { return new CThugAssaulter_Hit_L_Front_Up(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Hit_L_Front_Left : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_L_Front_Left* Create() { return new CThugAssaulter_Hit_L_Front_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CThugAssaulter_Hit_L_Front_Right : public IBaseState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_Hit_L_Front_Right* Create() { return new CThugAssaulter_Hit_L_Front_Right(); }
	virtual void Free() override { __super::Free(); }
};

NS_END