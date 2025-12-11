#pragma once
#include "Base.h"
class CTimer final :
    public CBase
{
private:
    CTimer();
    virtual ~CTimer() = default;

public:
    HRESULT Initialize();
    void Update_Timer();
    _float Get_DeltaTime(_bool raw);
    _float Get_TotalTime();
    void Set_TimeScale(_float Scale) { m_fTimeScale = Scale; };

private:
    _float m_fDeltaTime{};
    _float m_fTotalTime{};
    _float m_fTimeScale{1.f};

    LARGE_INTEGER m_TickCount{};
    LARGE_INTEGER m_CurTime{};
    LARGE_INTEGER m_LastTime{};
    LARGE_INTEGER		m_FixTime{};

public:
    static		CTimer* Create();
    virtual void Free() override;

};

