#pragma once
#include "IHState.h"
#include "Defiler_Control.h"

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
protected:
	void ComboTransition(CDefiler* pOwner);

private:
	void Build_Pattern(CDefiler* pOwner, _int Type);
	void ReadySubState();

private:
	_int m_maxPattern = {12};
};

class CDefilerState_Attack_01_01 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_01_01* Create() { return new CDefilerState_Attack_01_01(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_01_02 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_01_02* Create() { return new CDefilerState_Attack_01_02(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_01_01_P2 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_01_01_P2* Create() { return new CDefilerState_Attack_01_01_P2(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_01_03 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_01_03* Create() { return new CDefilerState_Attack_01_03(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_02 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_02* Create() { return new CDefilerState_Attack_02(); }
	virtual void Free() override { __super::Free(); }


};

class CDefilerState_Attack_03 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_03* Create() { return new CDefilerState_Attack_03(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_04 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_04* Create() { return new CDefilerState_Attack_04(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_05 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_05* Create() { return new CDefilerState_Attack_05(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_06 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_06* Create() { return new CDefilerState_Attack_06(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_07 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_07* Create() { return new CDefilerState_Attack_07(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_08_01_Start : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_08_01_Start* Create() { return new CDefilerState_Attack_08_01_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_08_01_Loop : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_08_01_Loop* Create() { return new CDefilerState_Attack_08_01_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_08_01_End : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_08_01_End* Create() { return new CDefilerState_Attack_08_01_End(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_08_02 : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_08_02* Create() { return new CDefilerState_Attack_08_02(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_09_Start : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_09_Start* Create() { return new CDefilerState_Attack_09_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_09_Loop : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_09_Loop* Create() { return new CDefilerState_Attack_09_Loop(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_09_End : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_09_End* Create() { return new CDefilerState_Attack_09_End(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_Grab : public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_Grab* Create() { return new CDefilerState_Attack_Grab(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_Summon:  public CDefilerState_Attack
{
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

public:
	static CDefilerState_Attack_Summon* Create() { return new CDefilerState_Attack_Summon(); }
	virtual void Free() override { __super::Free(); }
};

class CDefilerState_Attack_Evade:  public CDefilerState_Attack
{
	enum EvadeState {EVADE_IN,EVADE_OUT};
public:
	virtual void Enter(CDefiler* pOwner) override;
	virtual void Update(CDefiler* pOwner, _float dt) override;
	virtual void Exit(CDefiler* pOwner) override;

private:
	EvadeState m_eState = { EVADE_IN };
public:
	static CDefilerState_Attack_Evade* Create() { return new CDefilerState_Attack_Evade(); }
	virtual void Free() override { __super::Free(); }
};

NS_END