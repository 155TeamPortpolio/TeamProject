#pragma once

NS_BEGIN(Client)

class CMonitorGate final
{
public:
    void SetMinWidth(int px)  { m_minWidth  = px; }
    void SetMinHeight(int px) { m_minHeight = px; }

    bool Pass() const
    {
        HMONITOR hMon = MonitorFromWindow(g_hWnd, MONITOR_DEFAULTTONEAREST);

        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(hMon, &mi);

        const int w = mi.rcMonitor.right  - mi.rcMonitor.left;
        const int h = mi.rcMonitor.bottom - mi.rcMonitor.top;

        if (w < m_minWidth)  return false;
        if (h < m_minHeight) return false;
        return true;
    }

private:
    int m_minWidth  = 25000;
    int m_minHeight = 0;
};

NS_END