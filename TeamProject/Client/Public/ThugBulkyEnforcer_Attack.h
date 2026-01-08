#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugBulkyEnforcer;

class CThugBulkyEnforcer_Attack : public IHState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack* Create() { return new CThugBulkyEnforcer_Attack(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
	void BuildPattern(ATTACK_BLACK_BOARD& blackBoard, _int iPatternIndex = 0, _bool isAdditionalMoveState = false);	// 0일때 랜덤
	array<_int, 3>	Pick3RandomIndex();
	void DecideAttackPattern(CThugBulkyEnforcer* pOwner);

private:
	deque<_int>		m_AttackHistory;
};

//앞으로 나오면서 오른손 강펀치
class CThugBulkyEnforcer_Attack1 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack1* Create() { return new CThugBulkyEnforcer_Attack1(); }
	virtual void Free() override { __super::Free(); }
};

// 앞으로 나오면서 오른손 살짝 어퍼컷 다음 왼손 내리꽂으면서 강펀치
class CThugBulkyEnforcer_Attack2 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack2* Create() { return new CThugBulkyEnforcer_Attack2(); }
	virtual void Free() override { __super::Free(); }
};

// 앞으로 도약하면서 양손 아래로 펀치
class CThugBulkyEnforcer_Attack3 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack3* Create() { return new CThugBulkyEnforcer_Attack3(); }
	virtual void Free() override { __super::Free(); }
};

// 바닥 짚기 ? 내려찍기 ? 3과 이어지는 착지하는 모션으로 추정
class CThugBulkyEnforcer_Attack4 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack4* Create() { return new CThugBulkyEnforcer_Attack4(); }
	virtual void Free() override { __super::Free(); }
};

//시퀀스1-1. 왼쪽 대각선 사이드 위빙 후 왼손 훅
class CThugBulkyEnforcer_Attack5_1 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack5_1* Create() { return new CThugBulkyEnforcer_Attack5_1(); }
	virtual void Free() override { __super::Free(); }
};

//시퀀스1-2. 오른쪽 대각선 사이드 위빙으로 파고든 후 오른손 훅 치는 척하면서 왼손 어퍼컷
class CThugBulkyEnforcer_Attack5_2 : public IBaseState<CThugBulkyEnforcer>
{
public:
	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
	virtual void Exit(CThugBulkyEnforcer* pOwner) override;

public:
	static CThugBulkyEnforcer_Attack5_2* Create() { return new CThugBulkyEnforcer_Attack5_2(); }
	virtual void Free() override { __super::Free(); }
};

// 왼쪽 대각선으로 이동하며 위빙
//class CThugBulkyEnforcer_AttackSideStep_L : public IBaseState<CThugBulkyEnforcer>
//{
//public:
//	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
//	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
//	virtual void Exit(CThugBulkyEnforcer* pOwner) override;
//
//public:
//	static CThugBulkyEnforcer_AttackSideStep_L* Create() { return new CThugBulkyEnforcer_AttackSideStep_L(); }
//	virtual void Free() override { __super::Free(); }
//};
//
//// 오른쪽 대각선으로 이동하며 위빙
//class CThugBulkyEnforcer_AttackSideStep_R : public IBaseState<CThugBulkyEnforcer>
//{
//public:
//	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
//	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
//	virtual void Exit(CThugBulkyEnforcer* pOwner) override;
//
//public:
//	static CThugBulkyEnforcer_AttackSideStep_R* Create() { return new CThugBulkyEnforcer_AttackSideStep_R(); }
//	virtual void Free() override { __super::Free(); }
//};
//
//// 백스텝으로 피하는 모션(공격 후 뒤로 빠질때 사용)
//class CThugBulkyEnforcer_AttackEvade : public IBaseState<CThugBulkyEnforcer>
//{
//public:
//	virtual void Enter(CThugBulkyEnforcer* pOwner) override;
//	virtual void Update(CThugBulkyEnforcer* pOwner, _float dt) override;
//	virtual void Exit(CThugBulkyEnforcer* pOwner) override;
//
//public:
//	static CThugBulkyEnforcer_AttackEvade* Create() { return new CThugBulkyEnforcer_AttackEvade(); }
//	virtual void Free() override { __super::Free(); }
//};

NS_END