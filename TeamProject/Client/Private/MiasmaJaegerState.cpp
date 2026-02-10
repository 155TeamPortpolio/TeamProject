#include "pch.h"
#include "MiasmaGrandierJaeger.h"
#include "MiasmaHeavyJaeger.h"
#include "MiasmaJaegerState.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "BattleSystem.h"

CMiasmaGrandierJaeger_Appear* CMiasmaGrandierJaeger_Appear::Create()
{
	CMiasmaGrandierJaeger_Appear* pInstance = new CMiasmaGrandierJaeger_Appear();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaGrandierJaeger>::Create();
	//pInstance->ReadySubState();

	return pInstance;
}

void CMiasmaGrandierJaeger_Appear::Enter(CMiasmaGrandierJaeger* pOwner)
{
	__super::Enter(pOwner);
	m_AppearTime = Helper::Get_Random_Float(0, 1.f);
}

void CMiasmaGrandierJaeger_Appear::Update(CMiasmaGrandierJaeger* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
	
	m_Duration += dt;
	if (m_AppearTime < m_Duration) {
		pOwner->Get_MainStateMachine()->Set_Trigger("Appear_To_Attack");
	}
}

void CMiasmaGrandierJaeger_Appear::Exit(CMiasmaGrandierJaeger* pOwner)
{
	m_Duration = 0;
	pOwner->Get_Dissolve().Appear(0.5f);
}

//------------------------------------------------------//

CMiasmaGrandierJaeger_Attack* CMiasmaGrandierJaeger_Attack::Create()
{
	CMiasmaGrandierJaeger_Attack* pInstance = new CMiasmaGrandierJaeger_Attack();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaGrandierJaeger>::Create();

	return pInstance;
}

void CMiasmaGrandierJaeger_Attack::Enter(CMiasmaGrandierJaeger* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("GrenadierJaeger_Ani_Attack_02")
		.Speed(1.f)
		.Loop(true)
		.Apply();

}

void CMiasmaGrandierJaeger_Attack::Update(CMiasmaGrandierJaeger* pOwner, _float dt)
{
	if (m_fAnimProgress >= 0.95f)
		pOwner->Get_MainStateMachine()->Set_Trigger("Attack_To_DisAppear");
}

void CMiasmaGrandierJaeger_Attack::Exit(CMiasmaGrandierJaeger* pOwner)
{
	pOwner->LockOn(false);
}

CMiasmaGrandierJaeger_Hit* CMiasmaGrandierJaeger_Hit::Create()
{
	CMiasmaGrandierJaeger_Hit* pInstance = new CMiasmaGrandierJaeger_Hit();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaGrandierJaeger>::Create();

	return pInstance;
}

void CMiasmaGrandierJaeger_Hit::Enter(CMiasmaGrandierJaeger* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("GrenadierJaeger_Ani_Hit_Stay ")
		.Speed(1.f)
		.Loop(false)
		.Apply();

}

void CMiasmaGrandierJaeger_Hit::Update(CMiasmaGrandierJaeger* pOwner, _float dt)
{
	if (m_fAnimProgress >= 0.95f)
		pOwner->Get_MainStateMachine()->Set_Trigger("Appear_To_Attack");
}

void CMiasmaGrandierJaeger_Hit::Exit(CMiasmaGrandierJaeger* pOwner)
{
}

CMiasmaGrandierJaeger_DisAppear* CMiasmaGrandierJaeger_DisAppear::Create()
{
	CMiasmaGrandierJaeger_DisAppear* pInstance = new CMiasmaGrandierJaeger_DisAppear();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaGrandierJaeger>::Create();

	return pInstance;
}
void CMiasmaGrandierJaeger_DisAppear::Enter(CMiasmaGrandierJaeger* pOwner)
{
	pOwner->Get_Dissolve().DisAppear(0.5f);
}

void CMiasmaGrandierJaeger_DisAppear::Update(CMiasmaGrandierJaeger* pOwner, _float dt)
{
	if (pOwner->Get_Dissolve().isComplete()) {
		BattleSystem()->ExitBattleObject(BATTLE_OBJ_TYPE::MONSTER, pOwner->Get_Handle());
		ObjectManager()->Remove_Object(pOwner);
	}
}

void CMiasmaGrandierJaeger_DisAppear::Exit(CMiasmaGrandierJaeger* pOwner)
{
}


//===========================================================

