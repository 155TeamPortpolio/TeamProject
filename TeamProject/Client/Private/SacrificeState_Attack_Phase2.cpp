#include "pch.h"
#include "SacrificeState_Attack_Phase2.h"
#include "Helper_Func.h"
#include "GameInstance.h"

/* Object */
#include "Sacrifice.h"
#include "EffectContainer.h"

/* Component */
#include "CharacterController.h"

void CSacrificeState_Attack_Phase2::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		Register_States();
		Register_Transitions();
	}

	BuildPattern(pOwner);

	__super::Enter(pOwner);
}

void CSacrificeState_Attack_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (blackBoard.isRequestNext)
	{
		blackBoard.isRequestNext = false;
		blackBoard.isChainOpen = false;

		if (!blackBoard.stateQueue.empty())
		{
			string nextStateTag = blackBoard.stateQueue.front();
			blackBoard.stateQueue.pop_front();

			blackBoard.currentStateTag = nextStateTag;
			m_pSubStateMachine->Change_State(nextStateTag);
		}
	}

	if (blackBoard.isChainOpen && blackBoard.stateQueue.empty())
	{
		_uint iRandIndex = Helper::Get_Random_Int(0, 1);
		if (0 == iRandIndex || blackBoard.currentStateTag == "OverDrive_Loop" || blackBoard.currentStateTag == "OverDrive_Attack03")
			pOwner->Idle();
		else
			pOwner->Evade();
	}
}

void CSacrificeState_Attack_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Phase2::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01_Phase2",CSacrificeState_Attack_01_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack02_Phase2",CSacrificeState_Attack_02_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack03_Phase2",CSacrificeState_Attack_03_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack04_Phase2",CSacrificeState_Attack_04_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack05_Phase2",CSacrificeState_Attack_05_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack05_1_Phase2",CSacrificeState_Attack_05_1_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack08_Phase2", CSacrificeState_Attack_08_Phase2::Create());

	m_pSubStateMachine->Register_State("Attack_Charge_Start_Phase2", CSacrificeState_Attack_Charge_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_Loop_Phase2", CSacrificeState_Attack_Charge_Loop_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_U_Start_Phase2",CSacrificeState_Attack_Charge_U_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_U_Loop_Phase2", CSacrificeState_Attack_Charge_U_Loop_Phase2::Create());
	m_pSubStateMachine->Register_State("Attack_Charge_U_End_Phase2",CSacrificeState_Attack_Charge_U_End_Phase2::Create());

	m_pSubStateMachine->Register_State("Attack_Roar_Phase2", CSacrificeState_Attack_Roar_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Start", CSacrificeState_OverDrive_Release_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Loop", CSacrificeState_OverDrive_Release_Loop_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_End", CSacrificeState_OverDrive_Release_End_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Attack01", CSacrificeState_OverDrive_Release_Attack01_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Attack02", CSacrificeState_OverDrive_Release_Attack02_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Attack03", CSacrificeState_OverDrive_Release_Attack03_Phase2::Create());
}

void CSacrificeState_Attack_Phase2::Register_Transitions()
{
}

void CSacrificeState_Attack_Phase2::BuildPattern(CSacrifice* pOwner)
{
	TARGETING_INFO& targetInfo = pOwner->GetTargetingInfo();

	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.iPatternCount++;

	blackBoard.stateQueue.clear();

	if (pOwner->IsOverDrive())
	{
		if (!pOwner->IsOverDriveCharged())
		{
			blackBoard.stateQueue.push_back("Attack_Roar_Phase2");
			blackBoard.stateQueue.push_back("OverDrive_Start");
			blackBoard.stateQueue.push_back("OverDrive_Loop");
		}
		else
		{
			blackBoard.stateQueue.push_back("OverDrive_Attack01");
			blackBoard.stateQueue.push_back("OverDrive_Attack02");
			blackBoard.stateQueue.push_back("OverDrive_Attack03");
		}
	}
	else
	{
		if (blackBoard.iPatternCount >= 4)
		{
			blackBoard.iPatternCount = 0;
			blackBoard.stateQueue.push_back("Attack_Charge_Start_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_Loop_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_U_Start_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_U_Loop_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_U_End_Phase2");
		}
		else
		{
			_uint iRandIndex = Helper::Get_Random_Int(0, 3);
			switch (iRandIndex)
			{
			case 0:
			{
				blackBoard.stateQueue.push_back("Attack01_Phase2");
				blackBoard.stateQueue.push_back("Attack02_Phase2");

			}break;
			case 1:
			{
				blackBoard.stateQueue.push_back("Attack04_Phase2");
			}break;
			case 2:
			{
				blackBoard.stateQueue.push_back("Attack03_Phase2");
			}break;
			case 3:
			{
				blackBoard.stateQueue.push_back("Attack08_Phase2");
			}break;
			case 4:
			{
				blackBoard.stateQueue.push_back("Attack05_1_Phase2");
				blackBoard.stateQueue.push_back("Attack05_Phase2");
			}break;
			default:
				break;
			}
		}
	}

	blackBoard.isRequestNext = true;
}
		


