#include "pch.h"
#include "UI_GachaDisplay.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "TextSlot.h"
#include "UI_IconButton.h"

#include "UIDirector.h"

HRESULT CUI_GachaDisplay::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	
	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaDisplay::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha_display.json")));
    Cache();

    Create_SkipButton();

	return S_OK;
}

void CUI_GachaDisplay::Awake()
{
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

    GACHA_DISPLAY_DESC* pDesc = static_cast<GACHA_DISPLAY_DESC*>(pArg);
    switch (pDesc->eType)
    {
    case TYPE::LABEL:
        Set_ChildAnimation(CHILD::BG, 0);
        Set_ChildAnimation(CHILD::LABEL, 0);
        if (m_pLabelTextSlot)
            m_pLabelTextSlot->Set_Text(pDesc->strLabel);
        break;
    case TYPE::SKIP:
        if (m_pSkipButton) m_pSkipButton->Set_Alive(true);
        break;
    } 
}

void CUI_GachaDisplay::UI_DeActive(void* pArg)
{
    if (!pArg)
        return;

    GACHA_DISPLAY_DESC* pDesc = static_cast<GACHA_DISPLAY_DESC*>(pArg);
    switch (pDesc->eType)
    {
    case TYPE::LABEL:
        Set_ChildAnimation(CHILD::BG, 1);
        Set_ChildAnimation(CHILD::LABEL, 1);
        break;
    case TYPE::SKIP:
        if (m_pSkipButton) m_pSkipButton->Set_Alive(false);
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

void CUI_GachaDisplay::Create_SkipButton()
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->onClick = [this]() { }; // 스킵 버튼 눌렀을 때 호출할 함수 채워야
    pDesc->strLabel = L"건너뛰기";
    pDesc->strTextureKey = "IconSkip.png";
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