#include "pch.h"
#include "Room_Party.h"

#include  "FieldSystem.h"
#include "FieldPlayer.h"
#include "UIDirector.h"
#include "DataBase.h"

CRoom_Party::CRoom_Party(const ROOM_DESC& desc)
    :CRoom(desc)
{
}

void CRoom_Party::Enter()
{
    UIDirector()->FadeOut_Screen(0.4f, []()
        {
            UIDirector()->Show_Party(CDataBase::GetInstance()->Get_EnableCharacters());
            UIDirector()->FadeIn_Screen();
            FieldSystem()->PlayBGM("PartyBGM.wav", 0.15f);
        });
    auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
    pFieldPlayer->Lock_Input();
    pFieldPlayer->DeActive_Field();
    //UIDirector()->Show_Party(CDataBase::GetInstance()->Get_EnableCharacters());
}

void CRoom_Party::Exit()
{
    FieldSystem()->FadeOutBGM();
    auto pFieldPlayer = FieldSystem()->GetFieldPlayer();
    pFieldPlayer->Active_Field();
    pFieldPlayer->UnLock_Input();

    UIDirector()->Hide_Party();
}

void CRoom_Party::Update()
{
}

void CRoom_Party::OnResumeFromOverlay()
{
}

CRoom_Party* CRoom_Party::Create(const ROOM_DESC& desc)
{
    CRoom_Party* instance = new CRoom_Party(desc);
    return instance;
}

void CRoom_Party::Free()
{
    __super::Free();
}
