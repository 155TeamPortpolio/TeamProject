#include "pch.h"
#include "FieldSystem.h"
#include "FieldPlayer.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CFieldSystem)

CFieldSystem::CFieldSystem()
{
}

void CFieldSystem::Update()
{
	if (false == m_isActive)
		return;
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

/*isValid 체크 필요!*/
OBJECT_HANDLE CFieldSystem::GetCurCharacterHandle() const
{
	if (m_pFieldPlayer)
		return m_pFieldPlayer->GetCurCharacterHandle();
	else
		return OBJECT_HANDLE();
}

void CFieldSystem::Free()
{
	__super::Free();
	Safe_Release(m_pFieldPlayer);
}
