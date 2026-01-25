#include "pch.h"
#include "FieldSystem.h"
#include "FieldPlayer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "RoomDirector.h"
IMPLEMENT_SINGLETON(CFieldSystem)

CFieldSystem::CFieldSystem()
{
	m_pRoomDirector = CRoomDirector::Create();
}

void CFieldSystem::Update()
{
	if (false == m_isActive)
		return;
	_float delta = TimeManager()->Get_DeltaTime(G_EngineTimerID);

	m_DayTime.Update_Timer(delta);
	if (m_pRoomDirector)
		m_pRoomDirector->Update();
}

void CFieldSystem::SetFieldPlayer(CFieldPlayer* pFieldPlayer)
{
	if (pFieldPlayer)
	{
		if (m_pFieldPlayer)
		{
			Safe_Release(m_pFieldPlayer);
			m_pFieldPlayer = nullptr;
		}

		m_pFieldPlayer = pFieldPlayer;
		Safe_AddRef(m_pFieldPlayer);
	}
}

_bool CFieldSystem::RegisterRoom(class CRoom* pRoom)
{
	return m_pRoomDirector->RegisterRoom(pRoom);
}

_bool CFieldSystem::RequestEnter(const string& roomKey, _bool overlay)
{
	return m_pRoomDirector->RequestEnter(roomKey, overlay);
}

_bool CFieldSystem::RequestExitTop()
{
	return m_pRoomDirector->RequestExitTop();
}

/*isValid 체크 필요!*/
OBJECT_HANDLE CFieldSystem::GetCurCharacterHandle() const
{
	if (m_pFieldPlayer)
		return m_pFieldPlayer->GetCurCharacterHandle();
	else
		return OBJECT_HANDLE();
}

void CFieldSystem::SetInteractHandle(OBJECT_HANDLE InteractHandle)
{
	m_InteractHandle = InteractHandle;
}

OBJECT_HANDLE CFieldSystem::GetInteractHandle() const
{
	return m_InteractHandle;
}

void CFieldSystem::Free()
{
	__super::Free();
	Safe_Release(m_pFieldPlayer);
	Safe_Release(m_pRoomDirector);
}

void CFieldSystem::DayTimer::Set_DayPhase(DayPhase ePhase, _bool bTimer)
{
	m_eDayTime = ePhase;

	switch (ePhase)
	{
	case Client::DayPhase::EarlyMorning:
		RenderSystem()->Set_FogDesc({ _float4(0.95f, 0.75f, 0.8f, 1.0f),0.f,0.f, 0.003f, true});
		if(!bTimer) currentHour = 0.f;
		break;
	case Client::DayPhase::Morning:
		RenderSystem()->Set_FogDesc({ _float4(0.85f, 0.9f, 0.95f, 1.0f),0.f,0.f, 0.0015f, true });
		if (!bTimer) currentHour = 6.0f;
		break;
	case Client::DayPhase::Afternoon:
		RenderSystem()->Set_FogDesc({ _float4(1.0f, 0.55f, 0.45f, 1.0f), 0.f, 0.f, 0.0035f, true });
		if (!bTimer) currentHour = 12.0f;
		break;
	case Client::DayPhase::LateNight:
		RenderSystem()->Set_FogDesc({ _float4(0.25f, 0.3f, 0.45f, 1.0f),0.f,0.f,  0.004f, true });
		if (!bTimer) currentHour = 18.0f;
		break;
	default:
		break;
	}

	if (m_eDayTime != m_ePreDayTime)
	{
		Notify_DayPhaseEvent();
		m_ePreDayTime = m_eDayTime;
	}
}

void CFieldSystem::DayTimer::Notify_DayPhaseEvent()
{
	DAYPHASE_DESC desc{};
	desc.dayPhase = m_eDayTime;
	EventSystem()->Broadcast<DAYPHASE_DESC>({ desc });
}
