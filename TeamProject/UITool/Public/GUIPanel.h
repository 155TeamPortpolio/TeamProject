#pragma once
#include "BasePanel.h"

NS_BEGIN(Engine)
class CGameInstance;
class IProtoService;
class IObjectService;
class IUI_Service;
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

public:
    static CGUIPanel* Create(GUI_CONTEXT* pContext);
    virtual void Free() override;
};

NS_END