#include "pch.h"
#include "UI_TutorialGuideStart.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_TutorialGuideStart::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_TutorialGuideStart::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("tutorial_guide_start.json")));

    return S_OK;
}

void CUI_TutorialGuideStart::Awake()
{
}

void CUI_TutorialGuideStart::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_TutorialGuideStart::UI_Active(void* pArg)
{
    Set_Animation(0);

    auto pContainer = Get_Component<CObjectContainer>();
    for (auto& pChild : pContainer->Get_Children())
    {
        if (!pChild)
            continue;

        if (auto pUI = dynamic_cast<CUI_Object*>(pChild))
            pUI->Set_Animation(0);
    }
}

void CUI_TutorialGuideStart::UI_DeActive(void* pArg)
{
    Set_Animation(1);
}

CGameObject* CUI_TutorialGuideStart::Create()
{
    CUI_TutorialGuideStart* pInstance = new CUI_TutorialGuideStart();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_TutorialGuideStart");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_TutorialGuideStart::Clone(INIT_DESC* pArg)
{
    CUI_TutorialGuideStart* pInstance = new CUI_TutorialGuideStart(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_TutorialGuideStart");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_TutorialGuideStart::Free()
{
    __super::Free();
}