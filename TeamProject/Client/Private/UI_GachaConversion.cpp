#include "pch.h"
#include "UI_GachaConversion.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_GachaConversion::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaConversion::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha_conversion.json")));

	return S_OK;
}

void CUI_GachaConversion::Awake()
{
}

void CUI_GachaConversion::Update(_float dt)
{
	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaConversion::UI_Active(void* pArg)
{
}

void CUI_GachaConversion::UI_DeActive(void* pArg)
{
}

CGameObject* CUI_GachaConversion::Create()
{
    CUI_GachaConversion* pInstance = new CUI_GachaConversion();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaConversion");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaConversion::Clone(INIT_DESC* pArg)
{
    CUI_GachaConversion* pInstance = new CUI_GachaConversion(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaConversion");
        Safe_Release(pInstance);
    }
    return pInstance;
}