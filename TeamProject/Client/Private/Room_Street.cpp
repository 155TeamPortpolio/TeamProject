#include "pch.h"
#include "Room_Street.h"
#include "MapLoader.h"

CRoom_Street::CRoom_Street(const ROOM_DESC& desc)
	:CRoom(desc)
{
}

void CRoom_Street::Enter()
{
	m_pLoader = CMapLoader::Create("MainCity_Level", "MainCity");
	if (nullptr == m_pLoader)
		MSG_BOX("Failed to Load MapData!");

	//m_pLoader->MapIndexToEntityHandle();
}

void CRoom_Street::Update()
{

}

void CRoom_Street::Exit()
{
	/*메인 스트리트는 나가지 않음*/
}

void CRoom_Street::OnResumeFromOverlay()
{
	/*메인 스트리트는 나가지 않음*/
}


CRoom_Street* CRoom_Street::Create(const ROOM_DESC& desc)
{
	CRoom_Street* instance = new CRoom_Street(desc);
	return instance;
}

void CRoom_Street::Free() 
{
	__super::Free(); 
	Safe_Release(m_pLoader);
}