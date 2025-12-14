#pragma once
#include "AnimationTool_Defines.h"
#include "BasePanel.h"
class CAnimToolPanel
	: CBasePanel
{
private:
    CAnimToolPanel(GUI_CONTEXT* pContext);
    virtual ~CAnimToolPanel() DEFAULT;

public:
    virtual void Update_Panel(_float dt) override;
    virtual void Render_GUI() override;

public:
    static CBasePanel* Create(GUI_CONTEXT* context);
    virtual void Free() override;
};