void CSacrificeState_Attack_01_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_01").Loop(false).Speed(1.2f).Apply();

	HitDesc desc{};
	pOwner->SetAutoPlayBattleCollider("Whip", 0.f, 0.3f, desc);
	pOwner->Active_AttackSign();

	m_IsAttackStart = false;
}

void CSacrificeState_Attack_01_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	if (!m_IsAttackStart && m_fAnimProgress >= 0.1f)
	{
		m_IsAttackStart = true;
		pOwner->ActiveWhip();
	}

	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->DeactiveWhip();
	}

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_01_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_02_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_02").Loop(false).Speed(1.2f).Apply();

	HitDesc hitDesc{};
	pOwner->SetAutoPlayBattleCollider("Whip", 0.f, 0.3f, hitDesc);
	pOwner->Active_AttackSign();
	pOwner->ActiveWhip();
}

void CSacrificeState_Attack_02_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
		pOwner->DeactiveWhip();
	}

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_02_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_03_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_03").Loop(false).Speed(1.f).Apply();

	HitDesc desc{};
	pOwner->SetAutoPlayBattleCollider("Whip", 0.f, 0.2f, desc);
	pOwner->Active_AttackSign();
	pOwner->ActiveWhip();
}

void CSacrificeState_Attack_03_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
		pOwner->DeactiveWhip();
	}

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_03_Phase2::Exit(CSacrifice* pOwner)
{
	pOwner->DeactiveWhip();
}

void CSacrificeState_Attack_04_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack04").Loop(false).Speed(1.f).Apply();

	m_IsHandSpawn = false;
}

void CSacrificeState_Attack_04_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (!m_IsHandSpawn && m_fAnimProgress >= 0.4f)
	{
		pOwner->Phase2Attack();
		m_IsHandSpawn = true;
	}

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Attack_04_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Attack_05").Loop(false).Speed(1.2f).Apply();

	pOwner->Active_AttackSign();
}

void CSacrificeState_Attack_05_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.45f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if(m_fAnimProgress<0.4f)
		pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_05_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_1_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Attack_05_1").Loop(false).Speed(1.f).Apply();
}

void CSacrificeState_Attack_05_1_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.3f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_05_1_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_08_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Attack_08").Loop(false).Speed(1.2f).Apply();

	pOwner->Active_AttackSign();

	m_IsAttackStart = false;
	m_IsAttackEnd = false; 
	m_IsJumpStart = false;

	_vector3 vCurrDir = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
	_vector3 vTargetPos = pOwner->GetTargetingInfo().vTargetPos;
	m_vFirstTargetPosition = vTargetPos - vCurrDir * 2.f;
}

void CSacrificeState_Attack_08_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	Update_Move(pOwner, dt);
	Update_Weapons(pOwner);
	Update_Effects(pOwner);
}

void CSacrificeState_Attack_08_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_08_Phase2::Update_Weapons(CSacrifice* pOwner)
{
	if (IsCrossAnimProgress(0.05f))
	{
		pOwner->ActiveAxe();
		m_IsAttackStart = true;
	}
	
	if (IsCrossAnimProgress(0.85f))
	{
		pOwner->DeactiveAxe();
		m_IsAttackEnd = true;
	}
}

