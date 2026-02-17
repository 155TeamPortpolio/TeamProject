#include "pch.h"
#include "DefilerState_Attack.h"
#include "Defiler.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "CharacterController.h"
#include "ObjectContainer.h"

#include "DefilerLaser.h"
#include "DisplayGate.h"

CDefilerState_Attack* CDefilerState_Attack::Create()
{
	CDefilerState_Attack* pInstance = new CDefilerState_Attack();
	pInstance->m_pSubStateMachine = CStateMachine<CDefiler>::Create();
	pInstance->ReadySubState();

	return pInstance;
}

void CDefilerState_Attack::Build_Pattern(CDefiler* pOwner, _int Type)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	TARGETING_INFO& targetInfo = pOwner->GetTargetingInfo();
	blackBoard.patternTransition.clear();
	Type = 10;
	switch (Type)
	{
	case 0 :
	{
		blackBoard.patternTransition.push_back({ "Attack01_01",0.f,0.41f });
		blackBoard.patternTransition.push_back({ "Attack01_02",0.19f,1.f });
		break;
	}
	case 1 :
	{
		blackBoard.patternTransition.push_back({ "Attack03",0.f,1.f });/*������*/
		break;
	}
	case 2 :
	{
		blackBoard.patternTransition.push_back({ "RePos_Back",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack06",0.f,1.f });/*������*/
		blackBoard.patternTransition.push_back({ "Attack03",0.f,1.f });/*������*/
		break;
	}
	case 3 :
	{
		blackBoard.patternTransition.push_back({ "Attack02",0.f,1.f });/*ȸ��*/
		blackBoard.patternTransition.push_back({ "Attack03",0.f,1.f });/*������*/
		break;
	}
	case 4 :
	{
		blackBoard.patternTransition.push_back({ "RePos_Back",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack05",0.f,1.f });/*����*/
		blackBoard.patternTransition.push_back({ "RePos_Target",0.f,1.f });/*����*/
		break;
	}
	case 5 :
	{
		blackBoard.patternTransition.push_back({ "RePos_Back",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack_Summon",0.f,1.f });/*������*/
		blackBoard.patternTransition.push_back({ "Attack_Grab",0.f,1.f });/*����*/
		blackBoard.patternTransition.push_back({ "RePos_Target",0.f,1.f });/*����*/
		break;
	}
	case 6 :
	{
		blackBoard.patternTransition.push_back({ "Attack01_03",0.f,1.f });/*����*/
		blackBoard.patternTransition.push_back({ "Attack04",0.f,1.f });/*������2*/
		break;
	}
	case 7 :
	{
		blackBoard.patternTransition.push_back({ "Attack01_01",0.f,1.f });
		break;
	}
	case 8 :
	{
		blackBoard.patternTransition.push_back({ "Attack01_01",0.f,0.41f });
		blackBoard.patternTransition.push_back({ "Attack01_02",0.19f,1.f });
		blackBoard.patternTransition.push_back({ "Attack04",0.f,1.f });/*������2*/
		break;
	}
	case 9 :
	{
		blackBoard.patternTransition.push_back({ "RePos_Back",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack08_01_Start",0.f,1.f }); /*���к�*/
		blackBoard.patternTransition.push_back({ "Attack08_01_Loop",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack08_01_End",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack08_02",0.f,1.f });
		break;
	}
	case 10 :
	{
		blackBoard.patternTransition.push_back({ "Attack_Barrier",0.f,1.f });
		blackBoard.patternTransition.push_back({ "RePos_Front",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack09_Start",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack09_Loop",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack09_End",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack01_01_P2",0.f,1.f });
		break;
	}
	case 11 :
	{
		blackBoard.patternTransition.push_back({ "RePos_Back",0.f,1.f });
		blackBoard.patternTransition.push_back({ "Attack07",0.f,1.f });/*�̾��*/
		blackBoard.patternTransition.push_back({ "Attack04",0.f,1.f });/*������*/
		break;
	}
	case 12 :
	{
		blackBoard.patternTransition.push_back({ "Attack08_02",0.f,1.f }); /*â ������*/
		break;
	}
	case 13 :
	{
		break;
	}
	default:
		break;
	}

	blackBoard.isRequestNext = true;
}

void CDefilerState_Attack::ReadySubState()
{
	m_pSubStateMachine->Register_State("Attack01_01",				CDefilerState_Attack_01_01::Create());
	m_pSubStateMachine->Register_State("Attack01_02",				CDefilerState_Attack_01_02::Create());
	m_pSubStateMachine->Register_State("Attack01_01_P2",			CDefilerState_Attack_01_01_P2::Create());
	m_pSubStateMachine->Register_State("Attack01_03",				CDefilerState_Attack_01_03::Create());
	m_pSubStateMachine->Register_State("Attack02",					CDefilerState_Attack_02::Create());
	m_pSubStateMachine->Register_State("Attack03",					CDefilerState_Attack_03::Create());
	m_pSubStateMachine->Register_State("Attack04",					CDefilerState_Attack_04::Create());
	m_pSubStateMachine->Register_State("Attack05",					CDefilerState_Attack_05::Create());
	m_pSubStateMachine->Register_State("Attack06",					CDefilerState_Attack_06::Create());
	m_pSubStateMachine->Register_State("Attack07",					CDefilerState_Attack_07::Create());
	
	m_pSubStateMachine->Register_State("Attack08_01_Start",			CDefilerState_Attack_08_01_Start::Create());
	m_pSubStateMachine->Register_State("Attack08_01_Loop",			CDefilerState_Attack_08_01_Loop::Create());
	m_pSubStateMachine->Register_State("Attack08_01_End",			CDefilerState_Attack_08_01_End::Create());
	m_pSubStateMachine->Register_State("Attack08_02",				CDefilerState_Attack_08_02::Create());
	m_pSubStateMachine->Register_State("Attack09_Start",			CDefilerState_Attack_09_Start::Create());
	m_pSubStateMachine->Register_State("Attack09_Loop",				CDefilerState_Attack_09_Loop::Create());
	m_pSubStateMachine->Register_State("Attack09_End",				CDefilerState_Attack_09_End::Create());
	m_pSubStateMachine->Register_State("Attack_Grab",				CDefilerState_Attack_Grab::Create());
	m_pSubStateMachine->Register_State("Attack_Summon",				CDefilerState_Attack_Summon::Create());
	m_pSubStateMachine->Register_State("RePos_Front",				CDefilerState_RePos_Front::Create());
	m_pSubStateMachine->Register_State("RePos_Back",				CDefilerState_RePos_Back::Create());
	m_pSubStateMachine->Register_State("RePos_Target",				CDefilerState_RePos_Target::Create());
	m_pSubStateMachine->Register_State("Attack_Barrier",			CDefilerState_Attack_Barrier::Create());
}

void CDefilerState_Attack::Enter(CDefiler* pOwner)
{
	__super::Enter(pOwner);
	auto& blackboard = pOwner->GetBlackBoard();
	Build_Pattern(pOwner, blackboard.patternIndex);
	blackboard.patternIndex++;
	if (blackboard.patternIndex > 12)
		blackboard.patternIndex = 0.f;

	if (!blackboard.patternTransition.empty())
	{
		blackboard.ReservePattern();
		blackboard.ResetNextFlag();
		m_pSubStateMachine->Change_State(blackboard.reservedPattern.nextPattern);
	}
}

void CDefilerState_Attack::Update(CDefiler* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
	auto& blackboard = pOwner->GetBlackBoard();

	if (blackboard.isRequestNext)
	{
		blackboard.isRequestNext = false;

		if (!blackboard.patternTransition.empty())
		{
			blackboard.ReservePattern();
			m_pSubStateMachine->Change_State(blackboard.reservedPattern.nextPattern);
		}
		else
		{
			pOwner->Get_MainStateMachine()->Set_Trigger("Idle");
		}
	}

	Update_Effects(pOwner);
}

void CDefilerState_Attack::ComboTransition(CDefiler* pOwner)
{
	auto& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= blackBoard.reservedPattern.animEndProgress)
		blackBoard.isRequestNext = true;
}

void CDefilerState_Attack::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_01::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_01")
		.Speed(1.f)
		.UseFinalLocalPose(true)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_01_01::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
	Update_Effects(pOwner);
}

void CDefilerState_Attack_01_01::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_01::Update_Effects(CDefiler* pOwner)
{
	if (IsCrossAnimProgress(0.05f))
		pOwner->Play_Effect("Defiler_Axe_Light0", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
	if (IsCrossAnimProgress(0.19f))
		pOwner->Play_Effect("Defiler_Slash0_0", _vector3(0.f, 1.1f, 0.f), _quaternion(0.72f, 0.14f, 0.11f, 0.67f));
	if (IsCrossAnimProgress(0.3f))
		pOwner->Play_Effect("Defiler_Slash0_1", _vector3(0.f, 2.5f, 0.f), _quaternion(-0.19f, 0.6f, 0.62f, -0.47f));
}

void CDefilerState_Attack_01_02::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_02")
		.Speed(1.f)
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Loop(false)
		.Apply();

	pOwner->ChainParry(true);
}

void CDefilerState_Attack_01_02::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
	Update_Effects(pOwner);
}

void CDefilerState_Attack_01_02::Exit(CDefiler* pOwner)
{
	pOwner->ChainParry(false);
}

void CDefilerState_Attack_01_02::Update_Effects(CDefiler* pOwner)
{
	if (IsCrossAnimProgress(0.34f))
		pOwner->Play_Effect("Defiler_Slash1_0", _vector3(0.f, 0.8f, 0.f), _quaternion(-0.08f, 0.68f, 0.71f, 0.18f));
}

void CDefilerState_Attack_01_01_P2::Enter(CDefiler* pOwner)
{

	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_01_P2")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();

	pOwner->ChainParry(true);
}

void CDefilerState_Attack_01_01_P2::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
	Update_Effects(pOwner);
}

void CDefilerState_Attack_01_01_P2::Exit(CDefiler* pOwner)
{
	pOwner->ChainParry(false);
}

void CDefilerState_Attack_01_01_P2::Update_Effects(CDefiler* pOwner)
{
	if (IsCrossAnimProgress(0.13f))
		pOwner->Play_Effect("Defiler_Slash0_0", _vector3(0.f, 1.5f, 0.f), _quaternion(0.66f, 0.3f, -0.22f, 0.66f));
	if (IsCrossAnimProgress(0.18f))
		pOwner->Play_Effect("Defiler_Slash0_1", _vector3(0.f, 1.4f, 0.f), _quaternion(0.08f, 0.71f, 0.7f, 0.04f));
	if (IsCrossAnimProgress(0.27f))
		pOwner->Play_Effect("Defiler_Slash1_0", _vector3(0.f, 1.4f, 0.f), _quaternion(-0.1f, 0.71f, -0.53f, 0.46f));
	if (IsCrossAnimProgress(0.35f))
		pOwner->Play_Effect("Defiler_Slash1_1", _vector3(0.f, 1.4f, 0.f), _quaternion(0.56f, -0.47f, 0.03f, -0.69f));
	if (IsCrossAnimProgress(0.44f))
		pOwner->Play_Effect("Defiler_Slash1_0", _vector3(0.f, 0.8f, 0.f), _quaternion(-0.28f, 0.62f, 0.64f, 0.36f));
	if (IsCrossAnimProgress(0.57f))
		pOwner->Play_Effect("Defiler_Axe_Light0", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
	if (IsCrossAnimProgress(0.62f))
	{
		pOwner->Play_Effect("Defiler_Slash2_0", _vector3(0.f, 2.1f, 0.7f), _quaternion(-0.2f, 0.69f, -0.2f, 0.66f));
		pOwner->Play_Effect("Defiler_HitGround0", _vector3(0.2f, 0.2f, 2.9f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
	}
}

void CDefilerState_Attack_01_03::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_01_03")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_01_03::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
	Update_Effects(pOwner);
}

void CDefilerState_Attack_01_03::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_01_03::Update_Effects(CDefiler* pOwner)
{
	if (IsCrossAnimProgress(0.15f))
		pOwner->Play_Effect("Defiler_Slash1_0", _vector3(0.f, 1.f, 0.f), _quaternion(-0.01f, 0.69f, 0.72f, 0.1f));
	if (IsCrossAnimProgress(0.21f))
		pOwner->Play_Effect("Defiler_Axe_Slash0_0", _vector3(0.f, 0.f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
	if (IsCrossAnimProgress(0.36f))
		pOwner->Play_Effect("Defiler_Tail_Slash0_0", _vector3(-2.1f, 0.6f, 1.2f), _quaternion(0.52f, -0.48f, 0.5f, -0.5f));
	if (IsCrossAnimProgress(0.51f))
		pOwner->Play_Effect("Defiler_Axe_Light0", _vector3(0.f, 0.f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
	if (IsCrossAnimProgress(0.56f))
		pOwner->Play_Effect("Defiler_Slash2_0", _vector3(0.4f, 1.5f, 1.9f), _quaternion(-0.22f, 0.54f, -0.31f, 0.75f));
	if (IsCrossAnimProgress(0.565f))
		pOwner->Play_Effect("Defiler_HitGround0", _vector3(0.f, 0.2f, 2.6f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
}

void CDefilerState_Attack_02::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_02")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();

	m_iSlashCount = 0;
}

void CDefilerState_Attack_02::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
	Update_Effects(pOwner);
}

void CDefilerState_Attack_02::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_02::Update_Effects(CDefiler* pOwner)
{
	if (IsCrossAnimProgress(0.12f))
		pOwner->Play_Effect("Defiler_Slash0_0", _vector3(0.f, 1.6f, 0.f), _quaternion(0.72f, 0.17f, -0.11f, 0.66f));

	if (IsCrossAnimProgress(0.23f))
		pOwner->Play_Effect("Defiler_Axe_Spin", _vector3(0.f, 3.5f, 0.f), _quaternion(0.71f, 0.f, 0.f, -0.71f));
	if (IsCrossAnimProgress(0.46f))
		pOwner->Stop_Effect("Defiler_Axe_Spin");

	if (IsCrossAnimProgress(0.51f))
		pOwner->Play_Effect("Defiler_Slash1_0", _vector3(0.f, 1.4f, 0.f), _quaternion(-0.13f, 0.64f, 0.73f, 0.2f));

	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	const auto& events = pAnimator->Get_EventBus();
	for (const auto& event : events)
	{
		if (event.Tag == "I_Type_On")
		{
			_float3 randPosition0{}, randPosition1{};
			_float3 randAngle{};

			randPosition0.y = Helper::Get_Random_Float(1.3f, 2.3f);
			randPosition1.y = Helper::Get_Random_Float(1.3f, 2.3f);

			randAngle.x = XMConvertToRadians(Helper::Get_Random_Float(-10.f, 10.f));
			randAngle.y = XMConvertToRadians(Helper::Get_Random_Float(-5.f, 5.f));
			randAngle.z = XMConvertToRadians(Helper::Get_Random_Float(-5.f, 5.f));

			_quaternion rotation = m_BaseRotation;
			rotation *= _quaternion::CreateFromYawPitchRoll(randAngle);

			pOwner->Play_Effect("Defiler_Slash3_" + to_string(m_iSlashCount % 5), _vector3(randPosition0), rotation);
			++m_iSlashCount;

			rotation *= _quaternion::CreateFromYawPitchRoll(XMConvertToRadians(120.f), 0.f, 0.f);
			pOwner->Play_Effect("Defiler_Slash3_" + to_string(m_iSlashCount % 5), _vector3(randPosition1), rotation);
			++m_iSlashCount;
		}
	}
}

void CDefilerState_Attack_03::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_03")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_03::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
	Update_Effects(pOwner);
}

void CDefilerState_Attack_03::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_03::Update_Effects(CDefiler* pOwner)
{
	if (IsCrossAnimProgress(0.01f))
		pOwner->Play_Effect("Defiler_Axe_Charge0", _vector3(0.f, 0.f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
	if (IsCrossAnimProgress(0.16f))
		pOwner->Stop_Effect("Defiler_Axe_Charge0");
	if (IsCrossAnimProgress(0.2f))
		pOwner->Play_Effect("Defiler_Axe_Explode0", _vector3(0.f, 0.f, 0.f), _quaternion(0.f, 0.f, 0.f, 1.f), false);

	if (IsCrossAnimProgress(0.3f))
		pOwner->Play_Effect("Defiler_Slash2_0", _vector3(0.f, 2.9f, 1.6f), _quaternion(-0.31f, 0.67f, -0.15f, 0.66f));
	if (IsCrossAnimProgress(0.31f))
		pOwner->Play_Effect("Defiler_HitGround0", _vector3(0.f, 0.2f, 3.7f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
}

void CDefilerState_Attack_04::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_04")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_04::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
	Update_Effects(pOwner);
}

void CDefilerState_Attack_04::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_04::Update_Effects(CDefiler* pOwner)
{
	if (IsCrossAnimProgress(0.05f))
		pOwner->Play_Effect("Defiler_Axe_Light1", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
	if (IsCrossAnimProgress(0.31f))
		pOwner->Play_Effect("Defiler_Axe_Explode0", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);

	if (IsCrossAnimProgress(0.42f))
		pOwner->Play_Effect("Defiler_Slash2_0", _vector3(0.6f, 1.7f, 0.f), _quaternion(-0.27f, 0.64f, -0.15f, 0.71f));
	if (IsCrossAnimProgress(0.42f))
		pOwner->Play_Effect("Defiler_HitGround0", _vector3(0.8f, 0.2f, 3.1f), _quaternion(0.f, 0.f, 0.f, 1.f), false);
}

void CDefilerState_Attack_05::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_Fierce();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_05")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();

	pOwner->Change_CollisionMask();
}

void CDefilerState_Attack_05::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
	Update_Effects(pOwner);
}

void CDefilerState_Attack_05::Exit(CDefiler* pOwner)
{
	pOwner->Release_CollisionMask();
}

void CDefilerState_Attack_05::Update_Effects(CDefiler* pOwner)
{
	if (IsCrossAnimProgress(0.08f))
		pOwner->Play_Effect("Defiler_Axe_Light2", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
	if (IsCrossAnimProgress(0.3f))
		pOwner->Stop_Effect("Defiler_Axe_Light2");
	if (IsCrossAnimProgress(0.3f))
		pOwner->Play_Effect("Defiler_Dash_Trail", _vector3(-0.2f, 2.5f, -4.7f), _quaternion(0.71f, 0.f, 0.f, 0.71f));
}

void CDefilerState_Attack_06::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_06")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_06::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
	Update_Effects(pOwner);
}

void CDefilerState_Attack_06::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_06::Update_Effects(CDefiler* pOwner)
{
	if (IsCrossAnimProgress(0.12f))
		pOwner->Play_Effect("Defiler_Laser_Charge_Normal", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
	if (IsCrossAnimProgress(0.16f))
		pOwner->Stop_Effect("Defiler_Laser_Charge_Normal");

	if (IsCrossAnimProgress(0.46f))
		pOwner->Play_Effect("Defiler_Laser_Charge_Strong", _vector3(), _quaternion(0.f, 0.f, 0.f, 1.f), false);
	if (IsCrossAnimProgress(0.56f))
		pOwner->Stop_Effect("Defiler_Laser_Charge_Strong");


	if (IsCrossAnimProgress(0.18f))
	{
		auto pLaser = pOwner->Get_Component<CObjectContainer>()->Find_ObjectByName("Defiler_Laser_0");
		static_cast<CDefilerLaser*>(pLaser)->Set_ActiveLaser(true, CDefilerLaser::LASER_TYPE::NORMAL);
	}

	if (IsCrossAnimProgress(0.35f))
	{
		auto pLaser = pOwner->Get_Component<CObjectContainer>()->Find_ObjectByName("Defiler_Laser_1");
		static_cast<CDefilerLaser*>(pLaser)->Set_ActiveLaser(true, CDefilerLaser::LASER_TYPE::NORMAL);
	}
	
	if (IsCrossAnimProgress(0.57f))
	{
		auto pLaser = pOwner->Get_Component<CObjectContainer>()->Find_ObjectByName("Defiler_Laser_2");
		static_cast<CDefilerLaser*>(pLaser)->Set_ActiveLaser(true, CDefilerLaser::LASER_TYPE::STRONG);
	}
}

void CDefilerState_Attack_07::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	blackBoard.TraceType_IgnoreRotation();

	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_07")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();
	pOwner->ChainParry(true);
}

void CDefilerState_Attack_07::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_07::Exit(CDefiler* pOwner)
{
	pOwner->ChainParry(false);
}

void CDefilerState_Attack_08_01_Start::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	blackBoard.TraceType_IgnoreRotation();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_01_Start")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();
	pOwner->Control_TargetEnable(false);
}

void CDefilerState_Attack_08_01_Start::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_08_01_Start::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_08_01_Loop::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_01_Loop")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(true)
		.Apply();
	pOwner->ChainParry(true);
}

void CDefilerState_Attack_08_01_Loop::Update(CDefiler* pOwner, _float dt)
{
	m_Elapsed += dt;
	m_Interval += dt;
	if (m_Duration < m_Elapsed) {
		auto& blackBoard = pOwner->GetBlackBoard();
		blackBoard.isRequestNext = true;
	}

	if (m_Interval >= 0.6f) {
		pOwner->Control_Summon("Heavy");
		m_Interval = 0.f;
	}
}

void CDefilerState_Attack_08_01_Loop::Exit(CDefiler* pOwner)
{
	m_Interval = 0.f;
	m_Elapsed = 0.f;
	pOwner->ChainParry(false);
}

void CDefilerState_Attack_08_01_End::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_01_End")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_08_01_End::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_08_01_End::Exit(CDefiler* pOwner)
{
	pOwner->Control_TargetEnable(true);
}

void CDefilerState_Attack_08_02::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	blackBoard.TraceType_IgnoreRotation();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_08_02")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();

}

void CDefilerState_Attack_08_02::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_08_02::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_Start::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();


	pOwner->Control_TargetEnable(false);

	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	blackBoard.TraceType_OnlyAnim();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_09_Start")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();

}

void CDefilerState_Attack_09_Start::Update(CDefiler* pOwner, _float dt)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.vTargetDir = { 1,0,0 };

	ComboTransition(pOwner);
}

void CDefilerState_Attack_09_Start::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_09_Loop::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	blackBoard.TraceType_IgnoreRotation();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_09_Loop")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(true)
		.Apply();
}

void CDefilerState_Attack_09_Loop::Update(CDefiler* pOwner, _float dt)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	m_ElapsedTime += dt;
	if(m_ElapsedTime > 15.f)
		blackBoard.isRequestNext = true;
}

void CDefilerState_Attack_09_Loop::Exit(CDefiler* pOwner)
{
	m_ElapsedTime = 0.f;
}

void CDefilerState_Attack_09_End::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	blackBoard.TraceType_OnTarget();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_09_End")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();
	pOwner->GetDissolve().DisAppear(.5f);
	pOwner->HideHUD(true);
}

void CDefilerState_Attack_09_End::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_09_End::Exit(CDefiler* pOwner)
{
	pOwner->Control_TargetEnable(true);
	pOwner->GetDissolve().Appear(0.4f);
	pOwner->HideHUD(false);
}

void CDefilerState_Attack_Grab::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_Fierce();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_Grab_01")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_Grab::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_Grab::Exit(CDefiler* pOwner)
{
}

void CDefilerState_Attack_Summon::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	blackBoard.TraceType_IgnoreRotation();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_Attack_Summon")
		.StartAt(blackBoard.reservedPattern.animStartProgress)
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_Summon::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_Summon::Exit(CDefiler* pOwner)
{
}

void CDefilerState_RePos_Front::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	auto& dissolve = pOwner->GetDissolve();
	dissolve.Set_DissolveState(dissolve.DISAPPEAR, .5f);
	m_eState = EVADE_IN;
	pOwner->Control_TargetEnable(false);
}

void CDefilerState_RePos_Front::Update(CDefiler* pOwner, _float dt)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto& dissolve = pOwner->GetDissolve();
	if (m_eState == EVADE_IN && dissolve.isComplete()) {
		m_eState = EVADE_OUT;
		dissolve.Set_DissolveState(dissolve.APPEAR, .5f);
		pOwner->Set_CCTPos({ -15.f,-1.f,0.f});
	}
	else if (m_eState == EVADE_OUT && dissolve.isComplete()) {
		blackBoard.isRequestNext = true;
	}
}

