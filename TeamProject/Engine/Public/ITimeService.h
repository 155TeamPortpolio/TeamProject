#pragma once
#include "IService.h"
class CTimer;

NS_BEGIN(Engine)

class ENGINE_DLL ITimeService abstract:
    public IService
{
protected:
    virtual ~ITimeService() DEFAULT;

public:
    virtual CTimer* Add_Timer(const string& imguiID) PURE;
    virtual void Update_Timer(const string& imguiID) PURE;
    virtual _float Get_DeltaTime(const string& imguiID, _bool raw = false) PURE;
    virtual void Set_TimeScale(const string& imguiID, _float scale) PURE;
    virtual _float Get_TotalTime(const string& timerID) PURE;
};

NS_END


///밖에 내보낼 서비스 ->서비스 패턴이라고 하드라고 그래서 서비스라고 함