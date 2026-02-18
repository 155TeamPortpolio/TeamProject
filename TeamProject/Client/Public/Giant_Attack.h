#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CGiant;

class CGiant_Attack : public IHState<CGiant>
{
private:
	enum ATTACK {
		Attack1 = 1,
		Attack2,
		Attack2_1,
		Attack2_Explode,
		Attack3,
		Attack3_HitWall,
		Attack4,
		Attack5,
		//Attack6_AttackBack,
		//Attack7,
		//Attack7_Jump,
		//Attack7_Revenge
	};

public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack* Create() { return new CGiant_Attack(); }
	virtual void Free() override { __super::Free(); }

public:
	void DecideAttackPattern(CGiant* pOwner);

private:
	void Register_States();
	void Register_Transitions();
	void AttackFromIndex(CGiant* pOwner, _int iMoveIndex);

private:
	HitDesc		m_NormalHitDesc = {};
	HitDesc		m_HardHitDesc = {};
	HitDesc		m_KnockOutHitDesc = {};
	HitDesc		m_Attack3HitDesc = {};

};

class CGiant_Attack1 : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack1* Create() { return new CGiant_Attack1(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack2 : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack2* Create() { return new CGiant_Attack2(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack2_1 : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack2_1* Create() { return new CGiant_Attack2_1(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack2_Explode : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack2_Explode* Create() { return new CGiant_Attack2_Explode(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack3 : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack3* Create() { return new CGiant_Attack3(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack3_HitWall : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack3_HitWall* Create() { return new CGiant_Attack3_HitWall(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack4 : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack4* Create() { return new CGiant_Attack4(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack5 : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack5* Create() { return new CGiant_Attack5(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack6_AttackBack : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack6_AttackBack* Create() { return new CGiant_Attack6_AttackBack(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack7 : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack7* Create() { return new CGiant_Attack7(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack7_Jump : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack7_Jump* Create() { return new CGiant_Attack7_Jump(); }
	virtual void Free() override { __super::Free(); }
};

class CGiant_Attack7_Revenge : public IBaseState<CGiant>
{
public:
	virtual void Enter(CGiant* pOwner) override;
	virtual void Update(CGiant* pOwner, _float dt) override;
	virtual void Exit(CGiant* pOwner) override;

public:
	static CGiant_Attack7_Revenge* Create() { return new CGiant_Attack7_Revenge(); }
	virtual void Free() override { __super::Free(); }
};


NS_END