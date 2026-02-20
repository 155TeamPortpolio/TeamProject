#include "pch.h"
#include "UI_Banner.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "AudioSource.h"
#include "TextSlot.h"

#include "UI_IconButton.h"

HRESULT CUI_Banner::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();
    Add_Component<CAudioSource>();
    Get_Component<CAudioSource>()->SoundFolder(G_GlobalLevelKey, "../Bin/Resources/Global/UI/Sound/");

    return S_OK;
}

HRESULT CUI_Banner::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    BANNER_DESC* pDesc = static_cast<BANNER_DESC*>(pArg);
    m_onClickConfirm = pDesc->onClickConfirm;
    m_onClickCancel = pDesc->onClickCancel;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("banner.json")));
    Cache();

    Create_CancelButton();
    Create_ConfirmButton();

    if (m_pTitleTextSlot)
        m_pTitleTextSlot->Set_Text(pDesc->strTitle);

    if (m_pSubtitleTextSlot)
        m_pSubtitleTextSlot->Set_Text(pDesc->strSubtitle);

    return S_OK;
}

void CUI_Banner::Awake()
{
    Set_Alive(false);
}

void CUI_Banner::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (m_eState == STATE::INVISIBLE && Is_AnimFinished())
        Set_Alive(false);
}

void CUI_Banner::UI_Active(void* pArg)
{
    Change_State(STATE::VISIBLE);
}

void CUI_Banner::UI_DeActive(void* pArg)
{
    Change_State(STATE::INVISIBLE);
}

void CUI_Banner::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pTitle = pContainer->Find_Descendant("title");
    if (pTitle)
        m_pTitleTextSlot = pTitle->Get_Component<CTextSlot>();

    auto pSubtitle = pContainer->Find_Descendant("subtitle");
    if (pSubtitle)
        m_pSubtitleTextSlot = pSubtitle->Get_Component<CTextSlot>();
}

void CUI_Banner::Create_CancelButton()
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->onClick = [this]() {
        UI_DeActive(); 
        if (m_onClickCancel)
            m_onClickCancel();
        };
    pDesc->strLabel = L"취소";
    pDesc->strTextureKey = "IconCancel.png";
    pDesc->strSoundKey = "UI_Close_Swoosh.wav";
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_IconButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonCancel");

    if (!pObj)
        return;

    pObj->Add_AnchorOffsetX(-136.f);
    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_Banner::Create_ConfirmButton()
{
    CUI_IconButton::BUTTON_DESC* pDesc = new CUI_IconButton::BUTTON_DESC;
    pDesc->onClick = [this]() {
        UI_DeActive();
        if (m_onClickConfirm)
            m_onClickConfirm();
        };
    pDesc->strLabel = L"확인";
    pDesc->strTextureKey = "IconOK.png";
    pDesc->strSoundKey = "UI_Close_Swoosh.wav";
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_IconButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonOK");

    if (!pObj)
        return;

    pObj->Add_AnchorOffsetX(136.f);
    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_Banner::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;
    switch (eState)
    { 
    case STATE::VISIBLE:
        Set_Alive(true);
        Set_Animation(0);
        break;
    case STATE::INVISIBLE:
        Set_Animation(1);
        break;
    }
}

CGameObject* CUI_Banner::Create()
{
    CUI_Banner* pInstance = new CUI_Banner();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Banner");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Banner::Clone(INIT_DESC* pArg)
{
    CUI_Banner* pInstance = new CUI_Banner(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Banner");
        Safe_Release(pInstance);
    }
    return pInstance;
}