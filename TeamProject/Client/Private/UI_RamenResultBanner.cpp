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
    //m_OnClick = pDesc->onClickConfirm;

    __super::Initialize();

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("ramen_result_banner.json")));
    Cache();

    Create_ConfirmButton();

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

    ACTIVE_DESC* pDesc = static_cast<ACTIVE_DESC*>(pArg);
    Set_Text(TEXTSLOT::LABEL1, L"[" + pDesc->strMenu + L"] 효과 발동");
}

void CUI_RamenResultBanner::UI_DeActive(void* pArg)
{
    Change_State(STATE::INVISIBLE);
}

void CUI_RamenResultBanner::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(TEXTSLOT::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pTextSlots[i] = pObj->Get_Component<CTextSlot>();
    } 
}

void CUI_RamenResultBanner::Create_ConfirmButton()
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->onClick = [this]() { OnClick_Confirm(); };
    pDesc->strLabel = L"확인";
    pDesc->strTextureKey = "IconOK.png";
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_IconButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonOK");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
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

void CUI_RamenResultBanner::Set_Text(TEXTSLOT textSlot, const _wstring& strText)
{
    auto pTextSlot = m_pTextSlots[ENUM(textSlot)];
    if (!pTextSlot)
        return;

    pTextSlot->Set_Text(strText);
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