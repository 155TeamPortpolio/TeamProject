#include "pch.h"
#include "UI_TutorialGuideSlot.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "TextSlot.h"

HRESULT CUI_TutorialGuideSlot::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

    return S_OK;
}

HRESULT CUI_TutorialGuideSlot::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    SLOT_DESC* pDesc = static_cast<SLOT_DESC*>(pArg);
    m_eAction = pDesc->desc.eAction;
    m_iTargetCount = pDesc->desc.iCount; 

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath(Get_PrefabPath(m_eAction))));

    Cache();

    if(pDesc->desc.eAction != TUTORIAL_ACTION::END)
        Set_CountText();

    // ¿Ã∫•∆Æ : TUTORIAL_DESC
    Get_Component<CEventListener>()->Add_Listener<TUTORIAL_ACTION_DESC>([this](const TUTORIAL_ACTION_DESC& desc)
        {
            if (desc.eAction != m_eAction)
                return;

            if (m_iCurrentCount >= m_iTargetCount)
                return;

            m_iCurrentCount++;
            Set_CountText();

            if (m_iCurrentCount >= m_iTargetCount)
            {
                TUTORIAL_ACTION_COMPLETE desc = {};
                desc.eAction = m_eAction;
                EventSystem()->Broadcast<TUTORIAL_ACTION_COMPLETE>({ desc });
            }
        });

    return S_OK;
}

void CUI_TutorialGuideSlot::Awake()
{
}

void CUI_TutorialGuideSlot::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_TutorialGuideSlot::UI_Active(void* pArg)
{
    Set_Alive(true);
    Set_Animations(0);
}

void CUI_TutorialGuideSlot::UI_DeActive(void* pArg)
{
    if (!Is_Alive())
        return;

    Set_Animations(1);
}

void CUI_TutorialGuideSlot::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pCount = pContainer->Find_Descendant("count");
    if (pCount)
        m_pCountText = pCount->Get_Component<CTextSlot>();
}

void CUI_TutorialGuideSlot::Set_CountText()
{
    if (!m_pCountText)
        return;

    wstring strText = Helper::ConvertToWideString(to_string(m_iCurrentCount) + "/" + to_string(m_iTargetCount));
    m_pCountText->Set_Text(strText);
}

void CUI_TutorialGuideSlot::Set_Animations(_int iIndex)
{
    Set_Animation(iIndex);
    auto pContainer = Get_Component<CObjectContainer>();
    for (auto& pChild : pContainer->Get_Children())
    {
        if (!pChild)
            continue;

        if (auto pUI = dynamic_cast<CUI_Object*>(pChild))
            pUI->Set_Animation(iIndex);
    }
}

string CUI_TutorialGuideSlot::Get_PrefabPath(TUTORIAL_ACTION eAction)
{
    switch (eAction)
    {
    case TUTORIAL_ACTION::DODGE: return "tutorial_guide_slot_dodge.json";
    case TUTORIAL_ACTION::DODGE_COUNTER: return "tutorial_guide_slot_dodgeCounter.json";
    case TUTORIAL_ACTION::ASSIST: return "tutorial_guide_slot_assist.json";
    case TUTORIAL_ACTION::ASSIST_CHARGE: return "tutorial_guide_slot_assistCharge.json";
    case TUTORIAL_ACTION::ULTIMATE: return "tutorial_guide_slot_ultimate.json";
    case TUTORIAL_ACTION::COMBO: return "tutorial_guide_slot_combo.json";
    default: return "tutorial_guide_slot_complete.json";
    }
}

CGameObject* CUI_TutorialGuideSlot::Create()
{
    CUI_TutorialGuideSlot* pInstance = new CUI_TutorialGuideSlot();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_TutorialGuideSlot");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_TutorialGuideSlot::Clone(INIT_DESC* pArg)
{
    CUI_TutorialGuideSlot* pInstance = new CUI_TutorialGuideSlot(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_TutorialGuideSlot");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_TutorialGuideSlot::Free()
{
    __super::Free();
}