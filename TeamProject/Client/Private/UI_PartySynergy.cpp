#include "pch.h"
#include "UI_PartySynergy.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"

#include "DataBase.h"

void CUI_PartySynergy::Set_Synergy(_int iPartySynergyCount, _int iTotalPartyCount)
{
    Set_Text(Helper::ConvertToWideString(to_string(iPartySynergyCount) + "/" + to_string(iTotalPartyCount)));
    if (iPartySynergyCount >= 2)
        Set_Icon("IconPairUpSkillSmall02.png");
    else
        Set_Icon("IconPairUpSkillSmall01.png");
}

void CUI_PartySynergy::Set_Synergy(vector<CHARACTER> characters)
{
    array<_int, static_cast<_int>(ATTRIBUTE::END)> counts = {};

    auto pDatabase = CDataBase::GetInstance();
    for (auto character : characters)
    {
        if (character == CHARACTER::END)
            continue;
         
        counts[static_cast<_int>(pDatabase->GetPartyData(character).eAttribute)]++;
    }

    _int iTotalPartyCount = characters.size();
    _int iPartySynergyCount = {};
    for (int i = 0; i < static_cast<_int>(ATTRIBUTE::END); ++i)
        if (counts[i] > iPartySynergyCount)
            iPartySynergyCount = counts[i];
     
    Set_Text(Helper::ConvertToWideString(to_string(iPartySynergyCount) + "/" + to_string(iTotalPartyCount)));
    if (iPartySynergyCount >= 2)
        Set_Icon("IconPairUpSkillSmall02.png");
    else
        Set_Icon("IconPairUpSkillSmall01.png"); 
}

HRESULT CUI_PartySynergy::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_PartySynergy::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("partySynergy.json")));
    Cache();

    if (m_pIcon)
        m_pIcon->Change_Texture(0, G_GlobalLevelKey, "IconPairUpSkillSmall02.png");

    return S_OK;
}

void CUI_PartySynergy::Awake()
{
}

void CUI_PartySynergy::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_PartySynergy::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pIcon = pContainer->Find_Descendant("icon");
    if (pIcon)
        m_pIcon = pIcon->Get_Component<CSprite2D>();

    auto pText = pContainer->Find_Descendant("text");
    if (pText)
        m_pText = pText->Get_Component<CTextSlot>();
}

void CUI_PartySynergy::Set_Text(const _wstring& strText)
{
    if (!m_pText)
        return;
    
    m_pText->Set_Text(strText);
}

void CUI_PartySynergy::Set_Icon(const string& strTextureKey)
{
    if (!m_pIcon)
        return;

    m_pIcon->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

CGameObject* CUI_PartySynergy::Create()
{
    CUI_PartySynergy* pInstance = new CUI_PartySynergy();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_PartySynergy");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_PartySynergy::Clone(INIT_DESC* pArg)
{
    CUI_PartySynergy* pInstance = new CUI_PartySynergy(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_PartySynergy");
        Safe_Release(pInstance);
    }
    return pInstance;
}