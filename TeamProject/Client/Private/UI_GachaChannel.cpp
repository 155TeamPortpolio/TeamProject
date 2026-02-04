#include "pch.h"
#include "UI_GachaChannel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"

#include "ButtonUI.h"

HRESULT CUI_GachaChannel::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaChannel::Initialize(INIT_DESC* pArg)
{
    CHANNEL_DESC* pDesc = static_cast<CHANNEL_DESC*>(pArg);
    m_onSelect = pDesc->onSelect;

    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha_channel.json")));
    Cache();

    if (!pArg)
        return S_OK;

    if (m_pIcon)
        m_pIcon->Change_Texture(0, G_GlobalLevelKey, "IconRole13_GaCha_S.png");

    if (m_pLabel)
        m_pLabel->Set_Text(L"µ¶Á¡ Àç°³ºÀ");

    if (m_pButton)
        m_pButton->Set_OnClick([this]() {
        if (m_onSelect)
            m_onSelect(this);
            });

    m_vLerpColors[0] = Helper::HexToColor("#F7DB00");
    m_vLerpColors[1] = Helper::HexToColor("#9CBC00");

	return S_OK;
}

void CUI_GachaChannel::Awake()
{
}

void CUI_GachaChannel::Update(_float dt)
{
    __super::Update(dt);

    if (m_isSelected)
    {
        m_fLerpTimer += dt * m_fLerpSpeed;
        XMStoreFloat4(&m_vColor, XMVectorLerp(m_vLerpColors[0], m_vLerpColors[1], sinf(m_fLerpTimer)));

        m_pOutline->Set_Color(m_vColor);
        m_pPattern->Set_Color(m_vColor);
    }
    else
    { 
        m_fLerpTimer = 0.f;
        m_pOutline->Set_Alpha(0.f);
        m_pPattern->Set_Alpha(0.f);
    } 

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaChannel::UI_Active(void* pArg)
{
    m_isSelected = true;
}

void CUI_GachaChannel::UI_DeActive(void* pArg)
{
    m_isSelected = false;
}

void CUI_GachaChannel::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    m_pOutline = dynamic_cast<CUI_Object*>(pContainer->Find_Descendant("outline"));
    m_pPattern = dynamic_cast<CUI_Object*>(pContainer->Find_Descendant("pattern"));
    m_pIcon = pContainer->Find_Descendant("icon")->Get_Component<CSprite2D>();
    m_pLabel = pContainer->Find_Descendant("label")->Get_Component<CTextSlot>();
    m_pButton = dynamic_cast<CButtonUI*>(pContainer->Find_Descendant("button"));
}

void CUI_GachaChannel::OnClick()
{
}

CGameObject* CUI_GachaChannel::Create()
{
    CUI_GachaChannel* pInstance = new CUI_GachaChannel();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaChannel");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaChannel::Clone(INIT_DESC* pArg)
{
    CUI_GachaChannel* pInstance = new CUI_GachaChannel(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaChannel");
        Safe_Release(pInstance);
    }
    return pInstance;
}