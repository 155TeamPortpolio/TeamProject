#include "pch.h"
#include "UI_RamenOrderBanner.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "TextSlot.h"

#include "UI_IconButton.h"

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

    Create_CancelButton();
    Create_ConfirmButton();

    Change_State(STATE::INVISIBLE);
    Set_Alive(false); 

	return S_OK;
}

void CUI_RamenOrderBanner::Awake()
{
}

void CUI_RamenOrderBanner::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (m_eState == STATE::INVISIBLE && Is_AnimFinished())
        Set_Alive(false);
}

void CUI_RamenOrderBanner::UI_Active(void* pArg)
{
    Change_State(STATE::VISIBLE);

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
    Change_State(STATE::INVISIBLE);
}

void CUI_RamenOrderBanner::Create_CancelButton()
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->onClick = [this]() { OnClick_Cancel(); };
    pDesc->strLabel = L"취소";
    pDesc->strTextureKey = "IconCancel.png"; 
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_IconButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonCancel");

    if (!pObj)
        return;

    pObj->Add_AnchorOffsetX(-136.f);
    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_RamenOrderBanner::Create_ConfirmButton()
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

    pObj->Add_AnchorOffsetX(136.f);
    Get_Component<CObjectContainer>()->Add_Child(pObj);
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

void CUI_RamenOrderBanner::Change_State(STATE eState)
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

void CUI_RamenOrderBanner::OnClick_Cancel()
{
    UI_DeActive();
}

void CUI_RamenOrderBanner::OnClick_Confirm()
{
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