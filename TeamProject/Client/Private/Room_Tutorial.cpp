#include "pch.h"
#include "Room_Tutorial.h"
#include "MapLoader.h"

CRoom_Tutorial::CRoom_Tutorial(const ROOM_DESC& desc)
    :CRoom(desc)
{
}

void CRoom_Tutorial::Enter()
{
    m_pLoader = CMapLoader::Create("Scott_Level", "TrainingRoom");
    if (nullptr == m_pLoader)
        MSG_BOX("Failed to Load MapData!");
}

void CRoom_Tutorial::Exit()
{
}

void CRoom_Tutorial::Update()
{
}

void CRoom_Tutorial::OnResumeFromOverlay()
{
}

CRoom_Tutorial* CRoom_Tutorial::Create(const ROOM_DESC& desc)
{
    CRoom_Tutorial* instance = new CRoom_Tutorial(desc);
    return instance;
}

void CRoom_Tutorial::Free()
{
    __super::Free();
    Safe_Release(m_pLoader);
}
