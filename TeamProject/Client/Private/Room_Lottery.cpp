#include "pch.h"
#include "Room_Lottery.h"

CRoom_Lottery::CRoom_Lottery(const ROOM_DESC& desc)
	:CRoom(desc)
{
}

void CRoom_Lottery::Enter()
{
	//camera in
}

void CRoom_Lottery::Exit()
{
	//camera out
}

void CRoom_Lottery::Update()
{
}

void CRoom_Lottery::OnResumeFromOverlay()
{
}

CRoom_Lottery* CRoom_Lottery::Create(const ROOM_DESC& desc)
{
	CRoom_Lottery* instance = new CRoom_Lottery(desc);
	return instance;
}

void CRoom_Lottery::Free()
{
	__super::Free();
}
