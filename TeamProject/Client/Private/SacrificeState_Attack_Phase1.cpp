#include "pch.h"
#include "SacrificeState_Attack_Phase1.h"
#include "Sacrifice.h"
#include "CharacterController.h"
#include "ObjectContainer.h"
#include "Helper_Func.h"
#include "SacrificeHand.h"
#include "GameInstance.h"
#include "EffectContainer.h"

void CSacrificeState_Attack_Phase1::Enter(CSacrifice* pOwner)
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

void CSacrificeState_Attack_Phase1::Update(CSacrifice* pOwner, _float dt)
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
		iRandIndex = 1;
		if (0 == iRandIndex)
			pOwner->Idle();
		else
			pOwner->Evade();
	}
}

void CSacrificeState_Attack_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Phase1::Register_States()
{
	m_pSubStateMachine->Register_State("Arm_Recover", CSacrificeState_ArmRecover_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack01_Phase1", CSacrificeState_Attack_01_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack02_Phase1", CSacrificeState_Attack_02_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack03_Phase1", CSacrificeState_Attack_03_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack04_1_Phase1", CSacrificeState_Attack_04_1_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack04_2_Phase1", CSacrificeState_Attack_04_2_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack05_Phase1", CSacrificeState_Attack_05_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack06_Phase1", CSacrificeState_Attack_06_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack07_Phase1", CSacrificeState_Attack_07_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack08_Phase1", CSacrificeState_Attack_08_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack09_Phase1", CSacrificeState_Attack_09_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack10_Phase1", CSacrificeState_Attack_10_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack11_Phase1", CSacrificeState_Attack_11_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack12_Phase1", CSacrificeState_Attack_12_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack_Turn_Phase1", CSacrificeState_Attack_Turn_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack_Roar_Phase1", CSacrificeState_Attack_Roar_Phase1::Create());
}

void CSacrificeState_Attack_Phase1::Register_Transitions()
{
}

void CSacrificeState_Attack_Phase1::BuildPattern(CSacrifice* pOwner)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	TARGETING_INFO& targetInfo = pOwner->GetTargetingInfo();

	blackBoard.stateQueue.clear();

	if (targetInfo.fDistance < 5.f)
	{
		_vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
		_vector3 vTargetDir = targetInfo.vDirToTarget;
		if (vLook.Dot(vTargetDir) < 0.f)
		{
			blackBoard.stateQueue.push_back("Attack_Turn_Phase1");
			blackBoard.stateQueue.push_back("Attack02_Phase1");
		}
		else
		{
			_uint iRandIndex = Helper::Get_Random_Int(0, 3);
			//iRandIndex = 1;
			switch (iRandIndex)
			{
			case 0:
			{
				blackBoard.stateQueue.push_back("Attack01_Phase1");
				blackBoard.stateQueue.push_back("Attack02_Phase1");
				blackBoard.stateQueue.push_back("Attack08_Phase1");
			}break;
			case 1:
			{
				/* Hand Pattern */
				blackBoard.stateQueue.push_back("Attack10_Phase1");
				blackBoard.stateQueue.push_back("Attack11_Phase1");
				blackBoard.stateQueue.push_back("Attack12_Phase1");
			}break;
			case 2:
			{
				blackBoard.stateQueue.push_back("Attack05_Phase1");
			
			}break;
			case 3:
			{	
				blackBoard.stateQueue.push_back("Attack_Roar_Phase1");
				blackBoard.stateQueue.push_back("Attack06_Phase1");
			}break;
			default:
				break;
			}
		}
	}
	else
	{
		_uint iRandIndex = Helper::Get_Random_Int(0, 2);
		switch (iRandIndex)
		{
		case 0:
		{
			blackBoard.stateQueue.push_back("Attack07_Phase1");
			blackBoard.stateQueue.push_back("Attack02_Phase1");
			blackBoard.stateQueue.push_back("Attack08_Phase1");
		}break;
		case 1:
		{
			blackBoard.stateQueue.push_back("Attack07_Phase1");
			blackBoard.stateQueue.push_back("Attack02_Phase1");
			blackBoard.stateQueue.push_back("Attack03_Phase1");
			blackBoard.stateQueue.push_back("Arm_Recover");
		}break;
		case 2:
		{
			blackBoard.stateQueue.push_back("Attack04_1_Phase1");
			blackBoard.stateQueue.push_back("Attack02_Phase1");
		}break;
		default:
			break;
		}
	}

	blackBoard.stateQueue.clear();
	blackBoard.stateQueue.push_back("Attack07_Phase1");
	blackBoard.stateQueue.push_back("Attack02_Phase1");
	blackBoard.stateQueue.push_back("Attack08_Phase1");

	blackBoard.isRequestNext = true;
}

void CSacrificeState_ArmRecover_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Arm_Recover").Loop(false).Speed(1.4f).Apply();

	pAnimator->Get_EventBus();
}

void CSacrificeState_ArmRecover_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);

	if (m_fAnimProgress >= 0.6f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_ArmRecover_Phase1::Exit(CSacrifice* pOwner)
{
}


void CSacrificeState_Attack_01_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_01").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_01_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);

	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.3f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Attack_01_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_02_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_02").Loop(false).TransitionSpeed(1.f, 1.4f, 3.f, EaseType::OutQuint).Apply();
	
	pOwner->Active_AttackSign();

	m_IsAttackStart = false;
	m_IsAttackEnd = false;
	m_fAnimProgress = 0.f;
}

