#include "pch.h"
#include "UI_TutorialGuideSlot.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"

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

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("tutorial_bubble.json")));

    //// 이벤트 : TUTORIAL_DESC
    //Get_Component<CEventListener>()->Add_Listener<TUTORIAL_DESC>([&](const TUTORIAL_DESC& desc)
    //    {
    //    });

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
    SLOT_DESC* pDesc = static_cast<SLOT_DESC*>(pArg);

    // 기존꺼 삭제하고 새로 로드 ??
    Get_PrefabPath(pDesc->desc.eAction);
}

void CUI_TutorialGuideSlot::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

string CUI_TutorialGuideSlot::Get_PrefabPath(TUTORIAL_ACTION eAction)
{
    switch (eAction)
    {
    case TUTORIAL_ACTION::DODGE: return "tutorial_slot_dodge.json";
    case TUTORIAL_ACTION::DODGE_COUNTER: return "tutorial_slot_dodgeCounter.json";
    case TUTORIAL_ACTION::ASSIST: return "tutorial_slot_assist.json";
    case TUTORIAL_ACTION::ASSIST_CHARGE: return "tutorial_slot_assistCharge";
    case TUTORIAL_ACTION::ULTIMATE: return "tutorial_slot_ultimate.json";
    }
    return"tutorial_bubble.json";
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