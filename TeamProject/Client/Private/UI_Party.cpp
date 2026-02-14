#include "pch.h"
#include "UI_Party.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "UI_IconButton.h"
#include "UI_TextButton.h"
#include "UI_PartySynergy.h"
#include "UI_PartyEnterButton.h" 
#include "UI_PartyCard.h"
#include "UI_PartyCardRTDraw.h"
#include "UI_PartyAvatar.h"

#include "UIDirector.h"

HRESULT CUI_Party::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    PrototypeManager()->Add_ProtoType("Scott_Level", "Proto_GameObject_PartySynergy", CUI_PartySynergy::Create());
    PrototypeManager()->Add_ProtoType("Scott_Level", "Proto_GameObject_PartyEnterButton", CUI_PartyEnterButton::Create());
    PrototypeManager()->Add_ProtoType("Scott_Level", "Proto_GameObject_PartyCard", CUI_PartyCard::Create());
    PrototypeManager()->Add_ProtoType("Scott_Level", "Proto_GameObject_PartyCardRTDraw", CUI_PartyCardRTDraw::Create());
    PrototypeManager()->Add_ProtoType("Scott_Level", "Proto_GameObject_PartyAvatar", CUI_PartyAvatar::Create());

    return S_OK;
}

HRESULT CUI_Party::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("party.json")));

    Create_BackButton();
    Create_HomeButton();
    Create_PartySynergy();
    Create_SettingButton();
    Create_BackupButton();
    Create_EnterButton();

    Create_RenderTargets();
    Create_PartyCards();

    //Set_Alive(false);

    return S_OK;
}

void CUI_Party::Awake()
{
}

void CUI_Party::Update(_float dt)
{
    if (InputDevice()->Key_Tap('L'))
    {
        vector<CHARACTER> characters;
        characters.push_back(CHARACTER::Corin);
        characters.push_back(CHARACTER::Miyabi);
        characters.push_back(CHARACTER::JaneDoe);
        UIDirector()->Show_Party(characters);
    } 

    if (InputDevice()->Key_Tap('K'))
    {
        vector<CHARACTER> characters; 
        characters.push_back(CHARACTER::Miyabi);
        characters.push_back(CHARACTER::JaneDoe);
        UIDirector()->Show_Party(characters);
    }

    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_Party::UI_Active(void* pArg)
{
    if (!pArg)
        return;

    UI_PARTY_DESC* pDesc = static_cast<UI_PARTY_DESC*>(pArg);
    if (!pDesc)
        return;

    for (_int i = 0; i < PARTY_COUNT; ++i)
    {
        if (i < pDesc->characters.size())
            m_pPartyCard[i]->Change_Character(pDesc->characters[i]);
        else
            m_pPartyCard[i]->Change_Character(CHARACTER::END);
    }
}

void CUI_Party::Create_BackButton()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_BackButton"})
        .Build("buttonBack");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_Party::Create_HomeButton()
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->strLabel = L"거리";
    pDesc->strTextureKey = "IconMainPage.png";

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_IconButton"})
        .Add_UIDesc(pDesc)
        .Build("buttonHome");

    if (!pObj)
        return;

    pObj->Set_Pivot({});
    pObj->Set_Anchor(ANCHOR::Top | ANCHOR::Left);
    pObj->Set_AnchorOffset(_float2( 148.f, 18.f));

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_Party::Create_PartySynergy()
{
    auto pObj = Builder::Create_UIObject({ "Scott_Level", "Proto_GameObject_PartySynergy"})
        .Build("partySynergy");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_Party::Create_SettingButton()
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->strLabel = L"전투 설정";
    pDesc->strTextureKey = "IconMenu.png";

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_IconButton"})
        .Add_UIDesc(pDesc)
        .Build("buttonHome");

    if (!pObj)
        return;

    pObj->Set_Pivot({ 0.f, 1.f });
    pObj->Set_Anchor(ANCHOR::Bottom | ANCHOR::Left);
    pObj->Set_AnchorOffset(_float2(50.f, - 16.f));

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_Party::Create_BackupButton()
{
    CUI_TextButton::BUTTON_DESC* pDesc = new CUI_TextButton::BUTTON_DESC;
    pDesc->strLabel = L"예비 편성";

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_TextButton"})
        .Add_UIDesc(pDesc)
        .Build("buttonBackup");

    if (!pObj)
        return;

    pObj->Set_Pivot({ 0.f, 1.f });
    pObj->Set_Anchor(ANCHOR::Bottom | ANCHOR::Left);
    pObj->Set_AnchorOffset(_float2(292.f, -16.f));

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_Party::Create_EnterButton()
{
    auto pObj = Builder::Create_UIObject({ "Scott_Level", "Proto_GameObject_PartyEnterButton"})
        .Build("buttonEnter");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_Party::Create_RenderTargets()
{
    // 렌더타겟 생성
    RenderTargetDesc desc = {};
    desc.Width = m_WinSize.x;
    desc.Height = m_WinSize.y;

    for (_int i = 0; i < PARTY_COUNT; ++i)
    {
        m_RenderTargetKeys[i] = "party" + to_string(i);
        desc.Key = m_RenderTargetKeys[i];
        RenderSystem()->Create_RenderTarget(desc);
    } 
}

void CUI_Party::Create_PartyCards()
{
    const _float fWidth = 380.f;
    const _float fSpacing = 18.f;
    const _float fTotalWidth = fWidth * 3 + fSpacing * 2;

    for (_int i = 0; i < PARTY_COUNT; ++i)
    {
        CUI_PartyCard::CARD_DESC* pDesc = new CUI_PartyCard::CARD_DESC;
        pDesc->strRenderTargetKey = m_RenderTargetKeys[i];

        auto pObj = Builder::Create_UIObject({ "Scott_Level", "Proto_GameObject_PartyCard"})
            .Add_UIDesc(pDesc)
            .Build("partyCard");

        if (!pObj)
            continue;

        pObj->Set_Pivot(_float2(0.5f, 0.5f));
        pObj->Set_Anchor(ANCHOR::Center);

        float fStartX = -fTotalWidth * 0.5f + fWidth * 0.5f;
        float fOffsetX = fStartX + i * (fWidth + fSpacing);

        pObj->Set_AnchorOffset(_float2(fOffsetX, 0.f));

        Get_Component<CObjectContainer>()->Add_Child(pObj);
        m_pPartyCard[i] = dynamic_cast<CUI_PartyCard*>(pObj);
    } 
}

CGameObject* CUI_Party::Create()
{
    CUI_Party* pInstance = new CUI_Party();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Party");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Party::Clone(INIT_DESC* pArg)
{
    CUI_Party* pInstance = new CUI_Party(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Party");
        Safe_Release(pInstance);
    }
    return pInstance;
}