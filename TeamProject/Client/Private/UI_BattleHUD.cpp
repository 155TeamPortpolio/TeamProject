#include "pch.h"
#include "UI_BattleHUD.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "GaugeUI.h"

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

    const string& strLevelKey = pGameInstance->Get_LevelMgr()->Get_NowLevelKey();

    auto pRoot = Ready_Prefab(strLevelKey);
    if (!pRoot)
        return;

    Ready_Decibel(pRoot, strLevelKey, "Proto_GameObject_Decibel", "decibel", PREFAB::ULTIMATE1, _float2(68.f, 136.f));
  
    // 루트 UI의 0번 애니메이션 재생 (FadeIn)
    if (m_hRoot.isValid())
        m_hRoot.Get()->Set_Animation(0);
}

void CUI_BattleHUD::Update(_float dt)
{
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('G'))
    //{
    //    UI_STATUS_DESC desc = {};
    //    desc.eOwner = UI_STATUS_OWNER::ROLE1;
    //    desc.eType = UI_STATUS_TYPE::HP;
    //    desc.fCurValue = 50.f;
    //    desc.fMaxValue = 80.f;
    //    CGameInstance::GetInstance()->Get_EventSystem()->Broadcast<UI_STATUS_DESC>({ desc });
    //}
}

CUI_Object* CUI_BattleHUD::Ready_Prefab(const string& strLevelKey)
{
    auto pGameInstance = CGameInstance::GetInstance();

    // Battle HUD 프리팹 (json) 로드 후 UI 트리 루트(CanvasPanel) 생성
    CUI_Object* pRoot = Builder::Create_UIObject({ strLevelKey, "Proto_GameObject_CanvasPanel" })
        .Asset("hud_battle.json")
        .Build("prefab");

    if (!pRoot)
        return nullptr;

    // 생성된 루트 UI를 uiMgr에 등록
    if (FAILED(pGameInstance->Get_UIMgr()->Add_UIObject(pRoot, strLevelKey)))
        return nullptr;

    // UI 트리 기준으로 주요 핸들 캐싱 (root / chidlren)
    Cache_Handles(pRoot);

    return pRoot;
}

void CUI_BattleHUD::Ready_Decibel(CUI_Object* pRoot, const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, PREFAB prefab, _float2 vOffset)
{
    CUI_Object* pObj = Builder::Create_UIObject({ strLevelKey, strPrototypeTag })
        .Offset(vOffset)
        .Build(strInstanceName);

    if (!pObj)
        return;

    pRoot->Get_Component<CObjectContainer>()->Add_Child(pObj);

    UI_HANDLE handle = pObj->Get_Handle();
    if (!handle.isValid())
        return;

    m_hChildren[prefab] = handle;
}

void CUI_BattleHUD::Cache_Handles(CUI_Object* pRoot)
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

    m_hChildren[PREFAB::CUR_HP_TEXT] = pRoot->Get_DescendantHandle("curHpText");
    m_hChildren[PREFAB::MAX_HP_TEXT] = pRoot->Get_DescendantHandle("maxHpText");

    m_hChildren[PREFAB::BOSS_ICON] = pRoot->Get_DescendantHandle("bossIcon");
    m_hChildren[PREFAB::BOSS_HP_BACK] = pRoot->Get_DescendantHandle("bossHpBack");
    m_hChildren[PREFAB::BOSS_HP_FRONT] = pRoot->Get_DescendantHandle("bossHpFront");
    m_hChildren[PREFAB::BOSS_GROGGY] = pRoot->Get_DescendantHandle("bossGroggy");
    m_hChildren[PREFAB::BOSS_GROGGY_TEXT] = pRoot->Get_DescendantHandle("bossGroggyText");

    //m_hChildren[PREFAB::BTN_NORMAL] = ;
    //m_hChildren[PREFAB::BTN_EVADE] = ;
    //m_hChildren[PREFAB::BTN_SPECIAL] = ;
    //m_hChildren[PREFAB::BTN_SWITCH] = ;
    //m_hChildren[PREFAB::BTN_ULTIMATE] = ;

    // 게이지 정보(소유자, 게이지 타입) 설정
    for(const auto& bind : GaugeBindings)
    {
        auto& handle = m_hChildren[bind.ePrefab];
        if (!handle.isValid())
            continue;

        if (auto pGauge = dynamic_cast<CGaugeUI*>(handle.Get()))
        {
            pGauge->Set_Status(bind.eOwner, bind.eType);
        }
    }
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