void CSacrificeState_Attack_08_Phase2::Update_Move(CSacrifice* pOwner, _float dt)
{
	/* Move Update */
	if (m_fAnimProgress < 0.5f)
		pOwner->RotateToTarget(dt, 10.f);

	auto pCCT = pOwner->Get_Component<CCharacterController>();
	if (!m_IsJumpStart && m_fAnimProgress >= 0.12f)
	{
		_vector3 vCurrDir = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
		_vector3 vTargetPos = pOwner->GetTargetingInfo().vTargetPos;
		m_vSecondTargetPosition = vTargetPos - vCurrDir * 2.f;
		m_IsJumpStart = true;
	}

	if (m_IsAttackStart)
	{
		if (m_fAnimProgress < 0.12f)
		{
			_vector3 vCurrPosition = pOwner->Get_Component<CTransform>()->Get_Pos();
			_vector3 vNextPosition = _vector3::Lerp(vCurrPosition, m_vFirstTargetPosition, dt * 3.f);
			_vector3 vVelocity = (vNextPosition - vCurrPosition) / dt;
			pCCT->Move_Velocity(vVelocity, dt);
		}
		else if (m_fAnimProgress < 0.5f)
		{
			_vector3 vCurrPosition = pOwner->Get_Component<CTransform>()->Get_Pos();
			_vector3 vNextPosition = _vector3::Lerp(vCurrPosition, m_vSecondTargetPosition, dt * 3.f);
			_vector3 vVelocity = (vNextPosition - vCurrPosition) / dt;
			pCCT->Move_Velocity(vVelocity, dt);
		}
	}
}

void CSacrificeState_Attack_08_Phase2::Update_Effects(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	auto pTransform = pOwner->Get_Component<CTransform>();

	/* Jump Start */
	if (IsCrossAnimProgress(0.13f))
	{
		_vector3 vBonePosition = pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, "Skn_Finger2_03");
		vBonePosition = _vector3::Transform(vBonePosition, pTransform->Get_WorldMatrix());
		vBonePosition.y -= 0.2f;

		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("hit_ground_smoke.json")
			.Position(vBonePosition)
			.Build("Smoke");

		auto pRockParticle = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("rock_particle.json")
			.Position(vBonePosition)
			.Build("Rock");

		ObjectManager()->Add_Object(pEffect, { pOwner->Get_Level(),"Effect_Layer" });
		ObjectManager()->Add_Object(pRockParticle, { pOwner->Get_Level(),"Effect_Layer" });
	}

	/* Axe Slash2 */
	if (IsCrossAnimProgress(0.17f))
	{
		_smatrix worldMatrix = pTransform->Get_WorldMatrix();
		_vector3 vWorldPosition = _vector3::Transform(_vector3(0.3f, 2.5f, 0.f), worldMatrix);

		_quaternion worldQuaternion = pTransform->Get_QuaternionRotate();
		_quaternion localQuaternion = _quaternion(0.9f, 0.27f, -0.35f, 0.03f);
		localQuaternion *= worldQuaternion;

		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_axe_slash2.json")
			.Position(vWorldPosition)
			.Build("AxeSlash2");

		pEffect->Get_Component<CTransform>()->Set_Quaternion(localQuaternion);

		ObjectManager()->Add_Object(pEffect, { pOwner->Get_Level(),"Effect_Layer" });
	}

	/* Axe Slash */
	if (IsCrossAnimProgress(0.3f))
	{
		_smatrix worldMatrix = pTransform->Get_WorldMatrix();
		_vector3 vWorldPosition = _vector3::Transform(_vector3(-0.2f, 5.8f, 3.6f), worldMatrix);

		_quaternion worldQuaternion = pTransform->Get_QuaternionRotate();
		_quaternion localQuaternion = _quaternion(-0.07f, -0.01f, -0.64f, 0.77f);
		localQuaternion *= worldQuaternion;

		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_axe_slash.json")
			.Position(vWorldPosition)
			.Build("AxeSlash");

		pEffect->Get_Component<CTransform>()->Set_Quaternion(localQuaternion);

		ObjectManager()->Add_Object(pEffect, { pOwner->Get_Level(),"Effect_Layer" });
	}

	if (IsCrossAnimProgress(0.34f))
	{
		auto pAnimator = pOwner->Get_Component<CAnimator3D>();
		auto pTransform = pOwner->Get_Component<CTransform>();

		_vector3 vBonePosition = pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, "Bip001 L Hand");
		vBonePosition = _vector3::Transform(vBonePosition, pTransform->Get_WorldMatrix());
		vBonePosition.y -= 0.2f;
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("hit_ground_smoke_strong.json")
			.Position(vBonePosition)
			.Build("Smoke");

		ObjectManager()->Add_Object(pEffect, { pOwner->Get_Level(),"Effect_Layer" });
	}
}

void CSacrificeState_Attack_Charge_Start_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Charge_Start").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_Charge_Start_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.9f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_Charge_Start_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_Loop_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Charge_Loop").Loop(true).Speed(1.4f).Apply();

	m_IsStartDissolve = false;
	m_IsEndDissolve = false;
}

