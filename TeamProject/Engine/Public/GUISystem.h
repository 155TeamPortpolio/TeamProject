#pragma once
#include "IGUIService.h"
#include "GUIWidget.h"
#include "GUI_Context.h"

NS_BEGIN(Engine)

class CGUISystem :
    public IGUIService
{
private:
    CGUISystem();
   virtual  ~CGUISystem() DEFAULT;

public:
    HRESULT Initialize(const ENGINE_DESC& engine, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Update(_float dt);
    virtual void Render_GUI();
    virtual ImGuiContext* GetEngineImGuiContext() override;
public:
    virtual GUI_CONTEXT* Get_Context() override  { return &m_tGuiContext; };
    virtual void Register_Panel(class CBasePanel* pPanel)override;

public:
    virtual bool Set_ProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual _bool UsingUI() { return m_bUsingUI; }

private:
    void Set_Theme();
    void Set_Panel();
    void Render_Frame();
    void Adjust_Alpha(_float dt);
    void GUI_Begin();
    void GUI_End();
    void Test();

private:
    bool m_bActiveGUI = {true};
    bool m_bUsingUI = {false};
    class CGameInstance* m_pGameInstance = { nullptr };
    ImGuiContext* m_pContext = { nullptr };
    GUI_CONTEXT m_tGuiContext = {};
    vector<class CBasePanel*> m_Panels;
    ImGuiIO* m_GuiIo;

public:
    static CGUISystem* Create(const ENGINE_DESC& engine, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;

};

NS_END