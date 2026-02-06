#include "pch.h"
#include "UI_GachaResultItem.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

void CUI_GachaResultItem::Set_ResultDesc(GACHA_RESULT_DESC desc)
{
    Set_Alive(false);

    switch (desc.Grade)
    {
    case GachaGrade::S:
        m_vColor = { 1.f, 0.f, 0.f, 1.f };
        Change_SpriteTexture(SPRITE::RANK_ICON, "RANK_S.png");
        break;
    case GachaGrade::A:
        m_vColor = { 1.f, 0.f, 1.f, 1.f };
        Change_SpriteTexture(SPRITE::RANK_ICON, "RANK_A.png");
        break;
    case GachaGrade::B:
        m_vColor = { 0.f, 0.f, 1.f, 1.f };
        Change_SpriteTexture(SPRITE::RANK_ICON, "RANK_B.png");
        break;
    }

    Change_SpriteTexture(SPRITE::ITEM_ICON_BACK, desc.strTexture);
    Change_SpriteTexture(SPRITE::ITEM_ICON_FRONT, desc.strTexture);
}

HRESULT CUI_GachaResultItem::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaResultItem::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha_result_item.json")));
    Cache();

	return S_OK;
}

void CUI_GachaResultItem::Awake()
{
}

void CUI_GachaResultItem::Update(_float dt)
{
	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
    if (m_pOverlay)
        m_pOverlay->Set_RGB(_float3(m_vColor.x, m_vColor.y, m_vColor.z));
}

void CUI_GachaResultItem::UI_Active(void* pArg)
{
    Set_Alive(true);
    Set_Animation(0);
    auto pChildren = Get_Component<CObjectContainer>()->Get_Children();
    for (auto& pChild : pChildren)
        dynamic_cast<CUI_Object*>(pChild)->Set_Animation(0);
}

void CUI_GachaResultItem::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

void CUI_GachaResultItem::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    m_pOverlay = dynamic_cast<CUI_Object*>(pContainer->Find_Descendant("overlay"));

    for (_int i = 0; i < ENUM(SPRITE::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(SPRITE_INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pSprites[i] = pObj->Get_Component<CSprite2D>();
    }
}

void CUI_GachaResultItem::Change_SpriteTexture(SPRITE sprite, const string& strTextureKey)
{
    auto pSprite = m_pSprites[ENUM(sprite)];
    if (!pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

CGameObject* CUI_GachaResultItem::Create()
{
    CUI_GachaResultItem* pInstance = new CUI_GachaResultItem();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaResultItem");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaResultItem::Clone(INIT_DESC* pArg)
{
    CUI_GachaResultItem* pInstance = new CUI_GachaResultItem(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaResultItem");
        Safe_Release(pInstance);
    }
    return pInstance;
}