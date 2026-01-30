#include "pch.h"
#include "UI_Ramen.h"

#include "GameInstance.h" 
#include "ObjectContainer.h"
#include "ButtonUI.h"
#include "UI_ButtonBack.h"
#include "UI_RamenMenu.h"  

#include "DataBase.h"
#include "FieldSystem.h"

void CUI_Ramen::Select_Menu(CUI_Object* pSelected)
{
    if (m_pSelectedMenu == pSelected || !pSelected)
        return;

    if (m_pSelectedMenu)
        m_pSelectedMenu->UI_DeActive(nullptr);

    m_pSelectedMenu = pSelected;
    m_pSelectedMenu->UI_Active(nullptr);
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
    Create_ButtonBack();
    Create_Menus();


    Set_Alive(false);

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
}

void CUI_Ramen::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

void CUI_Ramen::Create_ButtonBack()
{
    CUI_ButtonBack::BUTTON_DESC* pDesc = new CUI_ButtonBack::BUTTON_DESC;

    auto pContainer = Get_Component<CObjectContainer>();
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_ButtonBack" })
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
        pDesc->onSelect = [this](CUI_Object* pSelected) { Select_Menu(pSelected); };
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

void CUI_Ramen::OnClick_Back()
{
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