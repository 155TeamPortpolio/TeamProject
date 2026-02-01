#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CClaymore;

class CClaymore_Attack : public IHState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Attack* Create() { return new CClaymore_Attack(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void AttackFromIndex(_int iMoveIndex);

private:
	HitDesc		m_HitDesc = {};
};

class CClaymore_Attack1 : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Attack1* Create() { return new CClaymore_Attack1(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Attack2 : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Attack2* Create() { return new CClaymore_Attack2(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Attack2b : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Attack2b* Create() { return new CClaymore_Attack2b(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Attack3 : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Attack3* Create() { return new CClaymore_Attack3(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Attack3_End : public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Attack3_End* Create() { return new CClaymore_Attack3_End(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Attack4: public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Attack4* Create() { return new CClaymore_Attack4(); }
	virtual void Free() override { __super::Free(); }
};

class CClaymore_Attack5: public IBaseState<CClaymore>
{
public:
	virtual void Enter(CClaymore* pOwner) override;
	virtual void Update(CClaymore* pOwner, _float dt) override;
	virtual void Exit(CClaymore* pOwner) override;

public:
	static CClaymore_Attack5* Create() { return new CClaymore_Attack5(); }
	virtual void Free() override { __super::Free(); }
};


NS_END