void CSacrificeState_Attack_02_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.2f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (!m_IsAttackStart && m_fAnimProgress >= 0.05f)
	{
		m_IsAttackStart = true;
		pOwner->ActiveSword();
	}

	if (!m_IsAttackEnd && m_fAnimProgress >= 0.15f)
	{
		m_IsAttackEnd = true;
		pOwner->DeactiveSword();
	}

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_02_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_03_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_03").Loop(false).Speed(1.2f).Apply();

	m_IsSpawnEffect = false;
}

void CSacrificeState_Attack_03_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.3f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (!m_IsSpawnEffect && m_fAnimProgress >= 0.18f)
	{
		auto pAnimator = pOwner->Get_Component<CAnimator3D>();
		auto pTransform = pOwner->Get_Component<CTransform>();

		_vector3 vBonePosition = pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, "Skn_Finger2_03");
		vBonePosition = _vector3::Transform(vBonePosition, pTransform->Get_WorldMatrix());

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("hit_ground_smoke.json")
			.Position(vBonePosition)
			.Build("Smoke");
		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(effect, { "Test_Level","Effect_Layer" });

		m_IsSpawnEffect = true;
	}

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_03_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_04_1_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_04_1").Loop(false).Speed(1.2f).Apply();

	pOwner->Active_AttackSign();
}

void CSacrificeState_Attack_04_1_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.25f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (m_fAnimProgress < 0.1f)
		pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_04_1_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_04_2_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_04_2").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_04_2_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_04_2_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_05").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_05_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_05_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_06_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_06").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_06_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.2f && m_fAnimProgress <= 0.65f)
	{
		auto pTransform = pOwner->Get_Component<CTransform>();
		_quaternion vRot = _quaternion::CreateFromYawPitchRoll(XMConvertToRadians(180.f) * dt, 0.f, 0.f);
		pTransform->Add_Quaternion(vRot);
	}

	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_06_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_07_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_07").Loop(false).Speed(1.4f).Apply();

	pOwner->Active_AttackSign();

	m_IsAttackStart = false;
	m_IsAttackEnd = false;
	m_fAnimProgress = 0.f;
}

void CSacrificeState_Attack_07_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.7f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (!m_IsAttackStart && m_fAnimProgress >= 0.05f)
	{
		m_IsAttackStart = true;
		pOwner->ActiveSword();
	}

	if (!m_IsAttackEnd && m_fAnimProgress >= 0.6f)
	{
		m_IsAttackEnd = true;
		pOwner->DeactiveSword();
	}

	if (m_fAnimProgress < 0.3f)
		pOwner->RotateToTarget(dt, 20.f);
	pOwner->MoveByRootMotion(dt, 1.2f);
}

void CSacrificeState_Attack_07_Phase1::Exit(CSacrifice* pOwner)
{
	static_cast<CSacrifice*>(pOwner)->DeactiveSword();
}

void CSacrificeState_Attack_08_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_08").Loop(false).Speed(1.2f).Apply();

	pOwner->Active_AttackSign();

	m_IsAttackStart = false;
	m_IsAttackEnd = false;
	m_IsJumpStart = false;	

	_vector3 vCurrDir = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
	_vector3 vTargetPos = pOwner->GetTargetingInfo().vTargetPos;
	m_vFirstTargetPosition = vTargetPos - vCurrDir * 2.f;
}

void CSacrificeState_Attack_08_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (!m_IsAttackStart && m_fAnimProgress >= 0.06f)
	{
		m_IsAttackStart = true;
		pOwner->ActiveAxe();
	}

	if (!m_IsAttackEnd && m_fAnimProgress >= 0.6f)
	{
		m_IsAttackEnd = true;
		pOwner->DeactiveAxe();
	}

	/* Move Update */
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

void CSacrificeState_Attack_08_Phase1::Exit(CSacrifice* pOwner)
{
	
}

void CSacrificeState_Attack_09_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_09").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_09_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_09_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_10_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_10").Loop(false).Speed(1.4f).Apply();

	pOwner->Phase1Attack();
}

void CSacrificeState_Attack_10_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.2f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->RotateToTarget(dt, 10.f);
}

void CSacrificeState_Attack_10_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_11_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_11").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_11_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.3f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->RotateToTarget(dt, 10.f);
}

void CSacrificeState_Attack_11_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_12_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_12").Loop(false).Speed(1.4f).Apply();
}

void CSacrificeState_Attack_12_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.5f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->RotateToTarget(dt, 10.f);
}

void CSacrificeState_Attack_12_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Turn_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_Turn").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_Turn_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.6f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
	
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	auto pTransform = pOwner->Get_Component<CTransform>();

	_quaternion vDeltaQuaternion = pAnimator->Get_RootBoneQuatDelta();
	pTransform->Add_Quaternion(vDeltaQuaternion);
}

void CSacrificeState_Attack_Turn_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Roar_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Stun_Roar").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_Roar_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_Attack_Roar_Phase1::Exit(CSacrifice* pOwner)
{
}
