#include "pch.h"
#include "UI_ButtonPanel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "AudioSource.h"

HRESULT CUI_ButtonPanel::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();
    Add_Component<CAudioSource>();
    Get_Component<CAudioSource>()->SoundFolder(G_GlobalLevelKey, "../Bin/Resources/Global/UI/Sound/");

    return S_OK;
}

HRESULT CUI_ButtonPanel::Initialize(INIT_DESC* pArg)
{
    BUTTON_DESC* pDesc = static_cast<BUTTON_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath(pDesc->strJsonKey)));// "tutorial_enterButton.json")));
    Cache();

    auto pButton = m_pChildren[ENUM(CHILD::BUTTON)];
    if (pButton)
        pButton->Set_OnClick([this]() {
        m_isClicked = true;
        Set_ChildAnimation(CHILD::OVERLAY, 0);
        Set_ChildAnimation(CHILD::LABEL, 0);
        Get_Component<CAudioSource>()->Slot("UI_Tick.wav").Play();
            });

    return S_OK;
}

void CUI_ButtonPanel::Awake()
{
}

void CUI_ButtonPanel::Update(_float dt)
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

void CUI_ButtonPanel::Cache()
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

void CUI_ButtonPanel::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

_bool CUI_ButtonPanel::Is_ChildAnimationFinished(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_AnimFinished();
}

CGameObject* CUI_ButtonPanel::Create()
{
    CUI_ButtonPanel* pInstance = new CUI_ButtonPanel();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_ButtonPanel");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_ButtonPanel::Clone(INIT_DESC* pArg)
{
    CUI_ButtonPanel* pInstance = new CUI_ButtonPanel(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_ButtonPanel");
        Safe_Release(pInstance);
    }
    return pInstance;
}