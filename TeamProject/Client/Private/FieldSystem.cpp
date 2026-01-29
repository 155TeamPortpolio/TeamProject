#include "pch.h"
#include "FieldSystem.h"
#include "FieldPlayer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "RoomDirector.h"
#include "TestCloud.h"
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

void CFieldSystem::SetActive(_bool is)
{
	if (is == false)
	{
		m_pRoomDirector->ClearRooms();
	}
	m_isActive = is;
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

CFieldPlayer* CFieldSystem::GetFieldPlayer()
{
	return m_pFieldPlayer;
}

void CFieldSystem::SetInteractHandle(OBJECT_HANDLE InteractHandle, OBJECT_HANDLE InteractPartnerHandle)
{
	m_InteractHandle = InteractHandle;
	m_InteractPartnerHandle = InteractPartnerHandle;
}

void CFieldSystem::ResetInteractHandle()
{
	m_InteractHandle = OBJECT_HANDLE{};
	m_InteractPartnerHandle = OBJECT_HANDLE{};
}

OBJECT_HANDLE CFieldSystem::GetInteractHandle() const
{
	return m_InteractHandle;
}

OBJECT_HANDLE CFieldSystem::GetInteractPartnerHandle() const
{
	return m_InteractPartnerHandle;
}

void CFieldSystem::Free()
{
	__super::Free();
	Safe_Release(m_pFieldPlayer);
	Safe_Release(m_pRoomDirector);
}

void CFieldSystem::DayTimer::Set_DayPhase(DayPhase ePhase)
{
	m_eDayTime = ePhase;
	if (m_eDayTime == m_ePreDayTime) return;

	m_ePreDayTime = m_eDayTime;

	auto pCloud = dynamic_cast<CTestCloud*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Cloud)));
	switch (ePhase)
	{
	case Client::DayPhase::EarlyMorning:
		RenderSystem()->Set_FogDesc({ _float4(0.95f, 0.75f, 0.8f, 1.0f),0.f,0.f, 0.003f, true });
		pCloud->Set_CloudInfo(_float3(0.7f, 0.5f, 0.65f), _float3(0.95f, 0.7f, 0.75f) );
		currentHour = 0.f;
		break;
	case Client::DayPhase::Morning:
		RenderSystem()->Set_FogDesc({ _float4(0.85f, 0.9f, 0.95f, 1.0f),0.f,0.f, 0.0015f, true });
		pCloud->Set_CloudInfo(_float3(0.4f, 0.7f, 1.0f), _float3(1.0f, 1.0f, 1.0f));
		currentHour = 6.0f;
		break;
	case Client::DayPhase::Afternoon:
		RenderSystem()->Set_FogDesc({ _float4(1.0f, 0.55f, 0.45f, 1.0f), 0.f, 0.f, 0.0035f, true });
		pCloud->Set_CloudInfo(_float3(0.486f, 0.073f, 0.073f), _float3(1.0f, 0.6f, 0.5f));
		currentHour = 12.0f;
		break;
	case Client::DayPhase::LateNight:
		RenderSystem()->Set_FogDesc({ _float4(0.25f, 0.3f, 0.45f, 1.0f),0.f,0.f,  0.004f, true });
		pCloud->Set_CloudInfo(_float3(0.1f, 0.15f, 0.3f), _float3(0.3f, 0.35f, 0.5f));
		currentHour = 18.0f;
		break;
	default:
		break;
	}

	Notify_DayPhaseEvent();
}

void CFieldSystem::DayTimer::Notify_DayPhaseEvent()
{
	DAYPHASE_DESC desc{};
	desc.dayPhase = m_eDayTime;
	EventSystem()->Broadcast<DAYPHASE_DESC>({ desc });
}
