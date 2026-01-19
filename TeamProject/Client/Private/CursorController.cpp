#include "pch.h"
#include "CursorController.h"

#include "GameInstance.h"

extern HWND g_hWnd;

void CCursorController::Initialize()
{
    m_cachedGuiActive = GUISystem()->Is_GUIActive();
    SyncMouseLockByGui();
}

RECT CCursorController::GetClientRectInScreen() const
{
    RECT rc{};
    GetClientRect(g_hWnd, &rc);

    POINT lt{rc.left, rc.top};
    POINT rb{rc.right, rc.bottom};

    ClientToScreen(g_hWnd, &lt);
    ClientToScreen(g_hWnd, &rb);

    rc.left = lt.x;
    rc.top = lt.y;
    rc.right = rb.x;
    rc.bottom = rb.y;

    return rc;
}

POINT CCursorController::GetClientCenterInScreen() const
{
    RECT rc = GetClientRectInScreen();

    POINT c{};
    c.x = (rc.left + rc.right) / 2;
    c.y = (rc.top + rc.bottom) / 2;
    return c;
}

void CCursorController::SetMouseLock(_bool lock)
{
    m_isMouseLocked = lock;

    if (lock)
    {
        RECT rc = GetClientRectInScreen();
        ClipCursor(&rc);

        while (ShowCursor(FALSE) >= 0) {}

        POINT c = GetClientCenterInScreen();
        SetCursorPos(c.x, c.y);
    }
    else
    {
        ClipCursor(nullptr);
        while (ShowCursor(TRUE) < 0) {}
    }
}

void CCursorController::ToggleMouseLock()
{
    SetMouseLock(!m_isMouseLocked);
}

void CCursorController::SyncMouseLockByGui()
{
    if (m_cachedGuiActive) SetMouseLock(false);
    else                   SetMouseLock(true);
}

void CCursorController::ToggleGui()
{
    m_cachedGuiActive = !m_cachedGuiActive;
    GUISystem()->Set_GUIActive(m_cachedGuiActive);
    SyncMouseLockByGui();
}

void CCursorController::Update(_float dt)
{
    const _bool guiNow = GUISystem()->Is_GUIActive();
    if (guiNow != m_cachedGuiActive)
    {
        m_cachedGuiActive = guiNow;
        SyncMouseLockByGui();
    }

    if (InputDevice()->Key_Tap(VK_TAB))
        ToggleGui();

    if (InputDevice()->Key_Tap(VK_CAPITAL))
        ToggleMouseLock();

    if (m_isMouseLocked)
    {
        POINT c = GetClientCenterInScreen();
        SetCursorPos(c.x, c.y);
    }
}