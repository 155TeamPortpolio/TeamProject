#include "pch.h"
#include "ThugBulkyEnforcer_Move.h"
#include "ThugBulkyEnforcer.h"
#include "Helper_Func.h"

#include "Animator3D.h" 

void CThugBulkyEnforcer_Move::Enter(CThugBulkyEnforcer* pOwner)
{
}

void CThugBulkyEnforcer_Move::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Move::Exit(CThugBulkyEnforcer* pOwner)
{
}

void CThugBulkyEnforcer_Move::Register_States()
{
	m_pSubStateMachine->Register_State("Walk_Front", CThugBulkyEnforcer_Walk_Front::Create());
	m_pSubStateMachine->Register_State("Walk_Left", CThugBulkyEnforcer_Walk_Left::Create());
	m_pSubStateMachine->Register_State("Walk_Right", CThugBulkyEnforcer_Walk_Right::Create());
	m_pSubStateMachine->Register_State("Walk_Back", CThugBulkyEnforcer_Walk_Back::Create());
	m_pSubStateMachine->Register_State("Run_Start", CThugBulkyEnforcer_Run_Start::Create());
	m_pSubStateMachine->Register_State("Run", CThugBulkyEnforcer_Run::Create());
	m_pSubStateMachine->Register_State("Run_End", CThugBulkyEnforcer_Run_End::Create());
	m_pSubStateMachine->Register_State("Run_End", CThugBulkyEnforcer_SideStep_L::Create());
	m_pSubStateMachine->Register_State("Run_End", CThugBulkyEnforcer_SideStep_R::Create());
	m_pSubStateMachine->Register_State("Run_End", CThugBulkyEnforcer_Evade::Create());
	m_pSubStateMachine->Register_State("Walk_RF_LFoot", CThugBulkyEnforcer_Walk_RF_LFoot::Create());
	m_pSubStateMachine->Register_State("Walk_FR_RFoot", CThugBulkyEnforcer_Walk_FR_RFoot::Create());
	m_pSubStateMachine->Register_State("Walk_LF_RFoot", CThugBulkyEnforcer_Walk_FL_RFoot::Create());
}

void CThugBulkyEnforcer_Move::Register_Transitions()
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_Front::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_F")
		.Apply();
}

void CThugBulkyEnforcer_Walk_Front::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Walk_Front::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_Left::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_L")
		.Apply();
}

void CThugBulkyEnforcer_Walk_Left::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Walk_Left::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_Right::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_R")
		.Apply();
}

void CThugBulkyEnforcer_Walk_Right::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Walk_Right::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_Back::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_B")
		.Apply();
}

void CThugBulkyEnforcer_Walk_Back::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Walk_Back::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Run_Start::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Run_Start")
		.Apply();
}

void CThugBulkyEnforcer_Run_Start::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Run_Start::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Run::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Run")
		.Apply();
}

void CThugBulkyEnforcer_Run::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Run::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Run_End::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Run_End")
		.Apply();
}

void CThugBulkyEnforcer_Run_End::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Run_End::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_SideStep_L::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_SideStep_L")
		.Apply();
}

void CThugBulkyEnforcer_SideStep_L::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_SideStep_L::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_SideStep_R::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_SideStep_R")
		.Apply();
}

void CThugBulkyEnforcer_SideStep_R::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_SideStep_R::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Evade::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Evade")
		.Apply(); 
}

void CThugBulkyEnforcer_Evade::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Evade::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_RF_LFoot::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_RF_LFoot")
		.Apply();
}

void CThugBulkyEnforcer_Walk_RF_LFoot::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Walk_RF_LFoot::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_FR_RFoot::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_FR_RFoot")
		.Apply();
}

void CThugBulkyEnforcer_Walk_FR_RFoot::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Walk_FR_RFoot::Exit(CThugBulkyEnforcer* pOwner)
{
}

/*============================================================================*/
void CThugBulkyEnforcer_Walk_FL_RFoot::Enter(CThugBulkyEnforcer* pOwner)
{
	pOwner->Get_Component<CAnimator3D>()->Change_Animation("ThugBulkyEnforcer_Ani_Walk_FL_RFoot")
		.Apply();
}

void CThugBulkyEnforcer_Walk_FL_RFoot::Update(CThugBulkyEnforcer* pOwner, _float dt)
{
}

void CThugBulkyEnforcer_Walk_FL_RFoot::Exit(CThugBulkyEnforcer* pOwner)
{
}


