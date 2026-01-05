#include "pch.h"
#include "UI_BattleHUD.h"

#include "GameInstance.h"

HRESULT CUI_BattleHUD::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CUI_BattleHUD::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CUI_BattleHUD::Awake()
{
    string strCurrentLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
    CUI_Object* pPrefab = Builder::Create_UIObject({ strCurrentLevel, "Proto_GameObject_CanvasPanel" })
        .Asset("hud_battle.json")
        .Build("prefab");

    if (!pPrefab)
        return;

    CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pPrefab, strCurrentLevel);
    m_hRoot = pPrefab->Get_Handle();

    //if (m_hRoot.isValid())
    //    m_hRoot.Get()->Set_Animation(0);
}

void CUI_BattleHUD::Update(_float dt)
{
}

CGameObject* CUI_BattleHUD::Create()
{
    CUI_BattleHUD* pInstance = new CUI_BattleHUD();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_BattleHUD");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BattleHUD::Clone(INIT_DESC* pArg)
{
    CUI_BattleHUD* pInstance = new CUI_BattleHUD(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_BattleHUD");
        Safe_Release(pInstance);
    }
    return pInstance;
}