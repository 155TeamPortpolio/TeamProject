#include "pch.h"
#include "UI_GachaResultItem.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_GachaResultItem::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaResultItem::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha_result_b.json")));

    m_pOverlay = dynamic_cast<CUI_Object*>(Get_Component<CObjectContainer>()->Find_Descendant("overlay"));

	return S_OK;
}

void CUI_GachaResultItem::Awake()
{
}

void CUI_GachaResultItem::Update(_float dt)
{
	__super::Update(dt);
     

	Get_Component<CObjectContainer>()->UpdateChild(dt);
    if (m_pOverlay)
        m_pOverlay->Set_RGB(_float3(1.f, 0.f, 0.f));
}

void CUI_GachaResultItem::UI_Active(void* pArg)
{
    Set_Alive(true);
    Set_Animation(0);
}

void CUI_GachaResultItem::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

CGameObject* CUI_GachaResultItem::Create()
{
    CUI_GachaResultItem* pInstance = new CUI_GachaResultItem();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaResultItem");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaResultItem::Clone(INIT_DESC* pArg)
{
    CUI_GachaResultItem* pInstance = new CUI_GachaResultItem(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaResultItem");
        Safe_Release(pInstance);
    }
    return pInstance;
}