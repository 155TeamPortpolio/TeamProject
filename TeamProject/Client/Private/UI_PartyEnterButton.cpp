#include "pch.h"
#include "UI_PartyEnterButton.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_PartyEnterButton::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_PartyEnterButton::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("party_enterButton.json")));
    Cache();

    auto pButton = m_pChildren[ENUM(CHILD::BUTTON)];
    if (pButton)
        pButton->Set_OnClick([this]() {
        m_isClicked = true;
        Set_ChildAnimation(CHILD::OVERLAY, 0);
        Set_ChildAnimation(CHILD::LABEL, 0);
        //Get_Component<CAudioSource>()->Slot(m_strSoundKey).Play();
            });

    return S_OK;
}

void CUI_PartyEnterButton::Awake()
{
}

void CUI_PartyEnterButton::Update(_float dt)
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

void CUI_PartyEnterButton::Cache()
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

void CUI_PartyEnterButton::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

_bool CUI_PartyEnterButton::Is_ChildAnimationFinished(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_AnimFinished();
}

CGameObject* CUI_PartyEnterButton::Create()
{
    CUI_PartyEnterButton* pInstance = new CUI_PartyEnterButton();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_PartyEnterButton");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_PartyEnterButton::Clone(INIT_DESC* pArg)
{
    CUI_PartyEnterButton* pInstance = new CUI_PartyEnterButton(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_PartyEnterButton");
        Safe_Release(pInstance);
    }
    return pInstance;
}