void CDefilerState_RePos_Front::Exit(CDefiler* pOwner)
{
	pOwner->Control_TargetEnable(true);
}

void CDefilerState_RePos_Back::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	auto& dissolve = pOwner->GetDissolve();
	dissolve.Set_DissolveState(dissolve.DISAPPEAR, .5f);
	m_eState = EVADE_IN;
	pOwner->Control_TargetEnable(false);
}

void CDefilerState_RePos_Back::Update(CDefiler* pOwner, _float dt)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto& dissolve = pOwner->GetDissolve();
	
	if (m_eState == EVADE_IN && dissolve.isComplete()) {
		m_eState = EVADE_OUT;
		dissolve.Set_DissolveState(dissolve.APPEAR, .5f);
		pOwner->Set_CCTPos({ 15.f,-2.f, -5.f });
	}
	else if (m_eState == EVADE_OUT && dissolve.isComplete()) {
		blackBoard.isRequestNext = true;
	}
}

void CDefilerState_RePos_Back::Exit(CDefiler* pOwner)
{
	pOwner->Control_TargetEnable(true);
}

void CDefilerState_RePos_Target::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnTarget();
	auto& dissolve = pOwner->GetDissolve();
	dissolve.Set_DissolveState(dissolve.DISAPPEAR, .5f);
	m_eState = EVADE_IN;
	pOwner->Control_TargetEnable(false);
}

