#include "pch.h"
#include "Room_Gacha.h"

#include  "FieldSystem.h"
#include "GameInstance.h"

#include "FieldPlayer.h"
#include "UIDirector.h"

CRoom_Gacha::CRoom_Gacha(const ROOM_DESC& desc)
	:CRoom(desc)
{
}

void CRoom_Gacha::Enter()
{
	auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
	pFieldPlayer->Lock_Input();
	pFieldPlayer->DeActive_Field();
}

void CRoom_Gacha::Exit()
{
}

void CRoom_Gacha::Update()
{
}

void CRoom_Gacha::OnResumeFromOverlay()
{
}

CRoom_Gacha* CRoom_Gacha::Create(const ROOM_DESC& desc)
{
	CRoom_Gacha* instance = new CRoom_Gacha(desc);
	return instance;
}

void CRoom_Gacha::Free()
{
	__super::Free();
}
