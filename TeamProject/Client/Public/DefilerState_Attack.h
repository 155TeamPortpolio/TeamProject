#pragma once
#include "IHState.h"

NS_BEGIN(Client)

class CDefiler;
class CDefilerState_Attack : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack* Create();
	virtual void Free() override { __super::Free(); }
private:
	void Build_Pattern(CDefiler* pOwner);
	void ReadySubState();
};

class CDefilerState_Attack_01_01 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_01_01* Create() { return new CDefilerState_Attack_01_01(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_01_02 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_01_02* Create() { return new CDefilerState_Attack_01_02(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_01_01_P2 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_01_01_P2* Create() { return new CDefilerState_Attack_01_01_P2(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_01_03 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_01_03* Create() { return new CDefilerState_Attack_01_03(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_02 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_02* Create() { return new CDefilerState_Attack_02(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_03 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_03* Create() { return new CDefilerState_Attack_03(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_04 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_04* Create() { return new CDefilerState_Attack_04(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_05 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_05* Create() { return new CDefilerState_Attack_05(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_06 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_06* Create() { return new CDefilerState_Attack_06(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_07 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_07* Create() { return new CDefilerState_Attack_07(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_08_01_Start : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_08_01_Start* Create() { return new CDefilerState_Attack_08_01_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_08_01_Loop : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_08_01_Loop* Create() { return new CDefilerState_Attack_08_01_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_08_01_End : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_08_01_End* Create() { return new CDefilerState_Attack_08_01_End(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_08_02 : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_08_02* Create() { return new CDefilerState_Attack_08_02(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_09_Start : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_09_Start* Create() { return new CDefilerState_Attack_09_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_09_Loop : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_09_Loop* Create() { return new CDefilerState_Attack_09_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_09_End : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_09_End* Create() { return new CDefilerState_Attack_09_End(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_Grab : public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_Grab* Create() { return new CDefilerState_Attack_Grab(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_Summon: public IHState<CDefiler>
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_Summon* Create() { return new CDefilerState_Attack_Summon(); }
	virtual void Free() override { __super::Free(); }
};

NS_END