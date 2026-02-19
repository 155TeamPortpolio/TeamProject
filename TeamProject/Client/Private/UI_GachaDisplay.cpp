#include "pch.h"
#include "UI_GachaDisplay.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "TextSlot.h"
#include "UI_IconButton.h"
#include "UI_GachaVideo.h"

HRESULT CUI_GachaDisplay::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;
	
	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaDisplay::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    DISPLAY_INIT_DESC* pDesc = static_cast<DISPLAY_INIT_DESC*>(pArg);
    m_eGrade = pDesc->eGrade;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha_display.json")));
    Cache();

    Create_Video(pDesc->onVideoFinished);
    Create_SkipButton(pDesc->onClickSkip);

	return S_OK;
}

void CUI_GachaDisplay::Awake()
{
    if (m_pVideo)
        m_pVideo->Play_Video(m_eGrade);
}

void CUI_GachaDisplay::Update(_float dt)
{
	__super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaDisplay::UI_Active(void* pArg)
{
    if (!pArg)
        return;

    DISPLAY_STATE_DESC* pDesc = static_cast<DISPLAY_STATE_DESC*>(pArg);
    switch (pDesc->eType)
    {
    case TYPE::LABEL:
        if (m_isLabelVisible)
            return;

        m_isLabelVisible = true;
        Set_ChildAnimation(CHILD::BG, 0);
        Set_ChildAnimation(CHILD::LABEL, 0);
        if (m_pLabelTextSlot)
            m_pLabelTextSlot->Set_Text(pDesc->strLabel);
        break;
    case TYPE::SKIP:
        if (m_pSkipButton)
            m_pSkipButton->Set_Alive(true);
        break;
    } 
}

void CUI_GachaDisplay::UI_DeActive(void* pArg)
{
    if (!pArg)
        return;

    DISPLAY_STATE_DESC* pDesc = static_cast<DISPLAY_STATE_DESC*>(pArg);
    switch (pDesc->eType)
    {
    case TYPE::LABEL:
        if (!m_isLabelVisible)
            return;

        m_isLabelVisible = false;
        Set_ChildAnimation(CHILD::BG, 1);
        Set_ChildAnimation(CHILD::LABEL, 1);
        break;
    case TYPE::SKIP:
        if (m_pSkipButton) 
            m_pSkipButton->Set_Alive(false);
        break;
    } 
}

void CUI_GachaDisplay::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);

        if (i == ENUM(CHILD::LABEL))
            m_pLabelTextSlot = pObj->Get_Component<CTextSlot>();
    }
}

void CUI_GachaDisplay::Create_Video(function<void()> onVideoFinished)
{
    PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaVideo", CUI_GachaVideo::Create());

    auto pObj = Builder::Create_UIObject({ "Gacha_Level", "Proto_GameObject_GachaVideo"})
        .Build("video");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pVideo = dynamic_cast<CUI_GachaVideo*>(pObj);
    m_pVideo->Set_OnVideoFinished(onVideoFinished);
}

void CUI_GachaDisplay::Create_SkipButton(function<void()> onClickSkip)
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->onClick = [onClickSkip]() { 
        if (onClickSkip) 
            onClickSkip(); 
        };
    pDesc->strLabel = L"°Ç³Ê¶Ù±â";
    pDesc->strTextureKey = "IconSkip.png";
    pDesc->strSoundKey = "UI_Tick.wav";
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_IconButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonSkip");

    if (!pObj)
        return;

    pObj->Set_Anchor(ANCHOR::Right | ANCHOR::Top);
    pObj->Set_AnchorOffset({ -140.f, 60.f });
    pObj->Set_Alive(false);
    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pSkipButton = pObj;
}

void CUI_GachaDisplay::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

CGameObject* CUI_GachaDisplay::Create()
{
    CUI_GachaDisplay* pInstance = new CUI_GachaDisplay();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaDisplay");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaDisplay::Clone(INIT_DESC* pArg)
{
    CUI_GachaDisplay* pInstance = new CUI_GachaDisplay(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaDisplay");
        Safe_Release(pInstance);
    }
    return pInstance;
}