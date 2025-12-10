#pragma once
#include "IService.h"
NS_BEGIN(Engine)

class ENGINE_DLL IInputService abstract :
    public IService
{
protected:
    virtual ~IInputService() DEFAULT;
public:
    virtual void Update()PURE;
    virtual void Process_Input(LPARAM lParam) PURE;

    virtual _bool Key_Tap(int vk_Key)PURE;
    virtual _bool Key_Hold(int vk_Key) PURE;
    virtual _bool Key_Away(int vk_Key) PURE;
    virtual _bool Key_Down(int vk_Key) PURE;
    virtual _bool Mouse_Tap(MOUSE_BTN btn)PURE;
    virtual _bool Mouse_Hold(MOUSE_BTN btn) PURE;
    virtual _bool Mouse_Away(MOUSE_BTN btn) PURE;
    virtual _bool Mouse_Down(MOUSE_BTN btn) PURE;

    virtual const _float& Mouse_DeltaX() PURE;
    virtual const _float& Mouse_DeltaY() PURE;
    virtual const _float& Mouse_DeltaW()PURE;
    virtual const _float2& Mouse_Pos() PURE;

};

NS_END
