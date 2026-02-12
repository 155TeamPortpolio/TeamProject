#pragma once
#include "Base.h"

NS_BEGIN(Client)

class CMonitorGate final
{
public:
    bool Pass() const;

    Vector2 GetMonitorSize(int monitorIdx = 0) const;

    int GetMonitorWidth(int monitorIdx = 0)  const { return GetMonitorSize(monitorIdx).x; }
    int GetMonitorHeight(int monitorIdx = 0) const { return GetMonitorSize(monitorIdx).y; }

    int GetMonitorCount() const;

private:
    struct MonitorDesc
    {
        HMONITOR hMon{};
        RECT     rc{};
        bool     isPrimary{};
    };

    static BOOL CALLBACK EnumMonitorsProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam);

    static vector<MonitorDesc> CollectMonitorsSorted();

private:
    int m_minWidth  = 2500;
    int m_minHeight = 0;
};

NS_END