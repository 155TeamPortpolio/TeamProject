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
        XMStoreFloat3(&m_vRGB, Helper::HexToColor("#FFD721"));
        Change_SpriteTexture(SPRITE::RANK_ICON, "RANK_S.png");
        break;
    case GachaGrade::A:
        XMStoreFloat3(&m_vRGB, Helper::HexToColor("#8B25D4"));
        Change_SpriteTexture(SPRITE::RANK_ICON, "RANK_A.png");
        break;
    case GachaGrade::B:
        XMStoreFloat3(&m_vRGB, Helper::HexToColor("#3B7FFC"));
        Change_SpriteTexture(SPRITE::RANK_ICON, "RANK_B.png");
        break;
    }

    Change_SpriteTexture(SPRITE::ITEM_ICON_BACK, desc.strTexture);
    Change_SpriteTexture(SPRITE::ITEM_ICON_FRONT, desc.strTexture);

    if (desc.Type == GachaType::Engine)
    { 
        Set_ChildAnchorOffset(SPRITE::ITEM_ICON_FRONT, _float2(-54.f, -85.f));
        Set_ChildSize(SPRITE::ITEM_ICON_FRONT, _float2(170.f, 170.f));
    } 
    else if (desc.Type == GachaType::Agent)
    {
        if (desc.strTexture == "IconRole13.png")    // 미야비
        { 
            Set_ChildAnchorOffset(SPRITE::ITEM_ICON_FRONT, _float2(-370.f, -220.f));
            Set_ChildSize(SPRITE::ITEM_ICON_FRONT, _float2(800.f, 1122.96f));
        }
        else if (desc.strTexture == "IconRole24.png")   // 제인도
        {
            Set_ChildAnchorOffset(SPRITE::ITEM_ICON_FRONT, _float2(-210.f, -100.f));
            Set_ChildSize(SPRITE::ITEM_ICON_FRONT, _float2(533.96f, 1000.f));
        }
    }
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
        m_pOverlay->Set_RGB(m_vRGB);
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

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
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

void CUI_GachaResultItem::Set_ChildAnchorOffset(SPRITE sprite, _float2 vOffset)
{
    auto pChild = m_pChildren[ENUM(sprite)];
    if (!pChild)
        return;

    pChild->Set_AnchorOffset(vOffset);
}

void CUI_GachaResultItem::Set_ChildSize(SPRITE sprite, _float2 vSize)
{
    auto pChild = m_pChildren[ENUM(sprite)];
    if (!pChild)
        return;

    pChild->Set_Size(vSize);
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