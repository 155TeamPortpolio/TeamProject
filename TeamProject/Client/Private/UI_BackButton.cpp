#include "pch.h"
#include "UI_BackButton.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "FieldSystem.h"

HRESULT CUI_BackButton::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_BackButton::Initialize(INIT_DESC* pArg)
{
    BUTTON_DESC* pDesc = static_cast<BUTTON_DESC*>(pArg);
    m_OnClick = pDesc->onClick;

    __super::Initialize(pArg);
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("button_back.json")));
    Cache_Children();

    auto pButton = m_pChildren[ENUM(CHILD::BUTTON)];
    if (pButton)
        pButton->Set_OnClick([this]() { 
        m_isClicked = true;
        Set_ChildAnimation(CHILD::OVERLAY, 0);
        Set_ChildAnimation(CHILD::ICON, 0);
            });

	return S_OK;
}

void CUI_BackButton::Awake()
{
}

void CUI_BackButton::Update(_float dt)
{
    __super::Update(dt);

    if (m_isClicked &&
        Is_ChildAnimationFinished(CHILD::OVERLAY) &&
        Is_ChildAnimationFinished(CHILD::ICON))
    {
        if (m_OnClick)
            m_OnClick();
        else
            FieldSystem()->RequestExitTop();

        m_isClicked = false;
    }

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_BackButton::Cache_Children()
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

void CUI_BackButton::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

_bool CUI_BackButton::Is_ChildAnimationFinished(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_AnimFinished();
}

CGameObject* CUI_BackButton::Create()
{
    CUI_BackButton* pInstance = new CUI_BackButton();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_BackButton");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BackButton::Clone(INIT_DESC* pArg)
{
    CUI_BackButton* pInstance = new CUI_BackButton(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_BackButton");
        Safe_Release(pInstance);
    }
    return pInstance;
}