#include "pch.h"
#include "DisplayGate.h"

bool CMonitorGate::Pass() const
{
    HMONITOR hMon = MonitorFromWindow(g_hWnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMon, &mi);

    const int w = mi.rcMonitor.right - mi.rcMonitor.left;
    const int h = mi.rcMonitor.bottom - mi.rcMonitor.top;

    if (w < m_minWidth)  return false;
    if (h < m_minHeight) return false;

    MSG_BOX("패딩 버려라");
    return true;
}

Vector2 CMonitorGate::GetMonitorSize(int monitorIdx) const
{
    vector<MonitorDesc> monitors = CollectMonitorsSorted();
    const RECT rc = monitors[monitorIdx].rc;

    Vector2 size{};
    size.x = rc.right  - rc.left;
    size.y = rc.bottom - rc.top;
    return size;
}

int CMonitorGate::GetMonitorCount() const
{
    vector<HMONITOR> monitors;
    EnumDisplayMonitors(nullptr, nullptr, &EnumMonitorsProc, reinterpret_cast<LPARAM>(&monitors));
    return (int)monitors.size();
}

BOOL CMonitorGate::EnumMonitorsProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam)
{
    auto& monitors = *reinterpret_cast<vector<HMONITOR>*>(lParam);
    monitors.push_back(hMon);
    return TRUE;
}

vector<CMonitorGate::MonitorDesc> CMonitorGate::CollectMonitorsSorted()
{
    vector<HMONITOR> raw;
    EnumDisplayMonitors(nullptr, nullptr, &EnumMonitorsProc, reinterpret_cast<LPARAM>(&raw));

    vector<MonitorDesc> monitors;
    monitors.reserve(raw.size());

    for (HMONITOR hMon : raw)
    {
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(hMon, &mi);

        MonitorDesc desc{};
        desc.hMon = hMon;
        desc.rc = mi.rcMonitor;
        desc.isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        monitors.push_back(desc);
    }

    sort(monitors.begin(), monitors.end(), [](const MonitorDesc& a, const MonitorDesc& b)
        {
            if (a.isPrimary != b.isPrimary) return a.isPrimary > b.isPrimary;
            if (a.rc.left != b.rc.left)   return a.rc.left < b.rc.left;
            return a.rc.top < b.rc.top;
        });

    return monitors;
}
