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

    //// ¿Ã∫•∆Æ : TUTORIAL_DESC
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
}

void CUI_TutorialGuideSlot::UI_DeActive(void* pArg)
{
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