#include "pch.h"
#include "UI_PartyCard.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "UI_PartyCardRTDraw.h"
#include "UI_PartyAvatar.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

void CUI_PartyCard::Change_Character(CHARACTER eCharacter)
{
    string strModel = {}, strMaterial = {}, strMeta = {}, strAnim = {};
    _float3 vPosition = {};

    switch (eCharacter)
    { 
    case CHARACTER::Corin:
        strModel = "Corin";
        strMaterial = "Corin";
        strMeta = "Avatar_Female_Size01_Corin_Meta";
        strAnim = "Avatar_Female_Size01_Corin_Ani_Idle";
        vPosition = _float3(0.f, -1.f, 0.9f);
        break;
    case CHARACTER::JaneDoe:
        strModel = "JaneDoeModel";
        strMaterial = "JaneDoe";
        strMeta = "JaneDoe_Meta";
        strAnim = "Avatar_Female_Size03_JaneDoe_Ani_Gacha_Loop_02";
        vPosition = _float3(0.f, -1.42f, 0.8f);
        break;
    case CHARACTER::Miyabi:
        strModel = "Miyabi";
        strMaterial = "Miyabi";
        strMeta = "Miyabi_Meta";
        strAnim = "Avatar_Female_Size02_Unagi_Ani_UI_CharacterSelect_Loop_02";
        vPosition = _float3(0.f, -1.24f, 0.7f);
        break;
    default: 
        return;
    }

    if (!m_hAvatar.isValid())
        return;

    auto pObj = m_hAvatar.Get();

    auto pModel = pObj->Get_Component<CSkeletalModel>();
    auto pMaterial = pObj->Get_Component<CMaterial>();
    auto pAnimator = pObj->Get_Component<CAnimator3D>();
    auto pTransform = pObj->Get_Component<CTransform>();

    pModel->Link_Model(G_GlobalLevelKey, strModel + ".model"); 
    pModel->SetModelDrawable(true);
    pModel->Hide_MehsByName("HairShadow");

    pMaterial->Link_Material(G_GlobalLevelKey, strMaterial + ".mat");

    pAnimator->LinkAnimate_Model(G_GlobalLevelKey, strModel + ".model");
    pAnimator->Link_MetaData(G_GlobalLevelKey,strMeta + ".json");
    pAnimator->Set_Animation(strAnim).Loop(true).Apply();

    pTransform->Set_Pos(vPosition);
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
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("partyCard_back.json")));

    // ∑ª¥ı≈∏∞Ÿ ∏∏µÈ∞Ì
    CARD_DESC* pDesc = static_cast<CARD_DESC*>(pArg);
    Create_RenderTarget(pDesc->strRenderTargetKey);

    // ∑ª¥ı≈∏∞Ÿ æ’∫Œ∫– ∑ŒµÂ«ÿ
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("partyCard_front.json")));

    // æ∆πŸ≈∏ ∏∏µÈæÓ
    Create_Avatar(pDesc->strRenderTargetKey);

    return S_OK;
}

void CUI_PartyCard::Awake()
{
}

void CUI_PartyCard::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
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