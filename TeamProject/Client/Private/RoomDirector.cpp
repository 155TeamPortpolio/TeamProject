#include "pch.h"
#include "RoomDirector.h"
#include "Room.h"
CRoomDirector::CRoomDirector()
{

}

HRESULT CRoomDirector::Initialize() 
{
	return S_OK;
}

void CRoomDirector::Update()
{
	if (!m_pendingEnterKey.empty()) {
		string key = m_pendingEnterKey;
		_bool overlay = m_pendingOverlay;
		m_pendingEnterKey.clear();

		DoEnter(key, overlay);
	}

	if (!m_ActiveStacks.empty()) {
		auto it = m_Rooms.find(m_ActiveStacks.back());
		if (it != m_Rooms.end() && it->second)
			it->second->Update();
	}
}

bool CRoomDirector::RegisterRoom(class CRoom* pRoom)
{
	if(!pRoom)
		return false;

	const string& key = pRoom->Key();
	if (key.empty())
		return false;

	auto it = m_Rooms.find(key);
	if (it != m_Rooms.end())
		return false;

	m_Rooms.emplace(key, pRoom);
	return true;
}

_bool CRoomDirector::RequestEnter(const string& roomKey, _bool overlay)
{
	auto it = m_Rooms.find(roomKey);
	if (it == m_Rooms.end() || !it->second)
		return false;

	if (!m_ActiveStacks.empty() && m_ActiveStacks.back() == roomKey)
		return false;

	m_pendingEnterKey = roomKey;
	m_pendingOverlay = overlay;
	return true;
}

bool CRoomDirector::RequestExitTop()
{
	if (m_ActiveStacks.empty())
		return false;
	if (m_ActiveStacks.size() <= 1)
		return false; 


	const string topKey = m_ActiveStacks.back();

	auto itTop = m_Rooms.find(topKey);
	if (itTop == m_Rooms.end() || !itTop->second)
		return false;

	CRoom* topRoom = itTop->second;

	if (topRoom->IsPersistent())
		return false;

	topRoom->Exit();

	m_ActiveStacks.pop_back();

	if (!m_ActiveStacks.empty())
	{
		const string& revealKey = m_ActiveStacks.back();
		auto itReveal = m_Rooms.find(revealKey);
		if (itReveal != m_Rooms.end() && itReveal->second)
		{
			itReveal->second->OnResumeFromOverlay();
		}
	}

	return true;
}

const vector<string>& CRoomDirector::GetActiveRoomStack() const
{
	return m_ActiveStacks;
}

void CRoomDirector::DoEnter(const string& key, _bool overlay)
{
	if (!overlay && !m_ActiveStacks.empty())
	{
		auto itCur = m_Rooms.find(m_ActiveStacks.back());
		if (itCur != m_Rooms.end() && itCur->second && !itCur->second->IsPersistent())
		{
			itCur->second->Exit();
			m_ActiveStacks.pop_back();
		}
	}

	auto it = m_Rooms.find(key);
	if (it == m_Rooms.end() || !it->second)
		return;

	it->second->Enter();
	m_ActiveStacks.push_back(key);
}

CRoomDirector* CRoomDirector::Create()
{
	CRoomDirector* pInstance = new CRoomDirector();
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CRoomDirector::Free()
{
	__super::Free();

	for (auto& pair : m_Rooms)
		Safe_Release(pair.second);
	m_Rooms.clear();
	m_ActiveStacks.clear();
	m_pendingEnterKey.clear();
}