#include "pch.h"
#include "UI_WipeoutRTV.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 

HRESULT CUI_WipeoutRTV::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_WipeoutRTV::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

   Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("wipeout.json")));

	return S_OK;
}

void CUI_WipeoutRTV::Awake()
{
    __super::Awake();
}

void CUI_WipeoutRTV::Update(_float dt)
{
    __super::Update(dt);

    Update_RTV("renderTargetScreen", true);
}

CGameObject* CUI_WipeoutRTV::Create()
{
    CUI_WipeoutRTV* pInstance = new CUI_WipeoutRTV();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_WipeoutRTV");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_WipeoutRTV::Clone(INIT_DESC* pArg)
{
    CUI_WipeoutRTV* pInstance = new CUI_WipeoutRTV(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_WipeoutRTV");
        Safe_Release(pInstance);
    }
    return pInstance;
}