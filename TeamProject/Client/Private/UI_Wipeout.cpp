#include "pch.h"
#include "UI_Wipeout.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 

HRESULT CUI_Wipeout::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Wipeout::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("wipeout.json")));

    Set_Alive(false);

    return S_OK;
}

void CUI_Wipeout::Awake()
{
    __super::Awake();
}

void CUI_Wipeout::Update(_float dt)
{
    __super::Update(dt);

    Update_RTV("renderTargetScreen", true);
}

CGameObject* CUI_Wipeout::Create()
{
    CUI_Wipeout* pInstance = new CUI_Wipeout();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Wipeout");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Wipeout::Clone(INIT_DESC* pArg)
{
    CUI_Wipeout* pInstance = new CUI_Wipeout(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Wipeout");
        Safe_Release(pInstance);
    }
    return pInstance;
}