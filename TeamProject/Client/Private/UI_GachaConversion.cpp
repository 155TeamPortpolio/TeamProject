#include "pch.h"
#include "UI_GachaConversion.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "TextSlot.h"
#include "ButtonUI.h"

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
    Cache();

    if (!pArg)
        return S_OK;

    CONVERSION_DESC* pDesc = static_cast<CONVERSION_DESC*>(pArg);

    Set_ChildText(CHILD::COST, Helper::ConvertToWideString(to_string(pDesc->iCost / 10000)) + L"¸¸");
    Set_ChildText(CHILD::COUNT, Helper::ConvertToWideString(to_string(pDesc->iCount)));
    m_OnClick = pDesc->onClick;

    if (m_pButton)
        m_pButton->Set_OnClick([this]() { OnClick(); });

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

void CUI_GachaConversion::Cache()
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

    m_pButton = dynamic_cast<CButtonUI*>(pContainer->Find_Descendant("button"));
}

void CUI_GachaConversion::OnClick()
{
    Set_ChildAnimation(CHILD::OVERLAY, 0);
    Set_ChildAnimation(CHILD::LABEL, 0);
    Set_ChildAnimation(CHILD::COUNT, 0);
    if (m_OnClick)
        m_OnClick();
}

void CUI_GachaConversion::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

void CUI_GachaConversion::Set_ChildText(CHILD child, const _wstring& strText)
{
    auto pTextSlot = m_pTextSlots[ENUM(child)];
    if (!pTextSlot)
        return;

    pTextSlot->Set_Text(strText);
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