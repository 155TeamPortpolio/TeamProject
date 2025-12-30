#include "Engine_Defines.h"
#include "EventListener.h"
#include "GameInstance.h"
CEventListener::CEventListener()
{
}

CEventListener::CEventListener(const CEventListener& rhs)
	: CComponent(rhs)
{
}

HRESULT CEventListener::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEventListener::Initialize(COMPONENT_DESC* pArg)
{
	m_pSystem = CGameInstance::GetInstance()->Get_EventSystem();
	return S_OK;
}

void CEventListener::Set_CompActive(_bool bActive)
{
	if (bActive == false)
		Clear();
}

void CEventListener::Render_GUI()
{
	
}

CEventListener* CEventListener::Create()
{
	CEventListener* instance = new CEventListener();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("EventListner Comp Failed To Create : CEventListner");
	}
	return instance;
}

CComponent* CEventListener::Clone()
{
	return new CEventListener(*this);
}

void CEventListener::Free()
{
	Clear();
	__super::Free();
}
