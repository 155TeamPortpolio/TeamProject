#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CStrikeJaeger;

class CStrikeJaeger_Groggy : public IHState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Groggy* Create() { return new CStrikeJaeger_Groggy(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

class CStrikeJaeger_Stun_Start : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Stun_Start* Create() { return new CStrikeJaeger_Stun_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CStrikeJaeger_Stun_Loop : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Stun_Loop* Create() { return new CStrikeJaeger_Stun_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CStrikeJaeger_Stun_End : public IBaseState<CStrikeJaeger>
{
public:
	virtual void Enter(CStrikeJaeger* pOwner) override;
	virtual void Update(CStrikeJaeger* pOwner, _float dt) override;
	virtual void Exit(CStrikeJaeger* pOwner) override;

public:
	static CStrikeJaeger_Stun_End* Create() { return new CStrikeJaeger_Stun_End(); }
	virtual void Free() override { __super::Free(); }
};

NS_END