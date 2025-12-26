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
    struct SEQ_SEGMENT
    {
        int   ClipIndex;   // 어떤 클립인지
        float Duration;    // 클립 길이
        float BlendTime;   // 다음으로 넘어갈 때 보간
    };
    vector<SEQ_SEGMENT> m_Sequence;
    int m_iSelectSegment = -1;

private:
    CAnimToolPanel(GUI_CONTEXT* pContext);
    virtual ~CAnimToolPanel() DEFAULT;

public:
    virtual void Update_Panel(_float dt) override;
    virtual void Render_GUI() override;

//GUI
private:
    void GUI_DefaultSetting();
    // -----------------------------------------------
    void GUI_Setting_Clips(_float fChildHeight);
    void Draw_ToolbarUI(); 
    void Draw_TimelineUI(float duration, float& ioTime, const char* id);
    void Draw_EventListUI();
    // -----------------------------------------------
    void GUI_Preview(_float fChildHeight);
    // -------------------------------------------------
    void GUI_Setting_Effect(_float fChildHeight); // << 이 칸 안에서 작업하고 클래스 추가하면 댐
    // -------------------------------------------------
    void GUI_Create_MetaData(_float fChildHeight);

    enum class PANELTYPE { CLIP, PREVIEW, RESOURCE };
    PANELTYPE m_ePanelType = { PANELTYPE::CLIP };

//Func
public:
    void Setting_NewClip();

private:
    void Reset_Panel();
    void Add_Event();
    void Save_Event();
    ImU32 GetEventColor(CLIP_EVENT_TYPE eType);
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
    int                  m_iCurClipIndex = { -1 };
    vector<ANIM_CLIP>    m_AnimClip;

    //전부 디버그용 표시여야함 
    _bool   m_bPause = { true };
    _bool   m_bLoop = { true };
    _float  m_fPlaySpeed = { 1.f };
    _float  m_fTickPerSec = { 1.f };
    _float  m_fTrackPos   = {};
    _float  m_fDuration  = {};
    
    vector<string> m_PreviewList;
    _bool m_bPreviewPlay = false;
    _int m_iCurrentPrevIndex = 0;
    
private: //Create MetaData
    unordered_map<string, vector<ANIM_CLIP>> m_Meta;
    unordered_map<string, string> m_Paths;

public:
    static CBasePanel* Create(GUI_CONTEXT* context);
    virtual void Free() override;
};

NS_END