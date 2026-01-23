#include "pch.h"
#include "RoomDirector.h"

CRoomDirector::CRoomDirector()
{

}

HRESULT CRoomDirector::Initialize() 
{
	return S_OK;
}

bool CRoomDirector::RegisterRoom(const ROOM_DESC& desc)
{
	return false;
}

bool CRoomDirector::RequestEnter(const string& roomKey, _bool overlay)
{
	return false;
}

bool CRoomDirector::RequestExitTop()
{
	return false;
}

const string& CRoomDirector::GetCityRoomKey() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return "";
}

const vector<string>& CRoomDirector::GetActiveRoomStack() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return {};
}

CRoomDirector* CRoomDirector::Create()
{
	CRoomDirector* pInstance = new CRoomDirector();
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CRoomDirector::Free()
{
	__super::Free();
}