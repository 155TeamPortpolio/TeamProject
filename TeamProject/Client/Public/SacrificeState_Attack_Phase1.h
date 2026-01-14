#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Attack_Phase1 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_Phase1* Create() { return new CSacrificeState_Attack_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void BuildPattern(CSacrifice* pOwner);
};

/* Phase1 States */
class CSacrificeState_ArmRecover_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_ArmRecover_Phase1* Create() { return new CSacrificeState_ArmRecover_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Attack_01_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_01_Phase1* Create() { return new CSacrificeState_Attack_01_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrifice* pOwner);
};

class CSacrificeState_Attack_02_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_02_Phase1* Create() { return new CSacrificeState_Attack_02_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	_bool m_IsAttackStart = false;
	_bool m_IsAttackEnd = false;
};

class CSacrificeState_Attack_03_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_03_Phase1* Create() { return new CSacrificeState_Attack_03_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrifice* pOwner);
};

class CSacrificeState_Attack_04_1_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_04_1_Phase1* Create() { return new CSacrificeState_Attack_04_1_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Attack_04_2_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_04_2_Phase1* Create() { return new CSacrificeState_Attack_04_2_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Attack_05_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_05_Phase1* Create() { return new CSacrificeState_Attack_05_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrifice* pOwner);

};

class CSacrificeState_Attack_06_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_06_Phase1* Create() { return new CSacrificeState_Attack_06_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
};

class CSacrificeState_Attack_07_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_07_Phase1* Create() { return new CSacrificeState_Attack_07_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrifice* pOwner);

	_bool m_IsAttackStart = false;
	_bool m_IsAttackEnd = false;
};

class CSacrificeState_Attack_08_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_08_Phase1* Create() { return new CSacrificeState_Attack_08_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrifice* pOwner);
	void Update_Weapons(CSacrifice* pOwner);
	void Update_Move(CSacrifice* pOwner, _float dt);

	_bool m_IsAttackStart = false;
	_bool m_IsAttackEnd = false;
	_bool m_IsJumpStart = false;

	_float3 m_vFirstTargetPosition{};
	_float3 m_vSecondTargetPosition{};
};

class CSacrificeState_Attack_09_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_09_Phase1* Create() { return new CSacrificeState_Attack_09_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Attack_10_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_10_Phase1* Create() { return new CSacrificeState_Attack_10_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Attack_11_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_11_Phase1* Create() { return new CSacrificeState_Attack_11_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Attack_12_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_12_Phase1* Create() { return new CSacrificeState_Attack_12_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Attack_Turn_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_Turn_Phase1* Create() { return new CSacrificeState_Attack_Turn_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

class CSacrificeState_Attack_Roar_Phase1 : public IBaseState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_Roar_Phase1* Create() { return new CSacrificeState_Attack_Roar_Phase1(); }
	virtual void Free() override { __super::Free(); }
};

NS_END