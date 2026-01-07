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
    auto pGameInstance = CGameInstance::GetInstance();

    string strCurrentLevel = pGameInstance->Get_LevelMgr()->Get_NowLevelKey();
    // Battle HUD 프리팹 (json) 로드 후 UI 트리 루트(CanvasPanel) 생성
    CUI_Object* pRoot = Builder::Create_UIObject({ strCurrentLevel, "Proto_GameObject_CanvasPanel" })
        .Asset("hud_battle.json")
        .Build("prefab");

    if (!pRoot)
        return;

    // 생성된 루트 UI를 uiMgr에 등록
    pGameInstance->Get_UIMgr()->Add_UIObject(pRoot, strCurrentLevel);
   
    // UI 트리 기준으로 주요 핸들 캐싱 (root / chidlren)
    CacheHandle(pRoot);

    // 루트 UI의 0번 애니메이션 재생 (FadeIn)
    if (m_hRoot.isValid())
        m_hRoot.Get()->Set_Animation(0);
}

void CUI_BattleHUD::Update(_float dt)
{
}

void CUI_BattleHUD::CacheHandle(CUI_Object* pRoot)
{ 
    // 루트 핸들 캐싱
    m_hRoot = pRoot->Get_Handle();

    // 자식 핸들 캐싱
    m_hChildren.resize(PREFAB::END);
    for (_int i = 0; i < 3; ++i)
    {
        m_hChildren[ICON_PREFABS[i]] = pRoot->Get_DescendantHandle("icon" + to_string(i + 1));
        m_hChildren[HPBACK_PREFABS[i]] = pRoot->Get_DescendantHandle("hpBack" + to_string(i + 1));
        m_hChildren[HPFRONT_PREFABS[i]] = pRoot->Get_DescendantHandle("hpFront" + to_string(i + 1));
        m_hChildren[SPECIAL_PREFABS[i]] = pRoot->Get_DescendantHandle("special" + to_string(i + 1));
        m_hChildren[ULTIMATE_PREFABS[i]] = pRoot->Get_DescendantHandle("ultimate" + to_string(i + 1));
    }

    //m_hChildren[PREFAB::CUR_HP_TEXT] = ;
    //m_hChildren[PREFAB::MAX_HP_TEXT] = ;

    m_hChildren[PREFAB::BOSS_ICON] = pRoot->Get_DescendantHandle("bossIcon");
    m_hChildren[PREFAB::BOSS_HP_BACK] = pRoot->Get_DescendantHandle("bossHpBack");
    m_hChildren[PREFAB::BOSS_HP_FRONT] = pRoot->Get_DescendantHandle("bossHpFront");
    m_hChildren[PREFAB::BOSS_GROGGY] = pRoot->Get_DescendantHandle("bossGroggy");
    //m_hChildren[PREFAB::BOSS_GROGGY_TEXT] = pPrefab->Get_DescendantHandle("bossGroggyText");

    //m_hChildren[PREFAB::BTN_NORMAL] = ;
    //m_hChildren[PREFAB::BTN_EVADE] = ;
    //m_hChildren[PREFAB::BTN_SPECIAL] = ;
    //m_hChildren[PREFAB::BTN_SWITCH] = ;
    //m_hChildren[PREFAB::BTN_ULTIMATE] = ;
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