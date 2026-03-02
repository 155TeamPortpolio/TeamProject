#include "pch.h"
#include "UI_PartyCard.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"

#include "UI_PartyCardRTDraw.h"
#include "UI_PartyAvatar.h"

#include "DataBase.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

#include "UVAnimationUI.h"

void CUI_PartyCard::Change_Character(CHARACTER eCharacter, ATTRIBUTE eMaxAttribute)
{
    RENDER_LAYER layer = RENDER_LAYER::None;

    _bool hasCharacter = (eCharacter != CHARACTER::END);

    if (hasCharacter)
    {
        layer = RENDER_LAYER::Default;
        
        auto pDesc = CDataBase::GetInstance()->GetPartyData(eCharacter);
        Change_Avatar(pDesc.strModelKey, pDesc.strMaterialKey, pDesc.strMetaKey, pDesc.strAnimClipKey, pDesc.vPosition);
        Change_Icons( (eMaxAttribute == pDesc.eAttribute) ? "IconPairUpSkillSmall02.png" : "IconPairUpSkillSmall01.png", pDesc.strAttributeTexture, pDesc.strSpecialtyTexture);
        Change_Texts(pDesc.strName, pDesc.iLevel);

        Set_ChildColor(CHILD::BG_FRONT, pDesc.vColor);
        Set_ChildColor(CHILD::BG_BACK, _float4(pDesc.vColor.x * 0.7f, pDesc.vColor.y * 0.7f, pDesc.vColor.z * 0.7f, pDesc.vColor.w));
        Set_ChildColor(CHILD::BG_PATTERN, _float4(pDesc.vColor.x * 0.8f, pDesc.vColor.y * 0.8f, pDesc.vColor.z * 0.8f, pDesc.vColor.w * 0.2f));
    }
    else
    {
        Set_ChildColor(CHILD::BG_FRONT, m_vBGFrontColor);
        Set_ChildColor(CHILD::BG_BACK, m_vBGBackColor);
        Set_ChildColor(CHILD::BG_PATTERN, _float4(1.f, 1.f, 1.f, 0.f));
    }

    if (m_pRTDraw)
    {
        m_pRTDraw->SetRenderLayer(layer);
        m_pRTDraw->UI_Active();
    }
        
    Set_ChildAlive(CHILD::EMPTY, !hasCharacter);
    Set_ChildAlive(CHILD::OCCUPIED, hasCharacter);  
}

void CUI_PartyCard::Reverse_UVAnimDirection(_bool isReverse)
{
    if (!isReverse)
        return;

    auto pFront = dynamic_cast<CUVAnimationUI*>(m_pChildren[ENUM(CHILD::BG_FRONT)]);
    if (!pFront)
        return;

    pFront->Reverse_Direction();   
}

HRESULT CUI_PartyCard::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_PartyCard::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    // ∑ª¥ı≈∏∞Ÿ µﬁ∫Œ∫– ∑ŒµÂ«œ∞Ì
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("partyCard.json")));
    Cache();

    // ∑ª¥ı≈∏∞Ÿ ∏∏µÈ∞Ì
    CARD_DESC* pDesc = static_cast<CARD_DESC*>(pArg);
    Create_RenderTarget(pDesc->strRenderTargetKey);

    // æ∆πŸ≈∏ ∏∏µÈæÓ
    Create_Avatar(pDesc->strRenderTargetKey);

    m_vBGBackColor = Get_ChildColor(CHILD::BG_BACK);
    m_vBGFrontColor = Get_ChildColor(CHILD::BG_FRONT);

    return S_OK;
}

void CUI_PartyCard::Awake()
{
    auto pContainer = Get_Component<CObjectContainer>();
    pContainer->Set_Order_Last(pContainer->Find_Descendant("partyCard_front"));
}

void CUI_PartyCard::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_PartyCard::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
    }

    for (_int i = 0; i < ENUM(SPRITE2D::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES_SPRITE2D[i]);
        if (!pObj)
            continue;

        m_pSprites[i] = pObj->Get_Component<CSprite2D>();
    }

    for (_int i = 0; i < ENUM(TEXTSLOT::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES_TEXTSLOT[i]);
        if (!pObj)
            continue;

        m_pTextSlots[i] = pObj->Get_Component<CTextSlot>();
    }
}

