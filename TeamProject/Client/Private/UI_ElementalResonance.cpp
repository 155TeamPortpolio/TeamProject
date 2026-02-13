#include "pch.h"
#include "UI_ElementalResonance.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"

void CUI_ElementalResonance::Set_Count(_int iCount)
{
    m_iCount = iCount;

    Set_Text();
}

void CUI_ElementalResonance::Set_TotalCount(_int iCount)
{
    m_iTotalCount = iCount;

    Set_Text();
}

HRESULT CUI_ElementalResonance::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_ElementalResonance::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("elementalResonance.json")));
    Cache();

    if (m_pIcon)
        m_pIcon->Change_Texture(0, G_GlobalLevelKey, "IconPairUpSkillSmall02.png");

    return S_OK;
}

void CUI_ElementalResonance::Awake()
{
}

void CUI_ElementalResonance::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_ElementalResonance::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pIcon = pContainer->Find_Descendant("icon");
    if (pIcon)
        m_pIcon = pIcon->Get_Component<CSprite2D>();

    auto pText = pContainer->Find_Descendant("text");
    if (pText)
        m_pText = pText->Get_Component<CTextSlot>();
}

void CUI_ElementalResonance::Set_Text()
{
    m_strText = Helper::ConvertToWideString(to_string(m_iCount) + " / " + to_string(m_iTotalCount));
    if (m_pText)
        m_pText->Set_Text(m_strText);
}

CGameObject* CUI_ElementalResonance::Create()
{
    CUI_ElementalResonance* pInstance = new CUI_ElementalResonance();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_ElementalResonance");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_ElementalResonance::Clone(INIT_DESC* pArg)
{
    CUI_ElementalResonance* pInstance = new CUI_ElementalResonance(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_ElementalResonance");
        Safe_Release(pInstance);
    }
    return pInstance;
}