#include "pch.h"
#include "UI_RamenMenu.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"
#include "ButtonUI.h"
#include "UI_Ramen.h"
#include "UI_RamenAttributeIcon.h"
#include "UI_RamenAttributeText.h"

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
    m_tRamenDesc = pDesc->tRamenDesc;

    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("ramen_menu.json")));
    Cache();
    Create_AttributeIcons();
    Create_AttributeText();

    if (m_pButton)
        m_pButton->Set_OnClick([this]() {
        if (m_onSelect)
            m_onSelect(this);
            });

    Set_Text(TEXTSLOT::NAME, m_tRamenDesc.strName);
    Set_Text(TEXTSLOT::PRICE, Helper::ConvertToWideString(to_string(m_tRamenDesc.iPrice)));
    auto iter = ICON_MENU_TEXTURES.find(m_tRamenDesc.strID);
    if (iter != ICON_MENU_TEXTURES.end())
        Set_ChildTexture(CHILD::ICON_MENU, iter->second);

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
    Set_ChildAlpha(CHILD::ACTIVE, 1.f);
    m_pAttributeText->Set_Alive(true);
    for (auto& pAttributeIcon: m_AttributeIcons)
        pAttributeIcon->Set_Alive(false);
}

void CUI_RamenMenu::UI_DeActive(void* pArg)
{
    Set_Animation(1);
    Set_ChildAlpha(CHILD::ACTIVE, 0.f);
    m_pAttributeText->Set_Alive(false);
    for (auto& pAttributeIcon : m_AttributeIcons)
        pAttributeIcon->Set_Alive(true);
}

void CUI_RamenMenu::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
    }

    for (_int i = 0; i < ENUM(TEXTSLOT::END); ++i)
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
}

void CUI_RamenMenu::Create_AttributeIcons()
{
    _int iIndex = {};

    for (auto& attribute: m_tRamenDesc.attributes)
    {
        CUI_RamenAttributeIcon::ATTRIBUTE_DESC* pDesc = new CUI_RamenAttributeIcon::ATTRIBUTE_DESC;
        pDesc->strAttributeID = attribute.strAttributeID;

        auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_RamenAttributeIcon" })
            .Add_UIDesc(pDesc)
            .Build("attributeIcon");

        if (!pObj)
            return;

        pObj->Add_AnchorOffsetX( (m_tRamenDesc.attributes.size() - ++iIndex) * - 34.f);
        Get_Component<CObjectContainer>()->Add_Child(pObj);
        m_AttributeIcons.push_back(pObj);
    } 
}

void CUI_RamenMenu::Create_AttributeText()
{
    CUI_RamenAttributeText::ATTRIBUTE_DESC* pDesc = new CUI_RamenAttributeText::ATTRIBUTE_DESC;
    for (auto& attribute : m_tRamenDesc.attributes)
        pDesc->attributes.push_back(attribute);

     auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_RamenAttributeText" })
         .Add_UIDesc(pDesc)
         .Build("attributeText");

     if (!pObj)
         return;

     Get_Component<CObjectContainer>()->Add_Child(pObj);
     m_pAttributeText = pObj;
}

void CUI_RamenMenu::Set_ChildAlive(CHILD child, _bool isAlive)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Alive(isAlive);
}

void CUI_RamenMenu::Set_ChildAlpha(CHILD child, _float fAlpha)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Alpha(fAlpha);
}

void CUI_RamenMenu::Set_ChildTexture(CHILD child, const string& strTextureKey)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    auto pSprite = pChild->Get_Component<CSprite2D>();
    if (!pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

void CUI_RamenMenu::Set_Text(TEXTSLOT text, const _wstring& strText)
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