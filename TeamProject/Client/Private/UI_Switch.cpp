#include "pch.h"
#include "UI_Switch.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 

HRESULT CUI_Switch::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Switch::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("switch.json")));

    Set_Alive(false);

    return S_OK;
}

void CUI_Switch::Awake()
{
    __super::Awake();
}

void CUI_Switch::Update(_float dt)
{
    __super::Update(dt);

    Update_RTV("renderTargetScreen", true);
}

CGameObject* CUI_Switch::Create()
{
    CUI_Switch* pInstance = new CUI_Switch();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Switch");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Switch::Clone(INIT_DESC* pArg)
{
    CUI_Switch* pInstance = new CUI_Switch(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Switch");
        Safe_Release(pInstance);
    }
    return pInstance;
}