void CUI_PartyCard::Create_RenderTarget(const string& strRenderTargetKey)
{
    CUI_PartyCardRTDraw::CARD_DESC* pDesc = new CUI_PartyCardRTDraw::CARD_DESC;
    pDesc->strRenderTargetKey = strRenderTargetKey;

    auto pObj = Builder::Create_UIObject({ "Scott_Level", "Proto_GameObject_PartyCardRTDraw"})
        .Add_UIDesc(pDesc)
        .Build("partyCardRTDraw");

    if (!pObj)
        return;

    pObj->Set_Pivot(_float2(0.5f, 0.5f));
    pObj->Set_Anchor(ANCHOR::Center);
    pObj->Set_Size(m_WinSize);

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pRTDraw = pObj;
}

HRESULT CUI_PartyCard::Create_Avatar(const string& strRenderTargetKey)
{
    CUI_PartyAvatar::AVATAR_DESC* pAvatarDesc = new CUI_PartyAvatar::AVATAR_DESC;
    pAvatarDesc->strRenderTargetKey = strRenderTargetKey;

    auto pObj = Builder::Create_Object({ "Scott_Level", "Proto_GameObject_PartyAvatar" })
        .Add_ObjDesc(pAvatarDesc)
        .Build("avatar");

    if (!pObj)
        return E_FAIL;

    ObjectManager()->Add_Object(pObj, { "Scott_Level", "UI_Layer" });
    m_hAvatar = pObj->Get_Handle();

    return S_OK;
}

void CUI_PartyCard::Change_Avatar(const string& strModelKey, const string& strMaterialKey, const string& strMetaKey, const string& strAnimClipKey, _float3 vPosition)
{
    if (!m_hAvatar.isValid())
        return;

    auto pObj = m_hAvatar.Get();

    auto pModel = pObj->Get_Component<CSkeletalModel>();
    auto pMaterial = pObj->Get_Component<CMaterial>();
    auto pAnimator = pObj->Get_Component<CAnimator3D>();
    auto pTransform = pObj->Get_Component<CTransform>();

    pModel->Link_Model(G_GlobalLevelKey, strModelKey);
    pModel->SetModelDrawable(true);
    pModel->Hide_MehsByName("HairShadow");

    pMaterial->Link_Material(G_GlobalLevelKey, strMaterialKey);

    pAnimator->LinkAnimate_Model(G_GlobalLevelKey, strModelKey);
    pAnimator->Link_MetaData(G_GlobalLevelKey, strMetaKey);
    pAnimator->Set_Animation(strAnimClipKey).Loop(true).Apply();

    pTransform->Set_Pos(vPosition);
}

void CUI_PartyCard::Change_Icons(const string& strPairKey, const string& strAttributeKey, const string& strSpecialtyKey)
{
    Change_SpriteTexture(SPRITE2D::ICON_PAIR, strPairKey);
    Change_SpriteTexture(SPRITE2D::ICON_ATTRIBUTE, strAttributeKey);
    Change_SpriteTexture(SPRITE2D::ICON_SPECIALTY, strSpecialtyKey);
}

void CUI_PartyCard::Change_Texts(const wstring& strName, _int iLevel)
{
    Set_Text(TEXTSLOT::NAME, strName);
    Set_Text(TEXTSLOT::LEVEL, Helper::ConvertToWideString("Lv." + to_string(iLevel)));
}

void CUI_PartyCard::Set_ChildAlive(CHILD child, _bool isAlive)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Alive(isAlive);
}

void CUI_PartyCard::Set_ChildColor(CHILD child, _float4 vColor)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Color(vColor);
}

_float4 CUI_PartyCard::Get_ChildColor(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return _float4();

    return pChild->Get_Color();
}

void CUI_PartyCard::Change_SpriteTexture(SPRITE2D sprite, const string& strTextureKey)
{
    auto pSprite = m_pSprites[ENUM(sprite)];
    if (!pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

void CUI_PartyCard::Set_Text(TEXTSLOT textSlot, const _wstring& strText)
{
    auto pTextSlot = m_pTextSlots[ENUM(textSlot)];
    if (!pTextSlot)
        return;

    pTextSlot->Set_Text(strText);
}

CGameObject* CUI_PartyCard::Create()
{
    CUI_PartyCard* pInstance = new CUI_PartyCard();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_PartyCard");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_PartyCard::Clone(INIT_DESC* pArg)
{
    CUI_PartyCard* pInstance = new CUI_PartyCard(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_PartyCard");
        Safe_Release(pInstance);
    }
    return pInstance;
}