#include "pch.h"
#include "UI_EnterBattleButton.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_EnterBattleButton::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_EnterBattleButton::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("button_enterBattle.json")));
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

void CUI_EnterBattleButton::Awake()
{
}

void CUI_EnterBattleButton::Update(_float dt)
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

void CUI_EnterBattleButton::Cache()
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

void CUI_EnterBattleButton::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

_bool CUI_EnterBattleButton::Is_ChildAnimationFinished(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_AnimFinished();
}

CGameObject* CUI_EnterBattleButton::Create()
{
    CUI_EnterBattleButton* pInstance = new CUI_EnterBattleButton();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_EnterBattleButton");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_EnterBattleButton::Clone(INIT_DESC* pArg)
{
    CUI_EnterBattleButton* pInstance = new CUI_EnterBattleButton(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_EnterBattleButton");
        Safe_Release(pInstance);
    }
    return pInstance;
}