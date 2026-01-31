#include "pch.h"
#include "UI_RamenOrderBanner.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "TextSlot.h"

HRESULT CUI_RamenOrderBanner::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_RamenOrderBanner::Initialize(INIT_DESC* pArg)
{
	__super::Initialize();

	Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("ramen_order_banner.json")));
    Cache();

    UI_DeActive();

	return S_OK;
}

void CUI_RamenOrderBanner::Awake()
{
}

void CUI_RamenOrderBanner::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_RamenOrderBanner::UI_Active(void* pArg)
{
    Set_Alive(true);

    wstring strMenu = L"";
    if (pArg)
    {
        ORDER_DESC* pDesc = static_cast<ORDER_DESC*>(pArg);
        strMenu = pDesc->strMenu;
        replace(strMenu.begin(), strMenu.end(), L'\n', L' ');
    }
     
    m_pLabelTextSlot->Set_Text(strMenu + L"(을)를 선택하시겠습니까?");
}

void CUI_RamenOrderBanner::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

void CUI_RamenOrderBanner::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    {
        auto pObj = pContainer->Find_Descendant("label");
        if (!pObj)
            return;

        m_pLabelTextSlot = pObj->Get_Component<CTextSlot>();
    }
}

CGameObject* CUI_RamenOrderBanner::Create()
{
    CUI_RamenOrderBanner* pInstance = new CUI_RamenOrderBanner();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_RamenOrderBanner");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_RamenOrderBanner::Clone(INIT_DESC* pArg)
{
    CUI_RamenOrderBanner* pInstance = new CUI_RamenOrderBanner(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_RamenOrderBanner");
        Safe_Release(pInstance);
    }
    return pInstance;
}