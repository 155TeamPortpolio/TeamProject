#include "pch.h"
#include "UI_RamenAttributeText.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "TextSlot.h"
#include "TextUI.h"

HRESULT CUI_RamenAttributeText::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_RamenAttributeText::Initialize(INIT_DESC* pArg)
{
    ATTRIBUTE_DESC* pDesc = static_cast<ATTRIBUTE_DESC*>(pArg);

    __super::Initialize(pArg);

    auto pContainer = Get_Component<CObjectContainer>();
    _int iIndex = {}; 

    _int iSize = pDesc->attributes.size();
    for (auto& attribute : pDesc->attributes)
    { 
        // 이름
        auto pObjName = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_Text" }).Build("text");
        if (!pObjName)
            continue;
        
        auto pTextSlotName = pObjName->Get_Component<CTextSlot>();
        if (!pTextSlotName)
            continue;

        pTextSlotName->Set_Font("NanumSquareNeo.spritefont");
        pTextSlotName->Set_Text(attribute.strAttributeName);
        pTextSlotName->Set_Size(0.32f);
        pTextSlotName->Set_Color(m_vColor);
        pTextSlotName->Enable_AutoPos(ANCHOR::Center);
        pContainer->Add_Child(pObjName);

        // 값
        auto pObjValue = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_Text" }).Build("text");
        if (!pObjValue)
            continue;

        pObjValue->Set_Color(Helper::HexToColor("#F7B23C"));

        auto pTextSlotValue = pObjValue->Get_Component<CTextSlot>();
        if (!pTextSlotValue)
            continue;

        pTextSlotValue->Set_Font("NanumSquareNeo.spritefont");
        pTextSlotValue->Set_Text(Helper::ConvertToWideString("+" + to_string(attribute.iAttributeValue) + "%"));
        pTextSlotValue->Set_Size(0.32f);
        pTextSlotValue->Set_Color(m_vColor);
        pTextSlotValue->Enable_AutoPos(ANCHOR::Center);
        pContainer->Add_Child(pObjValue);

        // 레이아웃
        _float fNameWidth = pTextSlotName->Get_TextSize().x * pTextSlotName->Get_Scale();
        _float fValueWidth = pTextSlotValue->Get_TextSize().x * pTextSlotValue->Get_Scale();
        _float fTotalWidth = fNameWidth + fValueWidth;

        pObjName->Add_AnchorOffsetX((fNameWidth - fTotalWidth) * 0.5f);
        pObjValue->Add_AnchorOffsetX((fTotalWidth - fValueWidth) * 0.5f);
        
        pObjName->Add_AnchorOffsetY(pTextSlotName->Get_TextSize().y * pTextSlotName->Get_Scale() * 0.8f * ( 2 * iIndex - (iSize - 1)));
        pObjValue->Add_AnchorOffsetY(pTextSlotValue->Get_TextSize().y * pTextSlotValue->Get_Scale() * 0.8f * (2 * iIndex - (iSize - 1)));

        iIndex++;
    }

    m_eAnchor = ANCHOR::Center;

	return S_OK;
}

void CUI_RamenAttributeText::Awake()
{
}

void CUI_RamenAttributeText::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_RamenAttributeText::Late_Update(_float dt)
{
}

void CUI_RamenAttributeText::UI_Active(void* pArg)
{
    Set_Alpha(1.f);
}

void CUI_RamenAttributeText::UI_DeActive(void* pArg)
{
    Set_Alpha(0.f);
}

CGameObject* CUI_RamenAttributeText::Create()
{
    CUI_RamenAttributeText* pInstance = new CUI_RamenAttributeText();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_RamenAttributeText");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_RamenAttributeText::Clone(INIT_DESC* pArg)
{
    CUI_RamenAttributeText* pInstance = new CUI_RamenAttributeText(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_RamenAttributeText");
        Safe_Release(pInstance);
    }
    return pInstance;
}