#include "pch.h"
#include "ThugBulkyEnforcer_Attack.h"
#include "ThugBulkyEnforcer.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "EffectContainer.h"

#include "Animator3D.h"
#include "CharacterController.h"
#include "ThugBulkyEnforcer_Death.h"

void CThugBulkyEnforcer_Death::Enter(CThugBulkyEnforcer* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}
	if (true == pOwner->Get_StateMachine()->Get_Bool("DeathBack")) {
		pOwner->Get_StateMachine()->Set_Bool("DeathBack", false);
		m_pSubStateMachine->Change_State("DeathBack");
	}
	else
		m_pSubStateMachine->Change_State("DeathFront");

	pOwner->Active_Vanish();
}

void CThugBulkyEnforcer_Death::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	pOwner->Update_DeathSquence(dt);
	if (IsCrossAnimProgress(0.4f))
	{
		_vector3 vWorldPosition = pOwner->Get_Component<CTransform>()->Get_WorldPos();
		vWorldPosition.y += 1.5f;

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("enemy_dead.json")
			.Position(vWorldPosition)
			.Build("Enemy_Dead");

		ObjectManager()->Add_Object(effect, { pOwner->Get_Level(),"Enemy_Effect_Layer" });
	}

	if (m_fAnimProgress > 0.99f)
		pOwner->Death();
}

void CThugBulkyEnforcer_Death::Exit(CThugBulkyEnforcer* pOwner)
{
}

void CThugBulkyEnforcer_Death::Register_States()
{
	m_pSubStateMachine->Register_State("DeathFront", CThugBulkyEnforcer_Death_Front::Create());
	m_pSubStateMachine->Register_State("DeathBack", CThugBulkyEnforcer_Death_Back::Create());
	m_pSubStateMachine->Register_State("DeathStay", CThugBulkyEnforcer_Death_Stay::Create());
}

void CThugBulkyEnforcer_Death::Register_Transitions()
{
	//m_pSubStateMachine->Register_Transition("DeathFront", "DeathStay",
	//	CStateMachine<CThugBulkyEnforcer>::CONDITION_ANIMATION_END);
	//m_pSubStateMachine->Register_Transition("DeathBack", "DeathStay",
	//	CStateMachine<CThugBulkyEnforcer>::CONDITION_ANIMATION_END);
}

/*============================================================================*/
void CThugBulkyEnforcer_Death_Front::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Death_Front")
		.Apply();
}

void CThugBulkyEnforcer_Death_Front::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugBulkyEnforcer_Death_Front::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Death_Back::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Death_Back")
		.Apply();
}

void CThugBulkyEnforcer_Death_Back::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugBulkyEnforcer_Death_Back::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Death_Stay::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Death_Stay")
		.Apply();
}

void CThugBulkyEnforcer_Death_Stay::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
}

void CThugBulkyEnforcer_Death_Stay::Exit(CThugBulkyEnforcer* pOwner)
{
}
