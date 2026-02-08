#include "pch.h"
#include "UI_TextButton.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"

HRESULT CUI_TextButton::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_TextButton::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("button_text.json")));
    Cache_Children();

    if (!pArg)
        return S_OK;

    CUI_TextButton::BUTTON_DESC* pDesc = dynamic_cast<CUI_TextButton::BUTTON_DESC*>(pArg);
    if (!pDesc)
        return S_OK;

    m_OnClick = pDesc->onClick;

    auto pText = m_pChildren[ENUM(CHILD::LABEL)];
    if (pText)
        if (auto pTextSlot = pText->Get_Component<CTextSlot>())
            pTextSlot->Set_Text(pDesc->strLabel);

    auto pButton = m_pChildren[ENUM(CHILD::BUTTON)];
    if (pButton)
        pButton->Set_OnClick([this]() {
        m_isClicked = true;
        Set_ChildAnimation(CHILD::OVERLAY, 0);
        Set_ChildAnimation(CHILD::LABEL, 0);
            });

    return S_OK;
}

void CUI_TextButton::Awake()
{
}

void CUI_TextButton::Update(_float dt)
{
    __super::Update(dt);

    if (m_isClicked &&
        Is_ChildAnimationFinished(CHILD::OVERLAY))
    {
        if (m_OnClick)
            m_OnClick();

        m_isClicked = false;
    }

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_TextButton::Cache_Children()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
    }
}

void CUI_TextButton::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

_bool CUI_TextButton::Is_ChildAnimationFinished(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_AnimFinished();
}

CGameObject* CUI_TextButton::Create()
{
    CUI_TextButton* pInstance = new CUI_TextButton();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_TextButton");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_TextButton::Clone(INIT_DESC* pArg)
{
    CUI_TextButton* pInstance = new CUI_TextButton(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_TextButton");
        Safe_Release(pInstance);
    }
    return pInstance;
}