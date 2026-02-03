#include "pch.h"
#include "UI_GachaPage.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_GachaPage::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaPage::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha.json")));

    Set_Alive(false);

	return S_OK;
}

void CUI_GachaPage::Awake()
{
}

void CUI_GachaPage::Update(_float dt)
{
	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaPage::UI_Active(void* pArg)
{
    Set_Alive(true);
}

void CUI_GachaPage::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

CGameObject* CUI_GachaPage::Create()
{
    CUI_GachaPage* pInstance = new CUI_GachaPage();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaPage");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaPage::Clone(INIT_DESC* pArg)
{
    CUI_GachaPage* pInstance = new CUI_GachaPage(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaPage");
        Safe_Release(pInstance);
    }
    return pInstance;
}