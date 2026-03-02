#include "pch.h"
#include "ThugAssaulter.h"
#include "ThugAssaulter_Death.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "EffectContainer.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CThugAssaulter_Death::Enter(CThugAssaulter* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugAssaulter>::Create();

		Register_States();
		Register_Transitions();

		__super::Enter(pOwner);
	}
	if (true == pOwner->GetStateMachine()->Get_Bool("DeathBack")) {
		pOwner->GetStateMachine()->Set_Bool("DeathBack", false);
		m_pSubStateMachine->Change_State("DeathBack");
	}
	else
		m_pSubStateMachine->Change_State("DeathFront");

	pOwner->Active_Vanish();
}

void CThugAssaulter_Death::Update(CThugAssaulter* pOwner, _float dt)
{
	_vector3 vRootBoneMoveDelta = pOwner->Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
	_quaternion qRot = pOwner->Get_Component<CTransform>()->Get_QuaternionRotate();
	pOwner->Get_Component<CCharacterController>()->Move_RootMotion(
		vRootBoneMoveDelta,
		qRot,
		dt);
	
	__super::Update(pOwner, dt);

	pOwner->Update_DeathSquence(dt);
	if (IsCrossAnimProgress(0.4f))
	{
		_vector3 vWorldPosition = pOwner->Get_Component<CTransform>()->Get_WorldPos();
		vWorldPosition.y += 1.f;

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("enemy_dead.json")
			.Position(vWorldPosition)
			.Build("Enemy_Dead");

		ObjectManager()->Add_Object(effect, { pOwner->Get_Level(),"Enemy_Effect_Layer" });
		pOwner->Death();
	}
}

void CThugAssaulter_Death::Exit(CThugAssaulter* pOwner)
{

}

void CThugAssaulter_Death::Register_States()
{
	m_pSubStateMachine->Register_State("DeathFront", CThugAssaulter_Death_Front::Create());
	m_pSubStateMachine->Register_State("DeathBack", CThugAssaulter_Death_Back::Create());
	m_pSubStateMachine->Register_State("DeathStay", CThugAssaulter_Death_Stay::Create());
}

void CThugAssaulter_Death::Register_Transitions()
{
}

/*============================================================================*/
void CThugAssaulter_Death_Front::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Death_Front")
		.Apply();
}

void CThugAssaulter_Death_Front::Update(CThugAssaulter* pOwner, _float dt)
{
	
}

void CThugAssaulter_Death_Front::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Death_Back::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Death_Back")
		.Apply();
}

void CThugAssaulter_Death_Back::Update(CThugAssaulter* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.99f)
		pOwner->Death();
}

void CThugAssaulter_Death_Back::Exit(CThugAssaulter* pOwner)
{
}

/*============================================================================*/
void CThugAssaulter_Death_Stay::Enter(CThugAssaulter* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugAssaulter_Ani_Death_Stay")
		.Apply();
}

void CThugAssaulter_Death_Stay::Update(CThugAssaulter* pOwner, _float dt)
{
}

void CThugAssaulter_Death_Stay::Exit(CThugAssaulter* pOwner)
{
}
 