CMiasmaHeavyJaeger_Appear* CMiasmaHeavyJaeger_Appear::Create()
{
	CMiasmaHeavyJaeger_Appear* pInstance = new CMiasmaHeavyJaeger_Appear();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaHeavyJaeger>::Create();
	//pInstance->ReadySubState();

	return pInstance;
}

void CMiasmaHeavyJaeger_Appear::Enter(CMiasmaHeavyJaeger* pOwner)
{
	__super::Enter(pOwner);
	m_AppearTime = 0.1f;
}

void CMiasmaHeavyJaeger_Appear::Update(CMiasmaHeavyJaeger* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	m_Duration += dt;
	if (m_AppearTime < m_Duration) {
		pOwner->Get_MainStateMachine()->Set_Trigger("Appear_To_Attack");
	}
}

void CMiasmaHeavyJaeger_Appear::Exit(CMiasmaHeavyJaeger* pOwner)
{
	pOwner->SpawnChild();
	m_Duration = 0;
	pOwner->Dissolve(true);
}

CMiasmaHeavyJaeger_Attack1* CMiasmaHeavyJaeger_Attack1::Create()
{
	CMiasmaHeavyJaeger_Attack1* pInstance = new CMiasmaHeavyJaeger_Attack1();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaHeavyJaeger>::Create();

	return pInstance;
}

void CMiasmaHeavyJaeger_Attack1::Enter(CMiasmaHeavyJaeger* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("HeavyJaeger_Ani_Attack_01_Start")
		.Speed(1.f)
		.Loop(false)
		.Apply();

	for (auto& child : pOwner->Get_Children())
	{
		auto pChildAnimator = child->Get_Component<CAnimator3D>();
		if (pChildAnimator) {
			pChildAnimator->Change_Animation("HeavyJaeger_Ani_Attack_01_Start")
				.Speed(1.f)
				.Loop(false)
				.Apply();
		}
	}
}

void CMiasmaHeavyJaeger_Attack1::Update(CMiasmaHeavyJaeger* pOwner, _float dt)
{
	if (m_fAnimProgress >= 0.95f)
		pOwner->Get_MainStateMachine()->Set_Trigger("Attack_To_Attack");
}

void CMiasmaHeavyJaeger_Attack1::Exit(CMiasmaHeavyJaeger* pOwner)
{
}


void CMiasmaHeavyJaeger_Attack2::Enter(CMiasmaHeavyJaeger* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("HeavyJaeger_Ani_Attack_01_End")
		.Speed(1.f)
		.Loop(false)
		.Apply();

	for (auto& child : pOwner->Get_Children())
	{
		auto pChildAnimator = child->Get_Component<CAnimator3D>();
		if (pChildAnimator) {
			pChildAnimator->Change_Animation("HeavyJaeger_Ani_Attack_01_End")
				.Speed(1.f)
				.Loop(false)
				.Apply();
		}
	}
}

void CMiasmaHeavyJaeger_Attack2::Update(CMiasmaHeavyJaeger* pOwner, _float dt)
{
	if (m_fAnimProgress >= 0.85f)
		pOwner->Get_MainStateMachine()->Set_Trigger("Attack_To_DisAppear");

}

void CMiasmaHeavyJaeger_Attack2::Exit(CMiasmaHeavyJaeger* pOwner)
{
}

CMiasmaHeavyJaeger_Attack2* CMiasmaHeavyJaeger_Attack2::Create()
{
	CMiasmaHeavyJaeger_Attack2* pInstance = new CMiasmaHeavyJaeger_Attack2();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaHeavyJaeger>::Create();

	return pInstance;
}


CMiasmaHeavyJaeger_DisAppear* CMiasmaHeavyJaeger_DisAppear::Create()
{
	CMiasmaHeavyJaeger_DisAppear* pInstance = new CMiasmaHeavyJaeger_DisAppear();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaHeavyJaeger>::Create();

	return pInstance;
}

void CMiasmaHeavyJaeger_DisAppear::Enter(CMiasmaHeavyJaeger* pOwner)
{
	pOwner->Dissolve(false);
}

void CMiasmaHeavyJaeger_DisAppear::Update(CMiasmaHeavyJaeger* pOwner, _float dt)
{
	if (pOwner->Get_Dissolve().isComplete()) {
		BattleSystem()->ExitBattleObject(BATTLE_OBJ_TYPE::MONSTER, pOwner->Get_Handle());
		ObjectManager()->Remove_Object(pOwner);
	}
}

void CMiasmaHeavyJaeger_DisAppear::Exit(CMiasmaHeavyJaeger* pOwner)
{
}
