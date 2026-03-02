#include "pch.h"
#include "Giant.h"
#include "Giant_Death.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "EffectContainer.h"

#include "Animator3D.h"
#include "CharacterController.h"

void CGiant_Death::Enter(CGiant* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CGiant>::Create();

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

void CGiant_Death::Update(CGiant* pOwner, _float dt)
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

void CGiant_Death::Exit(CGiant* pOwner)
{
}

void CGiant_Death::Register_States()
{
	m_pSubStateMachine->Register_State("DeathFront", CGiant_Death_Front::Create());
	m_pSubStateMachine->Register_State("DeathBack", CGiant_Death_Back::Create());
	m_pSubStateMachine->Register_State("DeathStay", CGiant_Death_Stay::Create());
}

void CGiant_Death::Register_Transitions()
{
}

/*============================================================================*/
void CGiant_Death_Front::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Death_Front")
		.Apply();
}

void CGiant_Death_Front::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Death_Front::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Death_Back::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Death_Back")
		.Apply();
}

void CGiant_Death_Back::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Death_Back::Exit(CGiant* pOwner)
{
}

/*============================================================================*/
void CGiant_Death_Stay::Enter(CGiant* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("Giant_Ani_Death_Stay")
		.Apply();
}

void CGiant_Death_Stay::Update(CGiant* pOwner, _float dt)
{
}

void CGiant_Death_Stay::Exit(CGiant* pOwner)
{
}
