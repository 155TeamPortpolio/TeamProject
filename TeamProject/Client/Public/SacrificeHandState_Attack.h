#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)
class CSacrificeHand;
class CSacrificeHandState_Attack : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack* Create() { return new CSacrificeHandState_Attack(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void BuildPattern(CSacrificeHand* pOwner);
};

/* Sub States */
class CSacrificeHandState_Attack_01_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_01_Phase1* Create() { return new CSacrificeHandState_Attack_01_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrificeHand* pOwner);
};

class CSacrificeHandState_Attack_02_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_02_Phase1* Create() { return new CSacrificeHandState_Attack_02_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrificeHand* pOwner);

};

class CSacrificeHandState_Attack_03_Phase1 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_03_Phase1* Create() { return new CSacrificeHandState_Attack_03_Phase1(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrificeHand* pOwner);

	_bool m_IsActiveHand = false;
};

class CSacrificeHandState_Attack_04_Phase2 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_Attack_04_Phase2* Create() { return new CSacrificeHandState_Attack_04_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	_bool m_IsActiveHand = false;
};

class CSacrificeHandState_OverDrive_Release_Start_Phase2 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_OverDrive_Release_Start_Phase2* Create() { return new CSacrificeHandState_OverDrive_Release_Start_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrificeHand* pOwner);

};

class CSacrificeHandState_OverDrive_Release_Loop_Phase2 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_OverDrive_Release_Loop_Phase2* Create() { return new CSacrificeHandState_OverDrive_Release_Loop_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
};

class CSacrificeHandState_OverDrive_Release_End_Phase2 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_OverDrive_Release_End_Phase2* Create() { return new CSacrificeHandState_OverDrive_Release_End_Phase2(); }
	virtual void Free() override { __super::Free(); }

};

class CSacrificeHandState_OverDrive_Release_Attack01_Phase2 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_OverDrive_Release_Attack01_Phase2* Create() { return new CSacrificeHandState_OverDrive_Release_Attack01_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	void Update_Effects(CSacrificeHand* pOwner);

};

class CSacrificeHandState_OverDrive_Release_Attack02_Phase2 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_OverDrive_Release_Attack02_Phase2* Create() { return new CSacrificeHandState_OverDrive_Release_Attack02_Phase2(); }
	virtual void Free() override { __super::Free(); }

private:
	void Rotate_ToTarget(CSacrificeHand* pOwner, _float dt);

};

class CSacrificeHandState_OverDrive_Release_Attack03_Phase2 : public IHState<CSacrificeHand>
{
public:
	virtual void Enter(CSacrificeHand* pOwner) override;
	virtual void Update(CSacrificeHand* pOwner, _float dt) override;
	virtual void Exit(CSacrificeHand* pOwner) override;

public:
	static CSacrificeHandState_OverDrive_Release_Attack03_Phase2* Create() { return new CSacrificeHandState_OverDrive_Release_Attack03_Phase2(); }
	virtual void Free() override { __super::Free(); }

};


NS_END