void CDefilerState_RePos_Target::Update(CDefiler* pOwner, _float dt)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto& targeting = pOwner->GetTargetingInfo();
	auto& dissolve = pOwner->GetDissolve();

	if (m_eState == EVADE_IN && dissolve.isComplete()) {
		m_eState = EVADE_OUT;
		dissolve.Set_DissolveState(dissolve.APPEAR, .5f);
		pOwner->Set_CCTPos({ (targeting.vTargetPos - targeting.vDirToTarget) });
	}
	else if (m_eState == EVADE_OUT && dissolve.isComplete()) {
		blackBoard.isRequestNext = true;
	}
}

void CDefilerState_RePos_Target::Exit(CDefiler* pOwner)
{
	pOwner->Control_TargetEnable(true);
}

void CDefilerState_Attack_Barrier::Enter(CDefiler* pOwner)
{
	DEFILER_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.TraceType_OnlyAnim();
	blackBoard.TraceType_IgnoreRotation();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_IsoldetheDefiler_Ani_EtherBarrier")
		.Speed(1.f)
		.Loop(false)
		.Apply();
}

void CDefilerState_Attack_Barrier::Update(CDefiler* pOwner, _float dt)
{
	ComboTransition(pOwner);
}

void CDefilerState_Attack_Barrier::Exit(CDefiler* pOwner)
{

}
