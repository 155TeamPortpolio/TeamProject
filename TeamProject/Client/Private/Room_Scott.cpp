#include "pch.h"
#include "Room_Scott.h"
#include "MapLoader.h"

#include "FieldSystem.h"

CRoom_Scott::CRoom_Scott(const ROOM_DESC& desc) 
    :CRoom(desc)
{
}

void CRoom_Scott::Enter()
{
	m_pLoader = CMapLoader::Create("Scott_Level", "Zero_Worksite");
	if (nullptr == m_pLoader)
		MSG_BOX("Failed to Load MapData!");

	FieldSystem()->PlayBGM("ScottBGM.wav");
}

void CRoom_Scott::Exit()
{
}

void CRoom_Scott::Update()
{
}

void CRoom_Scott::OnResumeFromOverlay()
{
	FieldSystem()->PlayBGM("ScottBGM.wav");
}

CRoom_Scott* CRoom_Scott::Create(const ROOM_DESC& desc)
{
	CRoom_Scott* instance = new CRoom_Scott(desc);
	return instance;
}

void CRoom_Scott::Free()
{
	__super::Free();
	Safe_Release(m_pLoader);
}
