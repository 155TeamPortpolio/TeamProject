#include "pch.h"
#include "UI_RamenResultBanner.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 
#include "TextSlot.h"

#include "UI_IconButton.h"

HRESULT CUI_RamenResultBanner::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_RamenResultBanner::Initialize(INIT_DESC* pArg)
{
    RESULT_BANNER_DESC* pDesc = static_cast<RESULT_BANNER_DESC*>(pArg);
    m_OnClick = pDesc->onClickConfirm;

    __super::Initialize();

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("ramen_result_banner.json")));
    Cache();

    Create_ConfirmButton();
    Create_AttrTexts();

    Change_State(STATE::INVISIBLE);
    Set_Alive(false);

    return S_OK;
}

void CUI_RamenResultBanner::Awake()
{
}

void CUI_RamenResultBanner::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (m_eState == STATE::INVISIBLE && Is_AnimFinished())
        Set_Alive(false);
}

void CUI_RamenResultBanner::UI_Active(void* pArg)
{
    Change_State(STATE::VISIBLE);

    if (!pArg)
        return;

    ACTIVE_DESC* pDesc = static_cast<ACTIVE_DESC*>(pArg); 
    if (m_pLabelTextSlot)
    {
        wstring strMenu = pDesc->strMenu;
        replace(strMenu.begin(), strMenu.end(), L'\n', L' ');
        m_pLabelTextSlot->Set_Text(L"[" + strMenu + L"] È¿°ú ¹ßµ¿");
    } 
     
    const _int iStartIndex = ENUM(ATTR::LABEL) + 1;

    for (_int i = iStartIndex; i < ENUM(ATTR::END); ++i)
        m_pAttrTextSlots[i]->Set_Text(L"");

    _int iIndex = iStartIndex;
    for (auto& pAttribute : pDesc->attributes)
    {
        if (iIndex >= ENUM(ATTR::END))
            return;

        m_pAttrTextSlots[iIndex++]->Set_Text(pAttribute.strAttributeName);
        m_pAttrTextSlots[iIndex++]->Set_Text(Helper::ConvertToWideString("+" + to_string(pAttribute.iAttributeValue) + "% "));
    }

    Refresh_Layout();
}

void CUI_RamenResultBanner::UI_DeActive(void* pArg)
{
    Change_State(STATE::INVISIBLE);
}

void CUI_RamenResultBanner::Cache()
{
    auto pObj = Get_Component<CObjectContainer>()->Find_Descendant("label");
    if (!pObj)
        return;

    m_pLabelTextSlot = pObj->Get_Component<CTextSlot>();
}

void CUI_RamenResultBanner::Create_ConfirmButton()
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->onClick = [this]() { OnClick_Confirm(); };
    pDesc->strLabel = L"È®ÀÎ";
    pDesc->strTextureKey = "IconOK.png";
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_IconButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonOK");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_RamenResultBanner::Create_AttrTexts()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(ATTR::END); ++i)
        Create_AttrText(&m_pAttrObjects[i], &m_pAttrTextSlots[i], (i == ENUM(ATTR::VALUE1) || i == ENUM(ATTR::VALUE2) || i == ENUM(ATTR::VALUE3)));
}

void CUI_RamenResultBanner::Create_AttrText(CUI_Object** ppOutObj, class CTextSlot** ppOutTextSlot, _bool isHighlighted)
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_Text" }).Build("attr");
    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    if(ppOutObj)
        *ppOutObj = pObj;
    pObj->Set_Anchor(ANCHOR::Center);
    pObj->Set_AnchorOffset({ 0.f, -20.f });
    if (isHighlighted)
        pObj->Set_Color(Helper::HexToColor("#FBAF2A"));
    else
        pObj->Set_Color(Helper::HexToColor("#B2B2B2FF"));

    auto pTextSlot = pObj->Get_Component<CTextSlot>();
    if(ppOutTextSlot)
        *ppOutTextSlot = pTextSlot;
    pTextSlot->Set_Font("NanumSquareNeo.spritefont");
    pTextSlot->Set_Text(L"È¹µæ È¿°ú: ");
    pTextSlot->Set_Size(0.42f);
    pTextSlot->Enable_AutoPos(ANCHOR::Center);
}

void CUI_RamenResultBanner::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;
    switch (eState)
    {
    case STATE::INVISIBLE:
        Set_Animation(1);
        break;
    case STATE::VISIBLE:
        Set_Alive(true);
        Set_Animation(0);
        break;
    }
}

void CUI_RamenResultBanner::OnClick_Confirm()
{
    UI_DeActive();
    if (m_OnClick)
        m_OnClick();
}

void CUI_RamenResultBanner::Refresh_Layout()
{
    _float fTotalWidth = {};
    for (_int i = 0; i < ENUM(ATTR::END); ++i)
    {
        auto pTextSlot = m_pAttrTextSlots[i];
        _float2 vTextSize = pTextSlot->Get_TextSize()* pTextSlot->Get_Scale();
        fTotalWidth += vTextSize.x;
    }

    _float fCurX = -fTotalWidth * 0.5f;
    for (_int i = 0; i < ENUM(ATTR::END); ++i)
    {
        auto pObj = m_pAttrObjects[i];
        auto pTextSlot = m_pAttrTextSlots[i];

        _float2 vTextSize = pTextSlot->Get_TextSize() * pTextSlot->Get_Scale();
        pObj->Set_AnchorOffsetX(fCurX + vTextSize.x * 0.5f);
        fCurX += vTextSize.x;
    }
}

CGameObject* CUI_RamenResultBanner::Create()
{
    CUI_RamenResultBanner* pInstance = new CUI_RamenResultBanner();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_RamenResultBanner");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_RamenResultBanner::Clone(INIT_DESC* pArg)
{
    CUI_RamenResultBanner* pInstance = new CUI_RamenResultBanner(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_RamenResultBanner");
        Safe_Release(pInstance);
    }
    return pInstance;
}