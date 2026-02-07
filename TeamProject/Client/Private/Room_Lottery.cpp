#include "pch.h"
#include "Room_Lottery.h"

#include  "FieldSystem.h"
#include "GameInstance.h"
#include "CamDirector.h"
#include "UIDirector.h"

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

	UIDirector()->Hide_HUD(CUIDirector::HUD::FIELD);
	UIDirector()->Show_Lottery();
}

void CRoom_Lottery::Exit()
{
	auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
	pFieldPlayer->Active_Field();
	pFieldPlayer->UnLock_Input();

	CamDirector()->SetSpaceRef(CamDirector()->GetCurHandle());
	CamDirector()->RequestSequence("Field/Back");
	
	UIDirector()->Show_HUD(CUIDirector::HUD::FIELD);
	UIDirector()->Hide_Lottery();
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
