#include "pch.h"
#include "UI_RamenMenu.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"
#include "ButtonUI.h"
#include "UI_Ramen.h"

HRESULT CUI_RamenMenu::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_RamenMenu::Initialize(INIT_DESC* pArg)
{
    RAMENMENU_DESC* pDesc = static_cast<RAMENMENU_DESC*>(pArg);
    m_onSelect = pDesc->onSelect;

    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("ramen_menu.json")));
    Cache();

    auto pSprite = m_pSprites[ENUM(SPRITE::BUTTON)];
    if (pSprite)
    {
        pSprite->Change_Texture(0, G_GlobalLevelKey, "RamenBg0.png");
        pSprite->Change_Texture(1, G_GlobalLevelKey, "RamenBg1.png");
    }

    if (m_pButton)
        m_pButton->Set_OnClick([this]() {
        if (m_onSelect)
            m_onSelect(this);
            });

    UI_DeActive(nullptr);

	return S_OK;
}

void CUI_RamenMenu::Awake()
{
}

void CUI_RamenMenu::Update(_float dt)
{
	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_RamenMenu::Late_Update(_float dt)
{
}

void CUI_RamenMenu::UI_Active(void* pArg)
{
    Set_Animation(0);
    Change_Sprite(SPRITE::BUTTON, 1);
    if (m_pOverlay) m_pOverlay->Set_Alive(true);
}

void CUI_RamenMenu::UI_DeActive(void* pArg)
{
    Set_Animation(1);
    Change_Sprite(SPRITE::BUTTON, 0);
    if (m_pOverlay) m_pOverlay->Set_Alive(false);
}

void CUI_RamenMenu::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(SPRITE::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(SPRITE_INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pSprites[i] = pObj->Get_Component<CSprite2D>();
    }

    for (_int i = 0; i < ENUM(TEXT::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(TEXT_INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pTexts[i] = pObj->Get_Component<CTextSlot>();
    }

    {
        auto pObj = pContainer->Find_Descendant("button");
        if (!pObj)
            return;

        m_pButton = dynamic_cast<CButtonUI*>(pObj);
    } 

    {
        auto pObj = pContainer->Find_Descendant("overlay");
        if (!pObj)
            return;
        m_pOverlay = dynamic_cast<CUI_Object*>(pObj);
    }
}

void CUI_RamenMenu::Change_Sprite(SPRITE sprite, _uint iIndex)
{
    auto pSprite = m_pSprites[ENUM(sprite)];
    if (!pSprite)
        return;

    pSprite->ChangeSprite(iIndex);
}

void CUI_RamenMenu::Set_Text(TEXT text, const _wstring& strText)
{
    auto pText = m_pTexts[ENUM(text)];
    if (!pText)
        return;

    pText->Set_Text(strText);
}

CGameObject* CUI_RamenMenu::Create()
{
    CUI_RamenMenu* pInstance = new CUI_RamenMenu();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_RamenMenu");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_RamenMenu::Clone(INIT_DESC* pArg)
{
    CUI_RamenMenu* pInstance = new CUI_RamenMenu(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_RamenMenu");
        Safe_Release(pInstance);
    }
    return pInstance;
}