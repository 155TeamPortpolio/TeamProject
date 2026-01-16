#include "pch.h"
#include "SacrificeState_Attack_Phase2.h"
#include "Sacrifice.h"
#include "Helper_Func.h"
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

	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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
		if (0 == iRandIndex || blackBoard.currentStateTag == "OverDrive_Loop")
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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

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
			blackBoard.stateQueue.push_back("Attack_Charge_Start_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_Loop_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_U_Start_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_U_Loop_Phase2");
			blackBoard.stateQueue.push_back("Attack_Charge_U_End_Phase2");
		}break;
		case 2:
		{
			blackBoard.stateQueue.push_back("Attack04_Phase2");
		}break;
		case 3:
		{
			blackBoard.stateQueue.push_back("Attack03_Phase2");
		}break;
		case 4:
		{
			blackBoard.stateQueue.push_back("Attack08_Phase2");
		}break;
		case 5:
		{
			blackBoard.stateQueue.push_back("Attack05_1_Phase2");
			blackBoard.stateQueue.push_back("Attack05_Phase2");
		}break;
		default:
			break;
		}
	}

	blackBoard.stateQueue.clear();
	blackBoard.stateQueue.push_back("Attack_Charge_Start_Phase2");
	blackBoard.stateQueue.push_back("Attack_Charge_Loop_Phase2");
	blackBoard.stateQueue.push_back("Attack_Charge_U_Start_Phase2");
	blackBoard.stateQueue.push_back("Attack_Charge_U_Loop_Phase2");
	blackBoard.stateQueue.push_back("Attack_Charge_U_End_Phase2");

	blackBoard.isRequestNext = true;
}

void CSacrificeState_Attack_01_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack_01").Loop(false).Speed(1.2f).Apply();

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

	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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

	pOwner->Active_AttackSign();
	pOwner->ActiveWhip();
}

void CSacrificeState_Attack_02_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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

	pOwner->Active_AttackSign();
	pOwner->ActiveWhip();
}

void CSacrificeState_Attack_03_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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
	pAnimator->Change_Animation("SacrificeBringer_Ani_P2_Attack04").Loop(false).Speed(1.2f).Apply();

	m_IsHandSpawn = false;
}

void CSacrificeState_Attack_04_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

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
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Attack_05_1").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_05_1_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (!m_IsAttackStart && m_fAnimProgress >= 0.1f)
	{
		pOwner->ActiveAxe();
		m_IsAttackStart = true;
	}
	
	if (!m_IsAttackEnd && m_fAnimProgress >= 0.85f)
	{
		pOwner->DeactiveAxe();
		m_IsAttackEnd = true;
	}

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (m_fAnimProgress < 0.5f)
		pOwner->RotateToTarget(dt, 10.f);

	auto pCCT = pOwner->Get_Component<CCharacterController>();
	if (!m_IsJumpStart && m_fAnimProgress >= 0.2f)
	{
		pOwner->Active_AttackSign();

		_vector3 vCurrDir = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
		_vector3 vTargetPos = pOwner->GetTargetingInfo().vTargetPos;
		m_vSecondTargetPosition = vTargetPos - vCurrDir * 2.f;
		m_IsJumpStart = true;
	}

	if (m_IsAttackStart)
	{
		if (m_fAnimProgress < 0.2f)
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

void CSacrificeState_Attack_08_Phase2::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Charge_Start_Phase2::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringer_Ani_P2_Charge_Start").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_Charge_Start_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

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
}

void CSacrificeState_Attack_Charge_U_Start_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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

	m_IsHandSpawn = false;
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (!m_IsHandSpawn && m_fAnimProgress >= 0.25f)
	{
		pOwner->OverDrive_Attack3();
		m_IsHandSpawn = true;
	}


	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->DeactiveSword();
		pOwner->SetOverDrive(false);
		pOwner->SetOverDriveCharged(false);
	}
}

void CSacrificeState_OverDrive_Release_Attack03_Phase2::Exit(CSacrifice* pOwner)
{
}
