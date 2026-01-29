#include "pch.h"
#include "Room_Noodle.h"

#include  "FieldSystem.h"
#include "GameInstance.h"

#include "FieldPlayer.h"
#include "CamDirector.h"

CRoom_Noodle::CRoom_Noodle(const ROOM_DESC& desc)
	:CRoom(desc)
{
}

void CRoom_Noodle::Enter()
{
	auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
	pFieldPlayer->Lock_Input();
	pFieldPlayer->DeActive_Field();

	CamDirector()->SetSpaceRef(FieldSystem()->GetInteractHandle());
	CamDirector()->RequestSequence("Field/Noodle");
}

void CRoom_Noodle::Exit()
{
	auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
	pFieldPlayer->UnLock_Input();
	pFieldPlayer->Active_Field();
	
	CamDirector()->AutoTarget();
	CamDirector()->RequestSequence("Field/Back");
}

void CRoom_Noodle::Update()
{
}

void CRoom_Noodle::OnResumeFromOverlay()
{
}

CRoom_Noodle* CRoom_Noodle::Create(const ROOM_DESC& desc)
{
	CRoom_Noodle* instance = new CRoom_Noodle(desc);
	return instance;
}

void CRoom_Noodle::Free()
{
	__super::Free();
}
