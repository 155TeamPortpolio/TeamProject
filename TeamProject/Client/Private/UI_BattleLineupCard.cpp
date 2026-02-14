#include "pch.h"
#include "UI_BattleLineupCard.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "UI_BattleLineupCardRT.h"

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

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("battleLineupCard.json")));

    Create_RenderTarget();

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

void CUI_BattleLineupCard::Create_RenderTarget()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_BattleLineupCardRT" })
        .Build("rt");

    if (!pObj)
        return;

    pObj->Set_Pivot(_float2(0.5f, 0.5f));
    pObj->Set_Anchor(ANCHOR::Center);
    pObj->Set_Size(m_WinSize);

    //pObj->Set_Size(_float2(500.f, 500.f));

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