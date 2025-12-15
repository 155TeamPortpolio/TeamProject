#pragma once
#include "AnimationTool_Defines.h"
#include "AnimationLayout.h"
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

//GUI
private:
    void GUI_Setting_Clips(_float fChildHeight);

//Func
private:
    void Load_Clips();
    void Set_Aniation();
    void Create_AnimatorFromAnim();
    void Create_ClipMeta();

private:
    CGameInstance* m_pGameInstance = { nullptr };
    //<애니매이션이름, { 클립이름, 이벤트 { 타이밍, 타입, 태그 } }>
    unordered_map<string, vector<ANIM_CLIP>> m_Clips;

public:
    static CBasePanel* Create(GUI_CONTEXT* context);
    virtual void Free() override;
};

NS_END