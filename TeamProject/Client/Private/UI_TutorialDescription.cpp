#include "pch.h"
#include "UI_TutorialDescription.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_TutorialDescription::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_TutorialDescription::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    TUTORIAL_DESC* pDesc = static_cast<TUTORIAL_DESC*>(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath(Get_PrefaPath(pDesc->eType))));// "tutorial_extremeEvade.json")));

    return S_OK;
}

void CUI_TutorialDescription::Awake()
{
}

void CUI_TutorialDescription::Update(_float dt)
{
    __super::Update(dt);
    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_TutorialDescription::UI_Active(void* pArg)
{
}

void CUI_TutorialDescription::UI_DeActive(void* pArg)
{
}

string CUI_TutorialDescription::Get_PrefaPath(TUTORIAL_TYPE eType)
{
    switch (eType)
    {
    case TUTORIAL_TYPE::EXTREME_EVADE: return "tutorial_extremeEvade.json";
    case TUTORIAL_TYPE::EXTREME_SUPPORT: return "tutorial_extremeSupport.json";
    case TUTORIAL_TYPE::DECIBEL_ULTIMATE: return "tutorial_decibelUltimate.json";
    case TUTORIAL_TYPE::GROGGY_COMBO: return "tutorial_groggyCombo.json";
    default: return "tutorial_extremeEvade.png";
    }
}

CGameObject* CUI_TutorialDescription::Create()
{
    CUI_TutorialDescription* pInstance = new CUI_TutorialDescription();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_TutorialDescription");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_TutorialDescription::Clone(INIT_DESC* pArg)
{
    CUI_TutorialDescription* pInstance = new CUI_TutorialDescription(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_TutorialDescription");
        Safe_Release(pInstance);
    }
    return pInstance;
}