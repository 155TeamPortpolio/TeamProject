#include "pch.h"
#include "Room.h"

CRoom::CRoom(const ROOM_DESC& desc)
{
	m_isPersistent=desc.persistent;
	m_RoomKey = desc.roomKey;
}

void CRoom::Free()
{
}