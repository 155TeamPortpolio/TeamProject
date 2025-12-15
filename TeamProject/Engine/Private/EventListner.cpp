#include "Engine_Defines.h"
#include "EventListner.h"
#include "GameInstance.h"
CEventListner::CEventListner()
{
}

CEventListner::CEventListner(const CEventListner& rhs)
	: CComponent(rhs)
{
}

HRESULT CEventListner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEventListner::Initialize(COMPONENT_DESC* pArg)
{
	m_pSystem = CGameInstance::GetInstance()->Get_EventSystem();
	return S_OK;
}

void CEventListner::Set_CompActive(_bool bActive)
{
	if (bActive == false)
		Clear();
}

void CEventListner::Render_GUI()
{
	
}

CEventListner* CEventListner::Create()
{
	CEventListner* instance = new CEventListner();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("EventListner Comp Failed To Create : CEventListner");
	}
	return instance;
}

CComponent* CEventListner::Clone()
{
	return new CEventListner(*this);
}

void CEventListner::Free()
{
	Clear();
	__super::Free();
}
