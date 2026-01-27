#include "pch.h"
#include "UI_ResultBanner.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"
#include "ButtonUI.h"

HRESULT CUI_ResultBanner::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_ResultBanner::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    // JSON ·Îµå
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("result_banner.json")));

    Cache();

    if (m_pBtnOk)
        m_pBtnOk->Set_OnClick([this]() { OnClick_OK(); });

    UI_DeActive(nullptr);

	return S_OK;
}

void CUI_ResultBanner::Awake()
{
}

void CUI_ResultBanner::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_ResultBanner::UI_Active(void* pArg)
{
    if (!pArg)
        return;

    Set_Alive(true);

    RESULT_DESC* pDesc = static_cast<RESULT_DESC*>(pArg);
    Set_Text(TEXT::TEXT1, pDesc->wstrText1);
    Set_Text(TEXT::TEXT2, pDesc->wstrText2);
    Set_Icon(pDesc->strTextureKey);
}

void CUI_ResultBanner::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

void CUI_ResultBanner::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pObj = pContainer->Find_Descendant("btnOK");
    if (pObj)
        m_pBtnOk = dynamic_cast<CButtonUI*>(pObj);

    pObj = pContainer->Find_Descendant("icon");
    if (pObj)
        m_pSpriteIcon = pObj->Get_Component<CSprite2D>();

    pObj = pContainer->Find_Descendant("text1");
    if (pObj)
        m_pTextSlot[ENUM(TEXT::TEXT1)] = pObj->Get_Component<CTextSlot>();

    pObj = pContainer->Find_Descendant("text2");
    if (pObj)
        m_pTextSlot[ENUM(TEXT::TEXT2)] = pObj->Get_Component<CTextSlot>();
}

void CUI_ResultBanner::OnClick_OK()
{
    UI_DeActive(nullptr);
}

void CUI_ResultBanner::Set_Text(TEXT text, const _wstring& strText)
{
    auto pTextSlot = m_pTextSlot[ENUM(text)];
    if (!pTextSlot)
        return;

    pTextSlot->Set_Text(strText);
}

void CUI_ResultBanner::Set_Icon(const string& strTextureKey)
{
    if (!m_pSpriteIcon)
        return;

    m_pSpriteIcon->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

CGameObject* CUI_ResultBanner::Create()
{
    CUI_ResultBanner* pInstance = new CUI_ResultBanner();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_ResultBanner");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_ResultBanner::Clone(INIT_DESC* pArg)
{
    CUI_ResultBanner* pInstance = new CUI_ResultBanner(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_ResultBanner");
        Safe_Release(pInstance);
    }
    return pInstance;
}