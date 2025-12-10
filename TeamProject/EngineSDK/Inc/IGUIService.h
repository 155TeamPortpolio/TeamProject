#pragma once
#include "IService.h"
#include "GUI_Context.h"
NS_BEGIN(Engine)

extern "C" ENGINE_DLL ImGuiContext* GetEngineImGuiContext();

class ENGINE_DLL IGUIService :
    public IService
{
protected:
    virtual ~IGUIService() DEFAULT;

public:
    virtual ImGuiContext* GetEngineImGuiContext() PURE;
    virtual void Update(_float dt) PURE;
    virtual void Render_GUI() PURE;
    virtual  GUI_CONTEXT* Get_Context()PURE;
    virtual void Register_Panel(class CBasePanel* pPanel)PURE;
    virtual bool Set_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)PURE;
    virtual _bool UsingUI()PURE;
};

NS_END