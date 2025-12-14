#pragma once
#include "AnimationTool_Defines.h"
#include "BasePanel.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(AnimTool)

class CAnimToolPanel
	: CBasePanel
{
private:
    CAnimToolPanel(GUI_CONTEXT* pContext);
    virtual ~CAnimToolPanel() DEFAULT;

public:
    virtual void Update_Panel(_float dt) override;
    virtual void Render_GUI() override;

private:
    void Load_Clips();

private:
    CGameInstance* m_pGameInstance = { nullptr };
    unordered_map<string, string> m_Clips; //클립묶음 이름, 실제 클립 태그

public:
    static CBasePanel* Create(GUI_CONTEXT* context);
    virtual void Free() override;
};

NS_END