#pragma once
#include "ITimeService.h"
class CTimer;

NS_BEGIN(Engine)

class CTimeMgr final :
    public ITimeService
{
private:
     CTimeMgr();
    virtual ~CTimeMgr()  DEFAULT;

public:
    CTimer* Add_Timer(const string& timerID);
    void Update_Timer(const string& timerID);
    _float Get_DeltaTime(const string& timerID,_bool raw = false);
    _float Get_TotalTime(const string& timerID);
    void Set_TimeScale(const string& timerID,_float scale);
  
private:
    CTimer* Find_Timer(const string& timerID);

private:
    unordered_map<string, CTimer*> m_Timers;
public:
    static CTimeMgr* Create();
    void Free();
};

NS_END