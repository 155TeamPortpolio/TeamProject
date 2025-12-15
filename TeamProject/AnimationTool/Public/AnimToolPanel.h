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
    void Create_Clips(vector<ANIM_CLIP>& pMetaData , const string& ClipTag, const string& FilePath);
    void Create_ClipMeta(const string& CurMetaTag);

private:
    void Set_Animation();


private:
    CGameInstance* m_pGameInstance = { nullptr };
    //<데이터이름, { 클립이름, 이벤트 { 타이밍, 타입, 태그 } }>
    // 데이터이름 + _Ani_ + 클립이름 + .anim = 파일
    //ex) Avatar_Female_Size02_Unagi / _Ani_ / Attack_ChargeAttack_Start_Front / .anim
    // (데이터이름 + _Ani_ + 클립이름) << 리소스 키였으면 함
    unordered_map<string, vector<ANIM_CLIP>> m_Meta;
    unordered_map<string, string> m_Paths;
public:
    static CBasePanel* Create(GUI_CONTEXT* context);
    virtual void Free() override;
};

NS_END