#include "pch.h"
#include "UI_GachaCurrency.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_GachaCurrency::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaCurrency::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha_currency.json")));

	return S_OK;
}

void CUI_GachaCurrency::Awake()
{
}

void CUI_GachaCurrency::Update(_float dt)
{
	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaCurrency::UI_Active(void* pArg)
{
}

void CUI_GachaCurrency::UI_DeActive(void* pArg)
{
}

CGameObject* CUI_GachaCurrency::Create()
{
    CUI_GachaCurrency* pInstance = new CUI_GachaCurrency();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaCurrency");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaCurrency::Clone(INIT_DESC* pArg)
{
    CUI_GachaCurrency* pInstance = new CUI_GachaCurrency(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaCurrency");
        Safe_Release(pInstance);
    }
    return pInstance;
}