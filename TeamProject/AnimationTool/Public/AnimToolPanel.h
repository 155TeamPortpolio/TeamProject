#pragma once
#include "AnimationTool_Defines.h"
#include "AnimationLayout.h"
#include "BasePanel.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CAnimator3D;
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
    void GUI_DefaultSetting();
    void Render_Taps(_float fChildHeight);
    // --------------------------------------------------
    void GUI_Setting_Clips(_float fChildHeight);
    void Draw_ToolbarUI();
    void Draw_TimelineUI(float duration, float& ioTime, const char* id);
    // -------------------------------------------------
    void GUI_Create_MetaData(_float fChildHeight);

//Func
public:
    void Update_Panel();
private:
    void Reset_Panel();
    

    /* Create Json MetaData */
    void Load_Clips();
    void Create_Clips(vector<ANIM_CLIP>& pMetaData , const string& ClipTag, const string& FilePath);
    void Create_ClipMeta(const string& CurMetaTag);

private:
    CGameInstance* m_pGameInstance = { nullptr };
   
private: //Create Clip
    class CGameObject*   m_pSelectModel = { nullptr };
    class CAnimator3DEX* m_pSelectAnimator = { nullptr };
    string               m_CurClipTag{};
    vector<string>       m_ClipTags;
    _bool                m_isPlay       = {};
    _bool                m_bLoop        = {};
    _float               m_fCurTime     = {};
    _float               m_fDuration    = {};

private: //Create MetaData
    unordered_map<string, vector<ANIM_CLIP>> m_Meta;
    unordered_map<string, string> m_Paths;

public:
    static CBasePanel* Create(GUI_CONTEXT* context);
    virtual void Free() override;
};

NS_END