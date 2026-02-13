#include "pch.h"
#include "UI_ElementalResonance.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_ElementalResonance::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_ElementalResonance::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("elementalResonance.json")));

    return S_OK;
}

void CUI_ElementalResonance::Awake()
{
}

void CUI_ElementalResonance::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

CGameObject* CUI_ElementalResonance::Create()
{
    CUI_ElementalResonance* pInstance = new CUI_ElementalResonance();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_ElementalResonance");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_ElementalResonance::Clone(INIT_DESC* pArg)
{
    CUI_ElementalResonance* pInstance = new CUI_ElementalResonance(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_ElementalResonance");
        Safe_Release(pInstance);
    }
    return pInstance;
}