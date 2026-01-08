#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)
class CSacrifice;
class CSacrificeState_Attack_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_Phase2* Create() { return new CSacrificeState_Attack_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void BuildPattern(CSacrifice* pOwner,ATTACK_BLACK_BOARD& blackBoard);
};

/* Phase2 State */
class CSacrificeState_Attack_01_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_01_Phase2* Create() { return new CSacrificeState_Attack_01_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	_bool m_IsAttackStart = false;

};

class CSacrificeState_Attack_02_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_02_Phase2* Create() { return new CSacrificeState_Attack_02_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Attack_03_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_03_Phase2* Create() { return new CSacrificeState_Attack_03_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Attack_04_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_04_Phase2* Create() { return new CSacrificeState_Attack_04_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	_bool m_IsHandSpawn = false;
};

class CSacrificeState_Attack_05_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_05_Phase2* Create() { return new CSacrificeState_Attack_05_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Attack_05_1_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_05_1_Phase2* Create() { return new CSacrificeState_Attack_05_1_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Attack_08_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_08_Phase2* Create() { return new CSacrificeState_Attack_08_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	_bool m_IsAttackStart = false;
	_bool m_IsAttackEnd = false;	
	_bool m_IsJumpStart = false;

	_float3 m_vFirstTargetPosition{};
	_float3 m_vSecondTargetPosition{};
};
class CSacrificeState_Attack_Charge_Start_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_Charge_Start_Phase2* Create() { return new CSacrificeState_Attack_Charge_Start_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Attack_Charge_Loop_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_Charge_Loop_Phase2* Create() { return new CSacrificeState_Attack_Charge_Loop_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Attack_Charge_U_Start_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_Charge_U_Start_Phase2* Create() { return new CSacrificeState_Attack_Charge_U_Start_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Attack_Charge_U_Loop_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_Charge_U_Loop_Phase2* Create() { return new CSacrificeState_Attack_Charge_U_Loop_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Attack_Charge_U_End_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_Charge_U_End_Phase2* Create() { return new CSacrificeState_Attack_Charge_U_End_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_Attack_Roar_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_Attack_Roar_Phase2* Create() { return new CSacrificeState_Attack_Roar_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_OverDrive_Release_Start_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_OverDrive_Release_Start_Phase2* Create() { return new CSacrificeState_OverDrive_Release_Start_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_OverDrive_Release_Loop_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_OverDrive_Release_Loop_Phase2* Create() { return new CSacrificeState_OverDrive_Release_Loop_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_OverDrive_Release_End_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_OverDrive_Release_End_Phase2* Create() { return new CSacrificeState_OverDrive_Release_End_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_OverDrive_Release_Attack01_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_OverDrive_Release_Attack01_Phase2* Create() { return new CSacrificeState_OverDrive_Release_Attack01_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_OverDrive_Release_Attack02_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_OverDrive_Release_Attack02_Phase2* Create() { return new CSacrificeState_OverDrive_Release_Attack02_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeState_OverDrive_Release_Attack03_Phase2 : public IHState<CSacrifice>
{
public:
	virtual void Enter(CSacrifice* pOwner) override;
	virtual void Update(CSacrifice* pOwner, _float dt) override;
	virtual void Exit(CSacrifice* pOwner) override;

public:
	static CSacrificeState_OverDrive_Release_Attack03_Phase2* Create(){ return new CSacrificeState_OverDrive_Release_Attack03_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	_bool m_IsHandSpawn = false;

};

NS_END
