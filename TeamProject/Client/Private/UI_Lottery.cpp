#include "pch.h"
#include "UI_Lottery.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_Lottery::Initialize_Prototype()
{
	__super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_Lottery::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    // JSON ·Îµå
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("lottery.json")));

	return S_OK;
}

void CUI_Lottery::Awake()
{
}

void CUI_Lottery::Update(_float dt)
{
	__super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_Lottery::Late_Update(_float dt)
{
}

void CUI_Lottery::UI_Active(void* pArg)
{
}

CGameObject* CUI_Lottery::Create()
{
    CUI_Lottery* pInstance = new CUI_Lottery();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Lottery");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Lottery::Clone(INIT_DESC* pArg)
{
    CUI_Lottery* pInstance = new CUI_Lottery(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Lottery");
        Safe_Release(pInstance);
    }
    return pInstance;
}