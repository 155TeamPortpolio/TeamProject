#include "pch.h"
#include "MiasmaGrandierJaeger.h"
#include "MiasmaJaegerState.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "BattleSystem.h"

CMiasmaJaeger_Appear* CMiasmaJaeger_Appear::Create()
{
	CMiasmaJaeger_Appear* pInstance = new CMiasmaJaeger_Appear();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaGrandierJaeger>::Create();
	//pInstance->ReadySubState();

	return pInstance;
}

void CMiasmaJaeger_Appear::Enter(CMiasmaGrandierJaeger* pOwner)
{
	__super::Enter(pOwner);
	m_AppearTime = Helper::Get_Random_Float(0, 1.f);
}

void CMiasmaJaeger_Appear::Update(CMiasmaGrandierJaeger* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
	
	m_Duration += dt;
	if (m_AppearTime < m_Duration) {
		pOwner->Get_MainStateMachine()->Set_Trigger("Appear_To_Attack");
	}
}

void CMiasmaJaeger_Appear::Exit(CMiasmaGrandierJaeger* pOwner)
{
	m_Duration = 0;
	pOwner->Get_Dissolve().Appear(0.5f);
}

//------------------------------------------------------//

CMiasmaJaeger_Attack* CMiasmaJaeger_Attack::Create()
{
	CMiasmaJaeger_Attack* pInstance = new CMiasmaJaeger_Attack();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaGrandierJaeger>::Create();

	return pInstance;
}

void CMiasmaJaeger_Attack::Enter(CMiasmaGrandierJaeger* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("GrenadierJaeger_Ani_Attack_02")
		.Speed(1.f)
		.Loop(true)
		.Apply();

}

void CMiasmaJaeger_Attack::Update(CMiasmaGrandierJaeger* pOwner, _float dt)
{
	//if (m_fAnimProgress >= 0.95f)
	//	pOwner->Get_MainStateMachine()->Set_Trigger("Attack_To_DisAppear");
}

void CMiasmaJaeger_Attack::Exit(CMiasmaGrandierJaeger* pOwner)
{
	pOwner->LockOn(false);
}

CMiasmaJaeger_Hit* CMiasmaJaeger_Hit::Create()
{
	CMiasmaJaeger_Hit* pInstance = new CMiasmaJaeger_Hit();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaGrandierJaeger>::Create();

	return pInstance;
}

void CMiasmaJaeger_Hit::Enter(CMiasmaGrandierJaeger* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("GrenadierJaeger_Ani_Hit_Stay ")
		.Speed(1.f)
		.Loop(false)
		.Apply();

}

void CMiasmaJaeger_Hit::Update(CMiasmaGrandierJaeger* pOwner, _float dt)
{
	if (m_fAnimProgress >= 0.95f)
		pOwner->Get_MainStateMachine()->Set_Trigger("Appear_To_Attack");
}

void CMiasmaJaeger_Hit::Exit(CMiasmaGrandierJaeger* pOwner)
{
}

CMiasmaJaeger_DisAppear* CMiasmaJaeger_DisAppear::Create()
{
	CMiasmaJaeger_DisAppear* pInstance = new CMiasmaJaeger_DisAppear();
	pInstance->m_pSubStateMachine = CStateMachine<CMiasmaGrandierJaeger>::Create();

	return pInstance;
}
void CMiasmaJaeger_DisAppear::Enter(CMiasmaGrandierJaeger* pOwner)
{
	pOwner->Get_Dissolve().DisAppear(0.5f);
}

void CMiasmaJaeger_DisAppear::Update(CMiasmaGrandierJaeger* pOwner, _float dt)
{
	if (pOwner->Get_Dissolve().isComplete()) {
		BattleSystem()->ExitBattleObject(BATTLE_OBJ_TYPE::MONSTER, pOwner->Get_Handle());
		ObjectManager()->Remove_Object(pOwner);
	}
}

void CMiasmaJaeger_DisAppear::Exit(CMiasmaGrandierJaeger* pOwner)
{
}
