#include "pch.h"
#include "Room_Gacha.h"

#include  "FieldSystem.h"
#include "GameInstance.h"

#include "FieldPlayer.h"
#include "UIDirector.h"
#include "CamDirector.h"

CRoom_Gacha::CRoom_Gacha(const ROOM_DESC& desc)
	:CRoom(desc)
{
}

void CRoom_Gacha::Enter()
{
	auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
	pFieldPlayer->Lock_Input();
	pFieldPlayer->DeActive_Field();
	UIDirector()->Hide_HUD(CUIDirector::HUD::FIELD);
	UIDirector()->Show_GachaPage();
	CamDirector()->GetOrbitCam()->Lock_Input();
	FieldSystem()->PlayBGM("GachaBGM.wav", 0.2f);
}

void CRoom_Gacha::Exit()
{
	FieldSystem()->FadeOutBGM();
	auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
	pFieldPlayer->UnLock_Input();
	pFieldPlayer->Active_Field();
	UIDirector()->Show_HUD(CUIDirector::HUD::FIELD);
	UIDirector()->Hide_GachaPage();
	CamDirector()->GetOrbitCam()->Unlock_Input();
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
