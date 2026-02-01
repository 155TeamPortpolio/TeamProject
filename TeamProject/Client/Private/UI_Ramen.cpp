#include "pch.h"
#include "UI_Ramen.h"

#include "GameInstance.h" 
#include "ObjectContainer.h"
#include "TextSlot.h"
#include "ButtonUI.h"
#include "UI_BackButton.h"
#include "UI_RamenMenu.h"  
#include "UI_RamenOrderBanner.h"
#include "UI_RamenVideo.h"
#include "UI_RamenResultBanner.h"

#include "DataBase.h"
#include "FieldSystem.h"

void CUI_Ramen::Select_Menu(CUI_Object* pSelected, _int iPrice, wstring strMenu)
{
    if (m_pSelectedMenu == pSelected || !pSelected)
        return;

    if (m_pSelectedMenu)
        m_pSelectedMenu->UI_DeActive(nullptr);

    m_pSelectedMenu = pSelected;
    m_pSelectedMenu->UI_Active(nullptr);

    Set_TextPrice(m_iMoney, iPrice);
    replace(strMenu.begin(), strMenu.end(), L'\n', L' ');
    m_strMenu = strMenu; 
}

HRESULT CUI_Ramen::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_Ramen::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("ramen.json")));
    Create_BackButton();
    Create_Menus();
    Create_OrderBanner();
    Create_Video();
    Create_ResultBanner();

    Cache();

    if (m_pButtonOrder)
        m_pButtonOrder->Set_OnClick([this]() { OnClick_Order(); });

    Set_Alive(false);
    Set_TextPrice(12345, 0);

	return S_OK;
}

void CUI_Ramen::Awake()
{
}

void CUI_Ramen::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_Ramen::Late_Update(_float dt)
{
}

void CUI_Ramen::UI_Active(void* pArg)
{
    Set_Alive(true); 
    Set_ChildAnimation(CHILD::ORDER, 0);
    m_pSelectedMenu = nullptr;
    for (auto& pMenu : m_pMenus)
        pMenu->UI_DeActive();
    Update_Affordable();
}

void CUI_Ramen::UI_DeActive(void* pArg)
{
    Set_ChildAnimation(CHILD::ORDER, 1);
    Set_Alive(false);
}

void CUI_Ramen::Create_BackButton()
{
    CUI_BackButton::BUTTON_DESC* pDesc = new CUI_BackButton::BUTTON_DESC;

    auto pContainer = Get_Component<CObjectContainer>();
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_BackButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonBack");

    if (!pObj)
        return;

    pContainer->Add_Child(pObj);
}

void CUI_Ramen::Create_Menus()
{
    const _float fSpacingX = 160.f;
    const _float fCenterY = 784.f;
    _float fCenterIndex = (MAX_MENU_COUNT - 1) * 0.5f;

    auto vecRamenTable = CDataBase::GetInstance()->GetRamenTable();
    _int iMenuCount = min(MAX_MENU_COUNT, vecRamenTable.size());

    auto pContainer = Get_Component<CObjectContainer>();
    for (_int i = 0; i < iMenuCount; ++i)
    {
        CUI_RamenMenu::RAMENMENU_DESC* pDesc = new CUI_RamenMenu::RAMENMENU_DESC;
        pDesc->onSelect = [this](CUI_Object* pSelected, _int iPrice, wstring strMenu) { Select_Menu(pSelected, iPrice, strMenu); };
        pDesc->tRamenDesc = *vecRamenTable[i];

        auto pMenu = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_RamenMenu" })
            .Add_UIDesc(pDesc)
            .Build("menu" + to_string(i));

        if (!pMenu)
            continue;

        pMenu->Set_AnchorOffset({ m_WinSize.x * 0.5f + fSpacingX * (i - fCenterIndex), fCenterY });
        pContainer->Add_Child(pMenu);
        m_pMenus[i] = pMenu;
    }
}

void CUI_Ramen::Create_OrderBanner()
{
    CUI_RamenOrderBanner::ORDER_BANNER_DESC* pDesc = new CUI_RamenOrderBanner::ORDER_BANNER_DESC;
    pDesc->onOrderComfirm = [this]() { OnClick_OrderComfirm();  };

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_RamenOrderBanner" })
        .Add_UIDesc(pDesc)
        .Build("orderBanner");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pOrderBanner = pObj;
}

void CUI_Ramen::Create_Video()
{
    CUI_RamenVideo::VIDEO_DESC* pDesc = new  CUI_RamenVideo::VIDEO_DESC;
    pDesc->onVideoFinished = [this]() { OnVideoFinished();  };

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_RamenVideo" })
        .Add_UIDesc(pDesc)
        .Build("video");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pVideo = pObj;
}

void CUI_Ramen::Create_ResultBanner()
{
    CUI_RamenResultBanner::RESULT_BANNER_DESC* pDesc = new CUI_RamenResultBanner::RESULT_BANNER_DESC;
    pDesc->onClickConfirm = [this]() { OnClick_ResultConfirm();  };

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_RamenResultBanner" })
        .Add_UIDesc(pDesc)
        .Build("resultBanner");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pResultBanner = pObj;
}

void CUI_Ramen::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
    }

    {
        auto pObj = m_pChildren[ENUM(CHILD::TEXT_PRICE)];
        if (pObj)
            m_pTextPrice = pObj->Get_Component<CTextSlot>();
    } 

    {
        auto pObj = m_pChildren[ENUM(CHILD::BTN_ORDER)];
        m_pButtonOrder = dynamic_cast<CButtonUI*>(pObj);
    }
}

void CUI_Ramen::OnClick_Order()
{
    if (m_isAffordable)
    {
        Set_ChildAnimation(CHILD::TEXT_ORDER, 0);
        Set_ChildAnimation(CHILD::CLICK_ORDER, 0);
        if (m_pOrderBanner)
        {
            CUI_RamenOrderBanner::ACTIVE_DESC desc = {};
            desc.strMenu = m_strMenu;
            m_pOrderBanner->UI_Active(&desc);
        } 
    }
    else
    {
        MSG_BOX("구매 불가능!");
    }    
}

void CUI_Ramen::OnClick_OrderComfirm()
{
    if (m_pVideo)
        m_pVideo->UI_Active();
}

void CUI_Ramen::OnVideoFinished()
{
    if (m_pResultBanner)
    {
        CUI_RamenResultBanner::ACTIVE_DESC desc = {};
        desc.strMenu = m_strMenu;
        m_pResultBanner->UI_Active(&desc);
    } 
}

void CUI_Ramen::OnClick_ResultConfirm()
{
}

void CUI_Ramen::Set_TextPrice(_int iMoney, _int iPrice)
{
    m_iMoney = iMoney;
    m_iPrice = iPrice;

    m_pTextPrice->Set_Text(Helper::ConvertToWideString(to_string(m_iMoney) + "/" + to_string(iPrice)));
    Update_Affordable();
}

void CUI_Ramen::Update_Affordable()
{
    if (m_iMoney >= m_iPrice && m_pSelectedMenu)
    {
        m_isAffordable = true;
        Set_ChildAnimation(CHILD::DISABLE_ORDER, 1);
    }
    else
    {
        m_isAffordable = false;
        Set_ChildAnimation(CHILD::DISABLE_ORDER, 0);
    } 
}

void CUI_Ramen::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

CGameObject* CUI_Ramen::Create()
{
    CUI_Ramen* pInstance = new CUI_Ramen();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Ramen");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Ramen::Clone(INIT_DESC* pArg)
{
    CUI_Ramen* pInstance = new CUI_Ramen(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Ramen");
        Safe_Release(pInstance);
    }
    return pInstance;
}