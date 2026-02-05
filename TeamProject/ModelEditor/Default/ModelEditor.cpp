#include "pch.h"

#include "framework.h"
#include "ModelEditor.h"
#include "MainApp.h"
#include "GameInstance.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE g_hInstance;                                // 현재 인스턴스입니다.
HWND g_hWnd;                                
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(22789770);
    // 덤프에 찍힌 번호 아무거나
   #endif

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MODELEDITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    CMainApp* mainApp = CMainApp::Create();
    CGameInstance* gameInstance = CGameInstance::GetInstance();
    Safe_AddRef(gameInstance);

    ITimeService* timer = gameInstance->Get_TimeMgr();
    Safe_AddRef(timer);
    timer->Add_Timer("Frame_Timer");

    if (!mainApp)
        return FALSE;

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    _float      fTimeAcc = {};
    _bool Break = false;
    const float step = 1.f / g_iMainFrame;
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                Break = true;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (Break) {
            break;
        }

        timer->Update_Timer("Frame_Timer");
        fTimeAcc += timer->Get_RawDeltaTime("Frame_Timer");

        if (true/*fTimeAcc >= step*/) {
            gameInstance->Update_EngineTimer();
            _float dt = gameInstance->Get_EngineDeltaTime();
            mainApp->Update(dt);
            mainApp->Render();
            fTimeAcc = 0.f;
        }
    }

    Safe_Release(timer);
    gameInstance->DestroyInstance();
    Safe_Release(mainApp);

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MODELEDITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

struct MonitorPickResult
{
    RECT workRect{};
    bool found = false;
};

static BOOL CALLBACK EnumMonProc(HMONITOR hMon, HDC, LPRECT, LPARAM userData)
{
    auto* data = reinterpret_cast<pair<int, MonitorPickResult*>*>(userData);

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfo(hMon, &monitorInfo))
        return TRUE;

    static int curIdx = 0;

    if (curIdx == data->first)
    {
        data->second->workRect = monitorInfo.rcWork;
        data->second->found = true;
        return FALSE;
    }
    curIdx++;
    return TRUE;
}

static MonitorPickResult GetWorkRectOfMonitorIndex(int monitorIdx)
{
    MonitorPickResult result{};
    pair<int, MonitorPickResult*> payload{monitorIdx, &result};

    EnumDisplayMonitors(nullptr, nullptr, EnumMonProc, (LPARAM)&payload);
    return result;
}

static RECT CalcWindowRectFromClientSize(int clientW, int clientH, DWORD style, DWORD exStyle)
{
    RECT rc{0, 0, clientW, clientH};
    AdjustWindowRectEx(&rc, style, FALSE, exStyle);
    return rc;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInstance = hInstance;

    HMONITOR hPrimary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hPrimary, &mi);

    const int primaryW = mi.rcMonitor.right - mi.rcMonitor.left;

    if (primaryW > 25000)
    {
        g_iWinSizeX = 2560;
        g_iWinSizeY = 1360;
    }
    else
    {
        g_iWinSizeX = 1900;
        g_iWinSizeY = 1000;
    }

    const DWORD style = WS_OVERLAPPEDWINDOW;
    const DWORD exStyle = 0;

    MonitorPickResult mon = GetWorkRectOfMonitorIndex(0);
    if (!mon.found) mon = GetWorkRectOfMonitorIndex(0);

    const int clientW = static_cast<int>(g_iWinSizeX);
    const int clientH = static_cast<int>(g_iWinSizeY);

    RECT winRc = CalcWindowRectFromClientSize(clientW, clientH, style, exStyle);
    const int windowW = winRc.right - winRc.left;
    const int windowH = winRc.bottom - winRc.top;

    const int x = mon.workRect.left;
    const int y = mon.workRect.top;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, style, x, y, windowW, windowH, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    g_hWnd = hWnd;
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (CGameInstance::GetInstance()->HandleMessage(hWnd, message, wParam, lParam))
        return 0;
    return DefWindowProc(hWnd, message, wParam, lParam);
}