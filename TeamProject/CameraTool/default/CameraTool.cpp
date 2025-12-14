#include "pch.h"
#include "CameraTool.h"
#include "GameInstance.h"
#include "MainApp.h"

#define MAX_LOADSTRING 100

HINSTANCE g_Inst;
HWND      g_hWnd;
WCHAR     szTitle[MAX_LOADSTRING];                
WCHAR     szWindowClass[MAX_LOADSTRING];         

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CAMERATOOL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    auto mainApp = MainApp::Create();
    auto game = CGameInstance::GetInstance();
    Safe_AddRef(game);

    auto timer = game->Get_TimeMgr();
    Safe_AddRef(timer);
    timer->Add_Timer("Default");
    timer->Add_Timer("FPS_60");

    MSG    msg{};
    _float timeAcc{};
    _bool  stop = false;

    while (1)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                stop = true;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (stop)
            break;

        timer->Update_Timer("Default");
        timeAcc += timer->Get_DeltaTime("Default");

        if (timeAcc >= 1.f / FrameRate)
        {
            timer->Update_Timer("FPS_60");
            _float dt = timer->Get_DeltaTime("FPS_60");
            mainApp->Update(dt);
            mainApp->Render();
            timeAcc = 0.f;
        }
    }

    Safe_Release(timer);
    game->DestroyInstance();
    Safe_Release(mainApp);

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CAMERATOOL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
// =======================================================================================
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
    pair<int, MonitorPickResult*> payload{ monitorIdx, &result };

    EnumDisplayMonitors(nullptr, nullptr, EnumMonProc, (LPARAM)&payload);
    return result;
}
static RECT CalcWindowRectFromClientSize(int clientW, int clientH, DWORD style, DWORD exStyle)
{
    RECT rc{ 0, 0, clientW, clientH };
    AdjustWindowRectEx(&rc, style, FALSE, exStyle);
    return rc; 
}
// =========================================================================================
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_Inst = hInstance;

    const DWORD style = WS_OVERLAPPEDWINDOW;
    const DWORD exStyle = 0;

    // 1) 보조 모니터(두번째) work rect 얻기
    //    - 0이면 첫 모니터, 1이면 두번째 모니터
    MonitorPickResult mon = GetWorkRectOfMonitorIndex(1);
    if (!mon.found)
    {
        // 보조 모니터 없으면 첫 모니터로 폴백
        mon = GetWorkRectOfMonitorIndex(0);
    }

    // 2) 원하는 "클라이언트" 크기
    const int clientW = static_cast<int>(WinX);
    const int clientH = static_cast<int>(WinY);

    // 3) 스타일을 반영해 창 전체 크기 계산
    RECT winRc = CalcWindowRectFromClientSize(clientW, clientH, style, exStyle);
    const int windowW = winRc.right - winRc.left;
    const int windowH = winRc.bottom - winRc.top;

    // 4) work area 안에 좌상단 배치 (혹은 가운데 배치)
    const int x = mon.workRect.left;
    const int y = mon.workRect.top;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, style,
        x, y, windowW, windowH,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    g_hWnd = hWnd;
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto game = CGameInstance::GetInstance();
    if (game->HandleMessage(hWnd, message, wParam, lParam))
        return 0;
    return DefWindowProc(hWnd, message, wParam, lParam);
}