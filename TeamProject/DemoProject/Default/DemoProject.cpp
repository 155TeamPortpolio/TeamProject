
#include "framework.h"
#include "DemoProject.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Demo_Defines.h"

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
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DEMOPROJECT, szWindowClass, MAX_LOADSTRING);
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
    timer->Add_Timer("Default_Timer");
    timer->Add_Timer("Timer_Frame60");

    if (!mainApp)
        return FALSE;

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    _float      fTimeAcc = {};
    _bool Break = false;

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
        timer->Update_Timer("Default_Timer");
        fTimeAcc += timer->Get_DeltaTime("Default_Timer");

        if (fTimeAcc >= 1.f / g_iMainFrame) {
            timer->Update_Timer("Timer_Frame60");
            _float dt = timer->Get_DeltaTime("Timer_Frame60");
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEMOPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInstance = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
    RECT rc{ 0, 0, Demo::g_iWinSizeX, Demo::g_iWinSizeY };

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,
        rc.right - rc.left,
        rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        DWORD err = GetLastError();

        wchar_t buf[256];
        swprintf_s(buf, L"CreateWindowW 실패. GetLastError = %u", err);
        MessageBoxW(nullptr, buf, L"Error", MB_OK);

        return FALSE;
    }


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_hWnd = hWnd;
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CGameInstance* pEngine = CGameInstance::GetInstance();

    if (pEngine->HandleMessage(hWnd, message, wParam, lParam))
        return 0; //엔진에서 메시지 처리함.

    return DefWindowProc(hWnd, message, wParam, lParam);
}

