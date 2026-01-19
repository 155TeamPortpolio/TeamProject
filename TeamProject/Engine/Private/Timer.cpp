#include "Engine_Defines.h"
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
	m_fRawTotalTime = m_fScaledTotalTime = 0.f;
	m_fTimeScale = 1.f;

	return S_OK;
}

void CTimer::Update_Timer()
{
	QueryPerformanceCounter(&m_CurTime);

	const LONGLONG diff = m_CurTime.QuadPart - m_LastTime.QuadPart;
	m_LastTime = m_CurTime;

	// raw dt (sec)
	_float raw = (_float)((double)diff / (double)m_TickCount.QuadPart);

	// 튐 방지(선택): alt-tab, 브레이크포인트 등
	if (raw < 0.f) raw = 0.f;
	if (raw > 0.1f) raw = 0.1f;

	m_fRawDeltaTime = raw;

	_float scaled = raw * m_fTimeScale;

	// 필요하면 scaled도 clamp (예: 30fps 제한)
	scaled = min(scaled, 0.033f);

	m_fScaledDeltaTime = scaled;

	m_fRawTotalTime += m_fRawDeltaTime;
	m_fScaledTotalTime += m_fScaledDeltaTime;
}

_float CTimer::Get_DeltaTime(_bool raw)
{
	if (!raw)
		return min(m_fScaledDeltaTime, 0.033);
	else
		return m_fScaledDeltaTime;
}

_float CTimer::Get_TotalTime()
{
	return m_fScaledTotalTime;
}

_float CTimer::Get_RawDeltaTime(_bool raw)
{
	if (!raw)
		return min(m_fRawDeltaTime, 0.033);
	else
		return m_fRawDeltaTime;
}

_float CTimer::Get_RawTotalTime()
{
	return m_fRawTotalTime;
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
