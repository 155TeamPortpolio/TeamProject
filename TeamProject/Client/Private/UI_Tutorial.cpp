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

    PrototypeManager()->Add_ProtoType("Scott_Level", "Proto_GameObject_TutorialDescription", CUI_TutorialDescription::Create());
    PrototypeManager()->Add_ProtoType("Scott_Level", "Proto_GameObject_TutorialVideo", CUI_TutorialVideo::Create());

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

    m_isCheck = false;
    Set_Alive(true);
    Set_Animation(0);
    Change_Description(pDesc->eTutorial);

    UIDirector()->Show_Mouse();
}

void CUI_Tutorial::UI_DeActive(void* pArg)
{
    m_isCheck = true;
    Set_Animation(1);
    if (m_pVideo)
        m_pVideo->UI_DeActive();

    UIDirector()->Hide_Mouse();
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

    pObj->Set_OnClick([this]() {});
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

    pObj->Set_OnClick([this]() { });
    Get_Component<CObjectContainer>()->Add_Child(pObj);

    return S_OK;
}

HRESULT CUI_Tutorial::Create_TutorialDescriptions()
{
    for (_int i = 0; i < ENUM(TUTORIAL::END); ++i)
    {
        TUTORIAL eTutorial = static_cast<TUTORIAL>(i);
        CUI_TutorialDescription::TUTORIAL_DESC* pDesc = new CUI_TutorialDescription::TUTORIAL_DESC;
        pDesc->eTutorial = eTutorial;

        auto pObj = Builder::Create_UIObject({ "Scott_Level", "Proto_GameObject_TutorialDescription"})
            .Add_UIDesc(pDesc)
            .Build("description");

        if (!pObj)
            return E_FAIL;

        Get_Component<CObjectContainer>()->Add_Child(pObj);
        m_Descriptions.emplace(eTutorial, pObj);
    } 

    return S_OK;
}

HRESULT CUI_Tutorial::Create_TutorialVideo()
{
    auto pObj = Builder::Create_UIObject({ "Scott_Level", "Proto_GameObject_TutorialVideo"})
        .Build("video");

    if (!pObj)
        return E_FAIL;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pVideo = dynamic_cast<CUI_TutorialVideo*>(pObj);

    return S_OK;
}

void CUI_Tutorial::Change_Description(TUTORIAL eTutorial)
{
    for (auto& pair : m_Descriptions)
        pair.second->Set_Alpha(0.f);

    auto iter = m_Descriptions.find(eTutorial);
    if (iter == m_Descriptions.end())
        return;

    iter->second->Set_Alpha(1.f);

    Change_TitleText(eTutorial);

    if (m_pVideo) 
        m_pVideo->Play(eTutorial);
}

void CUI_Tutorial::Change_TitleText(TUTORIAL eTutorial)
{
    if (!m_pTitle)
        return;

    m_pTitle->Set_Text(Get_TitleText(eTutorial));
}

wstring CUI_Tutorial::Get_TitleText(TUTORIAL eTutorial)
{
    switch (eTutorial)
    {
    case TUTORIAL::EXTREME_EVADE: return L"극한 회피 및 회피 반격";
    case TUTORIAL::EXTREME_SUPPORT: return L"극한 지원 및 지원 돌격";
    case TUTORIAL::DECIBEL_ULTIMATE: return L"데시벨 및 궁극기";
    case TUTORIAL::GROGGY_COMBO: return  L"그로기 수치 및 콤보 스킬";
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