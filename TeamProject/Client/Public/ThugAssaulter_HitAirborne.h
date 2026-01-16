#pragma once
#include "IHState.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugAssaulter;

class CThugAssaulter_HitAirborne : public IHState<CThugAssaulter>
{
public:
	virtual void Enter(CThugAssaulter* pOwner) override;
	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
	virtual void Exit(CThugAssaulter* pOwner) override;

public:
	static CThugAssaulter_HitAirborne* Create() { return new CThugAssaulter_HitAirborne(); }
	virtual void Free() override { __super::Free(); }

private:
	void Register_States();
	void Register_Transitions();
};

//class CThugAssaulter_HitAirborne : public IBaseState<CThugAssaulter>
//{
//public:
//	virtual void Enter(CThugAssaulter* pOwner) override;
//	virtual void Update(CThugAssaulter* pOwner, _float dt) override;
//	virtual void Exit(CThugAssaulter* pOwner) override;
//
//public:
//	static CThugAssaulter_HitAirborne* Create() { return new CThugAssaulter_HitAirborne(); }
//	virtual void Free() override { __super::Free(); }
//};

NS_END

	/*
	ThugAssaulter_Ani_Hit_Sky_Fly_Back_OnGround
	ThugAssaulter_Ani_Hit_Sky_Fly_Back_Start
	ThugAssaulter_Ani_Hit_Sky_Fly_Front_OnGround
	ThugAssaulter_Ani_Hit_Sky_Fly_Front_Start
	ThugAssaulter_Ani_Hit_Sky_Fly_Loop
	ThugAssaulter_Ani_Hit_Sky_InAir_OnGround
	ThugAssaulter_Ani_Hit_Sky_InAir_Start
	ThugAssaulter_Ani_Hit_Sky_Knock_Loop
	ThugAssaulter_Ani_Hit_Sky_Knock_OnGround
	ThugAssaulter_Ani_Hit_Sky_Knock_Start
	ThugAssaulter_Ani_Hit_Sky_Throw_High_Back_OnGround
	ThugAssaulter_Ani_Hit_Sky_Throw_High_Back_Start
	ThugAssaulter_Ani_Hit_Sky_Throw_High_Front_OnGround
	ThugAssaulter_Ani_Hit_Sky_Throw_High_Front_Start
	ThugAssaulter_Ani_Hit_Sky_Throw_Loop_Back
	ThugAssaulter_Ani_Hit_Sky_Throw_Loop_Front
	ThugAssaulter_Ani_Hit_Sky_Throw_Low_Back_OnGround
	ThugAssaulter_Ani_Hit_Sky_Throw_Low_Back_Start
	ThugAssaulter_Ani_Hit_Sky_Throw_Low_Front_OnGround
	ThugAssaulter_Ani_Hit_Sky_Throw_Low_Front_Start
	*/
