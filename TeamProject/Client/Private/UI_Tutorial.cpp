#include "pch.h"
#include "UI_Tutorial.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "TextSlot.h"

#include "UI_ButtonPanel.h"
#include "UI_TutorialDescription.h"
#include "UI_TutorialVideo.h"

#include "UIDirector.h"

HRESULT CUI_Tutorial::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

    Add_Component<CObjectContainer>();

    PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_TutorialDescription", CUI_TutorialDescription::Create());
    PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_TutorialVideo", CUI_TutorialVideo::Create());

	return S_OK;
}

HRESULT CUI_Tutorial::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("tutorial.json")));
    Cache();

    Create_ExitButton();
    Create_EnterButton();
    Create_TutorialDescriptions();
    Create_TutorialVideo();

	return S_OK;
}

void CUI_Tutorial::Awake()
{
    Set_Alive(false);
}

void CUI_Tutorial::Update(_float dt)
{
    __super::Update(dt);
    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (m_isCheck && Is_AnimFinished())
        Set_Alive(false);
}

void CUI_Tutorial::UI_Active(void* pArg)
{
    if (!pArg)
        return;
     
    TUTORIAL_DESC* pDesc = static_cast<TUTORIAL_DESC*>(pArg);
    m_eType = pDesc->eType;

    m_isCheck = false; 
    Set_Animation(0);
    Set_Alive(true);
    Change_Description(m_eType);
    
    UIDirector()->Show_Mouse();
    GameInstance()->Set_EngineTimeScale(0.f);
}

void CUI_Tutorial::UI_DeActive(void* pArg)
{
    m_isCheck = true;
    Set_Animation(1);
    if (m_pVideo)
        m_pVideo->UI_DeActive();

    UIDirector()->Hide_Mouse();
    GameInstance()->Set_EngineTimeScale(1.f);

    TUTORIAL_DESC desc = {};
    desc.eType = m_eType;
    desc.eState = TUTORIAL_STATE::PLAY;
    EventSystem()->Broadcast<TUTORIAL_DESC>({ desc });
}

void CUI_Tutorial::Cache()
{
    auto pTitle = Get_Component<CObjectContainer>()->Find_Descendant("title");
    if (pTitle)
        m_pTitle = pTitle->Get_Component<CTextSlot>();
}

HRESULT CUI_Tutorial::Create_ExitButton()
{
    CUI_ButtonPanel::BUTTON_DESC* pDesc = new CUI_ButtonPanel::BUTTON_DESC;
    pDesc->strJsonKey = "tutorial_exitButton.json";

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_ButtonPanel"})
        .Add_UIDesc(pDesc)
        .Build("buttonExit");

    if (!pObj)
        return E_FAIL;

    Get_Component<CObjectContainer>()->Add_Child(pObj);

    return S_OK;
}

HRESULT CUI_Tutorial::Create_EnterButton()
{
    CUI_ButtonPanel::BUTTON_DESC* pDesc = new CUI_ButtonPanel::BUTTON_DESC;
    pDesc->strJsonKey = "tutorial_enterButton.json";

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_ButtonPanel"})
        .Add_UIDesc(pDesc)
        .Build("buttonEnter");

    if (!pObj)
        return E_FAIL;

    pObj->Set_OnClick([this]() { UI_DeActive(); });
    Get_Component<CObjectContainer>()->Add_Child(pObj);

    return S_OK;
}

HRESULT CUI_Tutorial::Create_TutorialDescriptions()
{
    for (_int i = 0; i < ENUM(TUTORIAL_TYPE::END); ++i)
    {
        TUTORIAL_TYPE eType = static_cast<TUTORIAL_TYPE>(i);
        CUI_TutorialDescription::TUTORIAL_DESC* pDesc = new CUI_TutorialDescription::TUTORIAL_DESC;
        pDesc->eType = eType;

        auto pObj = Builder::Create_UIObject({ "Tutorial_Level", "Proto_GameObject_TutorialDescription"})
            .Add_UIDesc(pDesc)
            .Build("description");

        if (!pObj)
            return E_FAIL;

        Get_Component<CObjectContainer>()->Add_Child(pObj);
        m_Descriptions.emplace(eType, pObj);
    } 

    return S_OK;
}

HRESULT CUI_Tutorial::Create_TutorialVideo()
{
    auto pObj = Builder::Create_UIObject({ "Tutorial_Level", "Proto_GameObject_TutorialVideo"})
        .Build("video");

    if (!pObj)
        return E_FAIL;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pVideo = dynamic_cast<CUI_TutorialVideo*>(pObj);

    return S_OK;
}

void CUI_Tutorial::Change_Description(TUTORIAL_TYPE eType)
{
    for (auto& pair : m_Descriptions)
        pair.second->Set_Alpha(0.f);

    auto iter = m_Descriptions.find(eType);
    if (iter == m_Descriptions.end())
        return;

    iter->second->Set_Alpha(1.f);

    Change_TitleText(eType);

    if (m_pVideo) 
        m_pVideo->Play(eType);
}

void CUI_Tutorial::Change_TitleText(TUTORIAL_TYPE eType)
{
    if (!m_pTitle)
        return;

    m_pTitle->Set_Text(Get_TitleText(eType));
}

wstring CUI_Tutorial::Get_TitleText(TUTORIAL_TYPE eType)
{
    switch (eType)
    {
    case TUTORIAL_TYPE::EXTREME_EVADE: return L"극한 회피 및 회피 반격";
    case TUTORIAL_TYPE::EXTREME_SUPPORT: return L"극한 지원 및 지원 돌격";
    case TUTORIAL_TYPE::DECIBEL_ULTIMATE: return L"데시벨 및 궁극기";
    case TUTORIAL_TYPE::GROGGY_COMBO: return  L"그로기 수치 및 콤보 스킬";
    default: return  L"제목";
    }
}

CGameObject* CUI_Tutorial::Create()
{
    CUI_Tutorial* pInstance = new CUI_Tutorial();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Tutorial");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Tutorial::Clone(INIT_DESC* pArg)
{
    CUI_Tutorial* pInstance = new CUI_Tutorial(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Tutorial");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Tutorial::Free()
{
    __super::Free();

    m_Descriptions.clear();
}