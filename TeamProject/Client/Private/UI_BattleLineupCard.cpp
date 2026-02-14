#include "pch.h"
#include "UI_BattleLineupCard.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "UI_BattleLineupCardRT.h"
#include "UI_PartyAvatar.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

void CUI_BattleLineupCard::Change_Character(CHARACTER eCharacter)
{
    string strModel = "Miyabi";
    string strMaterial = "Miyabi";
    string strMeta = "Miyabi_Meta";
    string strAnim = "Avatar_Female_Size02_Unagi_Ani_UI_CharacterSelect_Loop_02";
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
    }

    if (m_handle.isValid())
    {
        auto pObj = m_handle.Get();

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
}

HRESULT CUI_BattleLineupCard::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_BattleLineupCardRT", CUI_BattleLineupCardRT::Create());

    return S_OK;
}

HRESULT CUI_BattleLineupCard::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    // 배경 만들고
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("battleLineupCard_back.json")));

    // 렌더 타겟 만들고
    CARD_DESC* pDesc = static_cast<CARD_DESC*>(pArg);
    Create_RenderTarget(pDesc->strRenderTargetKey);

    // 앞 부분 만들고
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("battleLineupCard_front.json")));

    CUI_PartyAvatar::AVATAR_DESC* pAvatarDesc = new CUI_PartyAvatar::AVATAR_DESC;
    pAvatarDesc->strRenderTargetKey = pDesc->strRenderTargetKey;

    auto pObj = Builder::Create_Object({ "Test_Level", "Proto_GameObject_PartyAvatar" })
        .Add_ObjDesc(pAvatarDesc)
        .Build("avatar");

    if (pObj)
    {
        ObjectManager()->Add_Object(pObj, { "Test_Level", "UI_Layer" });
        m_handle = pObj->Get_Handle();
    } 

    return S_OK;
}

void CUI_BattleLineupCard::Awake()
{
}

void CUI_BattleLineupCard::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_BattleLineupCard::Create_RenderTarget(const string& strRenderTargetKey)
{
    CUI_BattleLineupCardRT::CARD_DESC* pDesc = new CUI_BattleLineupCardRT::CARD_DESC;
    pDesc->strRenderTargetKey = strRenderTargetKey;

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_BattleLineupCardRT" })
        .Add_UIDesc(pDesc)
        .Build("cardRT");

    if (!pObj)
        return;

    pObj->Set_Pivot(_float2(0.5f, 0.5f));
    pObj->Set_Anchor(ANCHOR::Center);
    pObj->Set_Size(m_WinSize);

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

CGameObject* CUI_BattleLineupCard::Create()
{
    CUI_BattleLineupCard* pInstance = new CUI_BattleLineupCard();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_BattleLineupCard");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BattleLineupCard::Clone(INIT_DESC* pArg)
{
    CUI_BattleLineupCard* pInstance = new CUI_BattleLineupCard(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_CharacterCard");
        Safe_Release(pInstance);
    }
    return pInstance;
}