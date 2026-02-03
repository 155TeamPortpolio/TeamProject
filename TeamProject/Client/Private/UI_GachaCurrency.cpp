#include "pch.h"
#include "UI_GachaCurrency.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "TextSlot.h"

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
    Cache();

    UI_Active();

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
    _int iValue = { 56 };

    // 텍스트 슬롯 (패딩, 값) 에 값 채움
    string strValue = to_string(iValue);
    _int iPaddingCount = max(0, m_iMaxDigits - strValue.length());
    string strPadding(iPaddingCount, '0');

    m_pTextSlots[ENUM(CHILD::PADDING)]->Set_Text(Helper::ConvertToWideString(strPadding));
    m_pTextSlots[ENUM(CHILD::VALUE)]->Set_Text(Helper::ConvertToWideString(strValue));

    // 레이아웃
    _float2 vValueAnchorOffset = m_pChildren[ENUM(CHILD::VALUE)]->Get_AnchorOffset();
    _float2 vValueSize = m_pTextSlots[ENUM(CHILD::VALUE)]->Get_TextSize() * m_pTextSlots[ENUM(CHILD::VALUE)]->Get_Scale();
    m_pChildren[ENUM(CHILD::PADDING)]->Set_AnchorOffsetX(vValueAnchorOffset.x - vValueSize.x + 2.f);
}

void CUI_GachaCurrency::UI_DeActive(void* pArg)
{
}

void CUI_GachaCurrency::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;
        
        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
        m_pTextSlots[i] = pObj->Get_Component<CTextSlot>();
    }
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