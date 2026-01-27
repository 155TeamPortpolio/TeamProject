#include "pch.h"
#include "DefilerState_Attack.h"
#include "Defiler.h"
#include "StateMachine.h"

CDefilerState_Attack* CDefilerState_Attack::Create()
{
	CDefilerState_Attack* pInstance = new CDefilerState_Attack();
	pInstance->m_pSubStateMachine = CStateMachine<CDefiler>::Create();
	//pInstance->m_pSubStateMachine->Register_State();
	return pInstance;
}

void CDefilerState_Attack::Enter(CDefiler* pOwner)
{
	__super::Enter(pOwner);
}

void CDefilerState_Attack::Update(CDefiler* pOwner, _float dt)
{
	__super::Update(pOwner, dt);
}

void CDefilerState_Attack::Exit(CDefiler* pOwner)
{
}
