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
    virtual void Set_UIMode() PURE;
    virtual void Set_Bone(_int boneIndex)PURE;
    virtual void Set_GUIActive(_bool active)PURE;
    virtual _bool Is_GUIActive() const PURE;
};

NS_END