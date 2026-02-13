#include "pch.h"
#include "UI_EnterBattleButton.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_EnterBattleButton::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_EnterBattleButton::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("button_enterBattle.json")));

    return S_OK;
}

void CUI_EnterBattleButton::Awake()
{
}

void CUI_EnterBattleButton::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

CGameObject* CUI_EnterBattleButton::Create()
{
    CUI_EnterBattleButton* pInstance = new CUI_EnterBattleButton();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_EnterBattleButton");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_EnterBattleButton::Clone(INIT_DESC* pArg)
{
    CUI_EnterBattleButton* pInstance = new CUI_EnterBattleButton(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_EnterBattleButton");
        Safe_Release(pInstance);
    }
    return pInstance;
}