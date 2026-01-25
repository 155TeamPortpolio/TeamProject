#include "pch.h"
#include "UI_ScratchCard.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

HRESULT CUI_ScratchCard::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_ScratchCard::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("scratchCard.json")));

    Cache_RewardSprite();

	return S_OK;
}

void CUI_ScratchCard::Awake()
{
}

void CUI_ScratchCard::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_ScratchCard::UI_Active(void* pArg)
{
    Change_RewardTexture(REWARD_TEXTURES[rand() % ENUM(REWARD::END)]);
}

void CUI_ScratchCard::Cache_RewardSprite()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pObj = pContainer->Find_Descendant("reward");
    if (!pObj)
        return;

    m_pRewardSprite = pObj->Get_Component<CSprite2D>();
}

void CUI_ScratchCard::Change_RewardTexture(const string& strTextureKey)
{
    if (!m_pRewardSprite)
        return;

    m_pRewardSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

CGameObject* CUI_ScratchCard::Create()
{
    CUI_ScratchCard* pInstance = new CUI_ScratchCard();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_ScratchCard");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_ScratchCard::Clone(INIT_DESC* pArg)
{
    CUI_ScratchCard* pInstance = new CUI_ScratchCard(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_ScratchCard");
        Safe_Release(pInstance);
    }
    return pInstance;
}