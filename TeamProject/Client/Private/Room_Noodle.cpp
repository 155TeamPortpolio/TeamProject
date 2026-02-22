#include "pch.h"
#include "Room_Noodle.h"

#include  "FieldSystem.h"
#include "GameInstance.h"

#include "FieldPlayer.h"
#include "CamDirector.h"
#include "Npc.h"
#include "UIDirector.h"

CRoom_Noodle::CRoom_Noodle(const ROOM_DESC& desc)
	:CRoom(desc)
{
}

void CRoom_Noodle::Enter()
{
	UIDirector()->FadeOut_Screen(0.4f, []()
		{
			auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
			pFieldPlayer->DeActive_Field();
			pFieldPlayer->Lock_Input();
			UIDirector()->FadeIn_Screen();
			CamDirector()->SetSpaceRef(FieldSystem()->GetInteractHandle());
			CamDirector()->RequestSequence("Field/Noodle");

			UIDirector()->Hide_HUD(CUIDirector::HUD::FIELD);
			UIDirector()->Show_Ramen();

			FieldSystem()->PlayBGM("NoodleBGM.wav", 0.04f);
		});
}

void CRoom_Noodle::Exit()
{
	FieldSystem()->FadeOutBGM();
	UIDirector()->FadeOut_Screen(0.4f, []()
		{
			auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
			pFieldPlayer->Active_Field();
			pFieldPlayer->UnLock_Input();
			UIDirector()->FadeIn_Screen();
			auto NpcHandle = FieldSystem()->GetInteractHandle();
			if (NpcHandle.isValid()) dynamic_cast<CNpc*>(NpcHandle.Get())->Reset();

			CamDirector()->AutoTarget();
			CamDirector()->RequestSequence("Field/Back");

			UIDirector()->Show_HUD(CUIDirector::HUD::FIELD);
			UIDirector()->Hide_Ramen();
		});
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
