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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_Inst = hInstance;
    int monitorW = GetSystemMetrics(SM_CXSCREEN);
    
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        DefaultX, DefaultY, WinX, WinY,
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