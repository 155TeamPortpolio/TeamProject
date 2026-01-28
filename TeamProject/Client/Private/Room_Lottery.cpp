#include "pch.h"
#include "Room_Lottery.h"

#include  "FieldSystem.h"
#include "GameInstance.h"
#include "CamDirector.h"

#include "FieldPlayer.h"

CRoom_Lottery::CRoom_Lottery(const ROOM_DESC& desc)
	:CRoom(desc)
{
}

void CRoom_Lottery::Enter()
{
	auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
	pFieldPlayer->Lock_Input();
	pFieldPlayer->DeActive_Field();

	CamDirector()->SetSpaceRef(FieldSystem()->GetInteractHandle());
	CamDirector()->RequestSequence("Field/Howl");
}

void CRoom_Lottery::Exit()
{
	auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
	pFieldPlayer->UnLock_Input();
	pFieldPlayer->Active_Field();

	CamDirector()->SetSpaceRef(CamDirector()->GetCurHandle());
	CamDirector()->RequestSequence("Field/Front");
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
