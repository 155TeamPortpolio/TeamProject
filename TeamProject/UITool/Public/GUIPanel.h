#pragma once
#include "BasePanel.h"

NS_BEGIN(Engine)
class CGameInstance; class CGameObject; class CUI_Object; class IProtoService; class IObjectService; class IUI_Service; 
NS_END

NS_BEGIN(UITool)

class CGUIPanel final : public CBasePanel
{
private:
    CGUIPanel(GUI_CONTEXT* pContext);
    virtual ~CGUIPanel() = default;

public:
    virtual void Update_Panel(_float dt) override;
    virtual void Render_GUI() override;

private:
    CGameInstance*      m_pGameInstance = { nullptr };

private:
    void Render_GUI_CreateCanvasPanel(); 
    void Render_GUI_OpenPrefab();
    void Render_GUI_AddPrefab();
    void Render_GUI_CanvasPanel();

    CUI_Object* LoadPrefab();
    void SavePrefab(class CUIObject_Tool* pObj);

    CGameObject* Get_SelectedObject();

public:
    static CGUIPanel* Create(GUI_CONTEXT* pContext) { return new CGUIPanel(pContext); }
    virtual void Free() override { __super::Free(); }
};

NS_END