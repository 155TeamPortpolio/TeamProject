#include "pch.h"
#include "Room.h"
#include "DataBase.h"

CRoom::CRoom(const ROOM_DESC& desc)
{
	m_isPersistent=desc.persistent;
	m_RoomKey = desc.roomKey;
}

void CRoom::ReserveMap(const string& LevelTag, const string& AreaTag)
{
	const CASHED_OBJ_DATA* datas = CDataBase::GetInstance()->Get_CashedData(AreaTag);
	m_MapObjects.reserve(100);
	auto& Entity = datas->Entity;
	for (auto& data : Entity)
	{
		m_MapObjects.push_back(data.Handle);
	}
	auto& MapObj = datas->MapObj;
	for (auto& data : MapObj)
	{
		m_MapObjects.push_back(data.Handle);
	}
	auto& InvisibleWall = datas->InvWall;
	for (auto& data : InvisibleWall)
	{
		m_MapObjects.push_back(data.Handle);
	}
	auto& Trigger = datas->Trigger;
	for (auto& data : Trigger)
	{
		m_MapObjects.push_back(data.Handle);
	}
}

void CRoom::DeActiveMap()
{
	for (auto handle : m_MapObjects)
		handle.Set_Alive(false);
}

void CRoom::ActiveMap()
{
	for (auto handle : m_MapObjects)
		handle.Set_Alive(true);
}

void CRoom::Free()
{
}