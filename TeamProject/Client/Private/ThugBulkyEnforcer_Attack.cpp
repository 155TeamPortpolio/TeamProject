#include "pch.h"
#include "ThugBulkyEnforcer_Attack.h"
#include "ThugBulkyEnforcer.h"
#include "Helper_Func.h"

#include "Animator3D.h" 

void CThugBulkyEnforcer_Attack::Enter(CThugBulkyEnforcer* pOwner)
{
	if (nullptr == m_pSubStateMachine) {
		m_pSubStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();

		Register_States();
		Register_Transitions();
	}

	ATTACK_BLACK_BOARD& blackboard = pOwner->GetBlackBoard();
	BuildPattern(blackboard);
	blackboard.isRequestNext = true;

	__super::Enter(pOwner);
}

void CThugBulkyEnforcer_Attack::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	
}

void CThugBulkyEnforcer_Attack::Exit(CThugBulkyEnforcer* pOwner)
{
}


void CThugBulkyEnforcer_Attack::Register_States()
{
	m_pSubStateMachine->Register_State("Attack01", CThugBulkyEnforcer_Attack1::Create());
	m_pSubStateMachine->Register_State("Attack02", CThugBulkyEnforcer_Attack2::Create());
	m_pSubStateMachine->Register_State("Attack03", CThugBulkyEnforcer_Attack3::Create());
	m_pSubStateMachine->Register_State("Attack04", CThugBulkyEnforcer_Attack4::Create());
	m_pSubStateMachine->Register_State("Attack05_01", CThugBulkyEnforcer_Attack5_1::Create());
	m_pSubStateMachine->Register_State("Attack05_02", CThugBulkyEnforcer_Attack5_2::Create());
}

void CThugBulkyEnforcer_Attack::Register_Transitions()
{
}

void CThugBulkyEnforcer_Attack::BuildPattern(ATTACK_BLACK_BOARD& blackBoard)
{
	//юс╫ц
	blackBoard.stateQueue.push_back("Attack01");
	blackBoard.stateQueue.push_back("Attack02");
	blackBoard.stateQueue.push_back("Attack03");
	blackBoard.stateQueue.push_back("Attack04");
	blackBoard.stateQueue.push_back("Attack05_01");
	blackBoard.stateQueue.push_back("Attack05_02");

	_uint iRandIndex = Helper::Get_Random_Int(0, 3);
	iRandIndex = 2;
	switch (iRandIndex)
	{
	case 0:
	{
		//blackBoard.stateQueue.push_back("Attack01_Phase1");
		//blackBoard.stateQueue.push_back("Attack02_Phase1");
		//blackBoard.stateQueue.push_back("Attack08_Phase1");
	}break;
	case 1:
	{
		
	}break;
	case 2:
	{
		

	}break;
	case 3:
	{	

	}break;
	case 4:
	{

	}break;
	default:
		break;
	}
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack1::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_01")
		.Apply();
}

void CThugBulkyEnforcer_Attack1::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Attack1::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack2::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_02")
		.Apply();
}

void CThugBulkyEnforcer_Attack2::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Attack2::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack3::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_03")
		.Apply();
}

void CThugBulkyEnforcer_Attack3::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Attack3::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack4::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_04")
		.Apply();
}

void CThugBulkyEnforcer_Attack4::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Attack4::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack5_1::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_05_01")
		.Apply();
}

void CThugBulkyEnforcer_Attack5_1::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Attack5_1::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Attack5_2::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Attack_05_02")
		.Apply();
}

void CThugBulkyEnforcer_Attack5_2::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Attack5_2::Exit(CThugBulkyEnforcer* pOwner)
{
}