void CSacrificeState_Attack_Charge_Loop_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (!m_IsStartDissolve && m_fStateTime >= 0.4f)
		{
			pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::DISAPPEAR, 0.3f);
			m_IsStartDissolve = true;
		}

		if (!m_IsEndDissolve && m_fStateTime >= 0.7f)
		{
			pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::APPEAR, 0.3f);
			pOwner->Get_Component<CCharacterController>()->Set_Position(_vector3(-2.f, 1.f, 21.f));
			m_IsEndDissolve = true;
		}

		if (m_fStateTime >= 1.5f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
			{
				blackBoard.isRequestNext = true;
				pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::NONE, 0.f);
			}
		}
	}

	pOwner->Update_Dissolve(dt);
	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_Charge_Loop_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_U_Start_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Charge_U_Start").Loop(false).Speed(1.4f).Apply();

	pOwner->Set_HitBlendable(false);
}

void CSacrificeState_Attack_Charge_U_Start_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (blackBoard.stateQueue.empty())
	{
		if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
			blackBoard.isChainOpen = true;
	}
	else
	{
		if (m_fAnimProgress >= 0.9f)
		{
			blackBoard.isChainOpen = true;
			if (!blackBoard.stateQueue.empty())
				blackBoard.isRequestNext = true;
		}
	}

	if (IsCrossAnimProgress(0.7f))
		pOwner->ActiveEyeLaser();
}

void CSacrificeState_Attack_Charge_U_Start_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_U_Loop_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Charge_U_Loop").Loop(true).Speed(1.4f).Apply();

}

void CSacrificeState_Attack_Charge_U_Loop_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fStateTime >= 2.f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Attack_Charge_U_Loop_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_U_End_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
   	pAnimator->Change_Animation("Take 001Monster_SacrificeBringer_Ani_P2_Charge_U_End").Loop(false).Speed(1.6f).Apply();
}

void CSacrificeState_Attack_Charge_U_End_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->Set_HitBlendable(true);
	}

	if (IsCrossAnimProgress(0.01f))
		pOwner->DeactiveEyeLaser();
}

void CSacrificeState_Attack_Charge_U_End_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Roar_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Stun_Roar").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_Roar_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::NONE, 0.f);
	}

	if (IsCrossAnimProgress(0.5f))
		pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::DISAPPEAR, 0.2f);

	if (IsCrossAnimProgress(0.7f))
	{
		pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::APPEAR, 0.2f);
		pOwner->Get_Component<CCharacterController>()->Set_Position(_vector3(-15.f, 1.f, 22.f));
		pOwner->Get_Component<CTransform>()->Set_Look(_vector3(1.f, 0.f, 0.f));
	}

	pOwner->Update_Dissolve(dt);
}

void CSacrificeState_Attack_Roar_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Start_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Charge_Start_New").Loop(false).Speed(1.2f).Apply();

	pOwner->OverDrive_Start();
}

void CSacrificeState_OverDrive_Release_Start_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress>=0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_OverDrive_Release_Start_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Loop_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Charge_Loop").Loop(true).Speed(1.2f).Apply();
}

void CSacrificeState_OverDrive_Release_Loop_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fStateTime >= 3.5f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->SetOverDriveCharged(true);
	}
}

void CSacrificeState_OverDrive_Release_Loop_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_End_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Start").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_OverDrive_Release_End_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (pOwner->Get_Component<CAnimator3D>()->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_OverDrive_Release_End_Phase2::Exit(CSacrifice* pOwner)
{
}


void CSacrificeState_OverDrive_Release_Attack01_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack01").Loop(false).Speed(1.2f).Apply();

	pOwner->OverDrive_Attack1();
	pOwner->ActiveSword();
}

void CSacrificeState_OverDrive_Release_Attack01_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_OverDrive_Release_Attack01_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack02").Loop(false).Speed(1.2f).Apply();

	pOwner->OverDrive_Attack2();
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_OverDrive_Release_Attack02_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack03").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (IsCrossAnimProgress(0.25f))
		pOwner->OverDrive_Attack3();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->DeactiveSword();
		pOwner->SetOverDrive(false);
		pOwner->SetOverDriveCharged(false);
	}

	if (IsCrossAnimProgress(0.9f))
		pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::DISAPPEAR, 0.1f);

	pOwner->Update_Dissolve(dt);
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Exit(CSacrifice* pOwner)
{
}
