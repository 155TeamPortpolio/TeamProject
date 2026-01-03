#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugBulkyEnforcer;

class CThugBulkyEnforcer_Move : public IHState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Move* Create() { return new CThugBulkyEnforcer_Move(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void RandomWalk(ATTACK_BLACK_BOARD& blackBoard);
};

class CThugBulkyEnforcer_Walk_Front : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Walk_Front* Create() { return new CThugBulkyEnforcer_Walk_Front(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Walk_Left : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Walk_Left* Create() { return new CThugBulkyEnforcer_Walk_Left(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Walk_Right : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Walk_Right* Create() { return new CThugBulkyEnforcer_Walk_Right(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Walk_Back : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Walk_Back* Create() { return new CThugBulkyEnforcer_Walk_Back(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Run_Start : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Run_Start* Create() { return new CThugBulkyEnforcer_Run_Start(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Run : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Run* Create() { return new CThugBulkyEnforcer_Run(); }
	virtual void Free() override { __super::Free(); }
};

class CThugBulkyEnforcer_Run_End : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Run_End* Create() { return new CThugBulkyEnforcer_Run_End(); }
	virtual void Free() override { __super::Free(); }
};

// 왼쪽 대각선으로 이동하며 위빙
class CThugBulkyEnforcer_SideStep_L: public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_SideStep_L* Create() { return new CThugBulkyEnforcer_SideStep_L(); }
	virtual void Free() override { __super::Free(); }
};

// 오른쪽 대각선으로 이동하며 위빙
class CThugBulkyEnforcer_SideStep_R : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_SideStep_R* Create() { return new CThugBulkyEnforcer_SideStep_R(); }
	virtual void Free() override { __super::Free(); }
};

// 백스텝으로 피하는 모션(공격 후 뒤로 빠질때 사용)
class CThugBulkyEnforcer_Evade : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Evade* Create() { return new CThugBulkyEnforcer_Evade(); }
	virtual void Free() override { __super::Free(); }
};

//앞을 응시하며 오른쪽 대각선으로 걸어감(살짝 뒤로걷는 느낌.왼쪽 대각선보다 느림)
class CThugBulkyEnforcer_Walk_RF_LFoot: public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Walk_RF_LFoot* Create() { return new CThugBulkyEnforcer_Walk_RF_LFoot(); }
	virtual void Free() override { __super::Free(); }
};

//앞을 응시하며 오른쪽 대각선으로 걸어감(그냥 걸어감)
class CThugBulkyEnforcer_Walk_FR_RFoot : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Walk_FR_RFoot* Create() { return new CThugBulkyEnforcer_Walk_FR_RFoot(); }
	virtual void Free() override { __super::Free(); }
};

//앞을 응시하며 왼쪽 대각선으로 걸어감(사이드스텝)
class CThugBulkyEnforcer_Walk_FL_RFoot : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Walk_FL_RFoot* Create() { return new CThugBulkyEnforcer_Walk_FL_RFoot(); }
	virtual void Free() override { __super::Free(); }
};


NS_END