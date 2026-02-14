#include "pch.h"
#include "MeleeJaeger.h"
#include "MeleeJaeger_Death.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "EffectContainer.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CMeleeJaeger_Death::Enter(CMeleeJaeger* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CMeleeJaeger>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}
	if (true == pOwner->GetStateMachine()->Get_Bool("DeathBack")) {
		pOwner->GetStateMachine()->Set_Bool("DeathBack", false);
		m_pSubStateMachine->Change_State("DeathBack");
	}
	else
		m_pSubStateMachine->Change_State("DeathStay");

	pOwner->Active_Vanish();
}

void CMeleeJaeger_Death::Update(CMeleeJaeger* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);

	__super::Update(pOwner, dt);

	if (m_fAnimProgress > 0.4f)
		pOwner->Update_DeathSquence(dt);
	//pOwner->GetStateMachine()->Get_Float("DeathDisappearProgress");
	if (IsCrossAnimProgress(0.65f))
	{
		_vector3 vWorldPosition = pOwner->Get_Component<CTransform>()->Get_WorldPos();
		vWorldPosition.y += 0.2f;

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("enemy_dead.json")
			.Position(vWorldPosition)
			.Build("Enemy_Dead");

		ObjectManager()->Add_Object(effect, { pOwner->Get_Level(),"Enemy_Effect_Layer" });
		pOwner->Death();
	}
}

void CMeleeJaeger_Death::Exit(CMeleeJaeger* pOwner)
{
}

void CMeleeJaeger_Death::Register_States()
{
	m_pSubStateMachine->Register_State("DeathFront", CMeleeJaeger_Death_Front::Create());
	m_pSubStateMachine->Register_State("DeathBack", CMeleeJaeger_Death_Back::Create());
	m_pSubStateMachine->Register_State("DeathStay", CMeleeJaeger_Death_Stay::Create());
}

void CMeleeJaeger_Death::Register_Transitions()
{
}

/*============================================================================*/
void CMeleeJaeger_Death_Front::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Death_Front")
		.Apply();
}

void CMeleeJaeger_Death_Front::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Death_Front::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Death_Back::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Death_Back")
		.Apply();
}

void CMeleeJaeger_Death_Back::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Death_Back::Exit(CMeleeJaeger* pOwner)
{
}

/*============================================================================*/
void CMeleeJaeger_Death_Stay::Enter(CMeleeJaeger* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("MeleeJaeger_Ani_Death_Stay")
		.Apply();
}

void CMeleeJaeger_Death_Stay::Update(CMeleeJaeger* pOwner, _float dt)
{
}

void CMeleeJaeger_Death_Stay::Exit(CMeleeJaeger* pOwner)
{
}
