#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CStrikeJaeger;

class CStrikeJaeger_Attack : public IHState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Attack* Create() { return new CStrikeJaeger_Attack(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void AttackFromIndex(_int iMoveIndex);

private:
	HitDesc		m_HitDesc = {};
};

class CStrikeJaeger_Attack1 : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Attack1* Create() { return new CStrikeJaeger_Attack1(); }
	virtual void Free() override { __super::Free(); }
};

class CStrikeJaeger_Attack3 : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Attack3* Create() { return new CStrikeJaeger_Attack3(); }
	virtual void Free() override { __super::Free(); }
};

class CStrikeJaeger_Attack5: public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Attack5* Create() { return new CStrikeJaeger_Attack5(); }
	virtual void Free() override { __super::Free(); }
};


NS_END