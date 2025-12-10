#pragma once
#include "IInputService.h"

NS_BEGIN(Engine)

class CInputMgr final :
    public IInputService
{
private:
    explicit CInputMgr();
    virtual ~CInputMgr();
    virtual HRESULT Initialize(HWND hwnd);
public:
    virtual void Update();

public:
    virtual _bool Key_Tap(int vk_Key) override { return m_Keyboard[vk_Key].state == KEY_STATE::TAP; }
    virtual _bool Key_Hold(int vk_Key) override { return m_Keyboard[vk_Key].state == KEY_STATE::HOLD; }
    virtual _bool Key_Away(int vk_Key) override { return m_Keyboard[vk_Key].state == KEY_STATE::AWAY; }
    virtual _bool Key_Down(int vk_Key) override { return Key_Tap(vk_Key) || Key_Hold(vk_Key); }
    virtual _bool Mouse_Tap(MOUSE_BTN btn) override { return m_Mouse.mouseKey[static_cast<int>(btn)].state == KEY_STATE::TAP; } ;
    virtual _bool Mouse_Hold(MOUSE_BTN btn)override { return m_Mouse.mouseKey[static_cast<int>(btn)].state == KEY_STATE::HOLD; } ;
    virtual _bool Mouse_Away(MOUSE_BTN btn)override { return m_Mouse.mouseKey[static_cast<int>(btn)].state == KEY_STATE::AWAY; } ;
    virtual _bool Mouse_Down(MOUSE_BTN btn)override { return Mouse_Tap(btn) || Mouse_Hold(btn); }
    virtual const _float& Mouse_DeltaX() override { return m_Mouse.fDeltaX; } ;
    virtual const _float& Mouse_DeltaY()override { return m_Mouse.fDeltaY; } ;
    virtual const _float& Mouse_DeltaW() override { return m_Mouse.fWheelDelta; } ;
    virtual const _float2& Mouse_Pos() override;

    void Process_Input(LPARAM lParam);
private:
    void Process_RawInput(RAWINPUT* raw);

private:
    HWND m_hWnd = {nullptr};
    vector<BYTE> m_vecInputMsg;
    UINT messageSize = {};

    vector<KEY_DESC> m_Keyboard;
    MOUSE_DESC m_Mouse = {};
    POINT m_pMousePos = {};
private:
    void HandleKeyboardInput(const RAWKEYBOARD& keyboard);
    void HandleMouseInput(const RAWMOUSE& mouse);

public:
    static CInputMgr* Create(HWND hwnd);
    virtual void Free() override;
};
NS_END
