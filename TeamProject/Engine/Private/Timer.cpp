#include "Timer.h"

CTimer::CTimer()
{
}


HRESULT CTimer::Initialize()
{
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceFrequency(&m_TickCount);

	return S_OK;
}

void CTimer::Update_Timer()
{
	QueryPerformanceCounter(&m_CurTime);	

	if (m_CurTime.QuadPart - m_FixTime.QuadPart >= m_TickCount.QuadPart)
	{
		QueryPerformanceFrequency(&m_TickCount);
		m_FixTime = m_CurTime;
	}

	m_fDeltaTime = (m_CurTime.QuadPart - m_LastTime.QuadPart) / static_cast<_float>(m_TickCount.QuadPart);
	m_LastTime = m_CurTime;
	m_fTotalTime += m_fDeltaTime;
}

_float CTimer::Get_DeltaTime(_bool raw)
{
	if (!raw)
		return min(m_fDeltaTime* m_fTimeScale, 0.033);
	else
		return m_fDeltaTime* m_fTimeScale;
}

_float CTimer::Get_TotalTime()
{
	return m_fTotalTime;
}

CTimer* CTimer::Create()
{
	CTimer* instance = new CTimer();

	if (FAILED(instance->Initialize())) {
		Safe_Release(instance);
		instance = nullptr;
	}

	return instance;
}

void CTimer::Free()
{
	__super::Free();
}
