#include "pch.h"
#include "UI_BattleLineup.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "UI_IconButton.h"
#include "UI_TextButton.h"
#include "UI_ElementalResonance.h"
#include "UI_EnterBattleButton.h" 
#include "UI_BattleLineupCard.h"
#include "UI_AvatarTest.h"

HRESULT CUI_BattleLineup::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_ElementalResonance", CUI_ElementalResonance::Create());
    PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_EnterBattleButton", CUI_EnterBattleButton::Create());
    PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_BattleLineupCard", CUI_BattleLineupCard::Create());
    PrototypeManager()->Add_ProtoType("Test_Level", "Proto_GameObject_AvatarTest", CUI_AvatarTest::Create());

    return S_OK;
}

HRESULT CUI_BattleLineup::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("battleLineup.json")));

    Create_BackButton();
    Create_HomeButton();
    Create_ElementalResonance();
    Create_BattleSettingButton();
    Create_BackupButton();
    Create_EnterButton();

    Create_BattleLineupCards();

    auto pObj = Builder::Create_Object({ "Test_Level", "Proto_GameObject_AvatarTest"})
        .Build("avatarTest");

    if (pObj)
        ObjectManager()->Add_Object(pObj, {"Test_Level", "UI_Layer"});

    //Set_Alive(false);

    return S_OK;
}

void CUI_BattleLineup::Awake()
{
}

void CUI_BattleLineup::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_BattleLineup::Create_BackButton()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_BackButton" })
        .Build("buttonBack");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_BattleLineup::Create_HomeButton()
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->strLabel = L"거리";
    pDesc->strTextureKey = "IconMainPage.png";

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_IconButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonHome");

    if (!pObj)
        return;

    pObj->Set_Pivot({});
    pObj->Set_Anchor(ANCHOR::Top | ANCHOR::Left);
    pObj->Set_AnchorOffset(_float2( 148.f, 18.f));

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_BattleLineup::Create_ElementalResonance()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_ElementalResonance" })
        .Build("elementalResonance");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_BattleLineup::Create_BattleSettingButton()
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->strLabel = L"전투 설정";
    pDesc->strTextureKey = "IconMenu.png";

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_IconButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonHome");

    if (!pObj)
        return;

    pObj->Set_Pivot({ 0.f, 1.f });
    pObj->Set_Anchor(ANCHOR::Bottom | ANCHOR::Left);
    pObj->Set_AnchorOffset(_float2(50.f, - 16.f));

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_BattleLineup::Create_BackupButton()
{
    CUI_TextButton::BUTTON_DESC* pDesc = new CUI_TextButton::BUTTON_DESC;
    pDesc->strLabel = L"예비 편성";

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_TextButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonBackup");

    if (!pObj)
        return;

    pObj->Set_Pivot({ 0.f, 1.f });
    pObj->Set_Anchor(ANCHOR::Bottom | ANCHOR::Left);
    pObj->Set_AnchorOffset(_float2(292.f, -16.f));

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_BattleLineup::Create_EnterButton()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_EnterBattleButton" })
        .Build("buttonEnter");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_BattleLineup::Create_BattleLineupCards()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_BattleLineupCard" })
        .Build("lineupCard");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

CGameObject* CUI_BattleLineup::Create()
{
    CUI_BattleLineup* pInstance = new CUI_BattleLineup();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_BattleLineup");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BattleLineup::Clone(INIT_DESC* pArg)
{
    CUI_BattleLineup* pInstance = new CUI_BattleLineup(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_BattleLineup");
        Safe_Release(pInstance);
    }
    return pInstance;
}