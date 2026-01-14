#include "pch.h"
#include "UI_BattleHUD.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "TextSlot.h"
#include "GaugeUI.h" 

HRESULT CUI_BattleHUD::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CEventListener>();

    return S_OK;
}

HRESULT CUI_BattleHUD::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    // 이벤트 : UI_STATUS_DESC
    Get_Component<CEventListener>()->Add_Listener<UI_STATUS_DESC>([&](const UI_STATUS_DESC& desc)
        {
            Set_Values(desc);
        });

    // 이벤트 : UI_PLAYER_STATUS_DESC
    Get_Component<CEventListener>()->Add_Listener<UI_PLAYER_STATUS_DESC>([&](const UI_PLAYER_STATUS_DESC& desc)
        {
            Set_Values(desc);
        });

    return S_OK;
}

void CUI_BattleHUD::Awake()
{
    auto pGameInstance = CGameInstance::GetInstance();

    const string& strLevelKey = pGameInstance->Get_LevelMgr()->Get_NowLevelKey();

    auto pRoot = Ready_Prefab(strLevelKey);
    if (!pRoot)
        return;

    // 루트 UI의 0번 애니메이션 재생 (FadeIn)
    if (m_hRoot.isValid())
        m_hRoot.Get()->Set_Animation(0);
}

void CUI_BattleHUD::Update(_float dt)
{
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

    Add_PartObject(pRoot, strLevelKey, "Proto_GameObject_Decibel", "decibel", Child::ULTIMATE1, _float2(50.f, 136.f));
    Add_PartObject(pRoot, strLevelKey, "Proto_GameObject_BattleHUDAction", "action", Child::ACTION, _float2(1178.f, 655.f));

    // 생성된 루트 UI를 uiMgr에 등록
    if (FAILED(pGameInstance->Get_UIMgr()->Add_UIObject(pRoot, strLevelKey)))
        return nullptr;

    // UI 트리 기준으로 주요 핸들 캐싱 (root / chidlren)
    Cache_Handles(pRoot);

    return pRoot;
}

void CUI_BattleHUD::Add_PartObject(CUI_Object* pRoot, const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, Child child, _float2 vOffset)
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

    m_handles[child] = handle;
}

void CUI_BattleHUD::Cache_Handles(CUI_Object* pRoot)
{
    // 루트 핸들 캐싱
    m_hRoot = pRoot->Get_Handle();

    // 자식 핸들 캐싱
    m_handles.resize(Child::END);
    for (_int i = 0; i < 3; ++i)
    {
        m_handles[ICON_CHILD[i]] = pRoot->Get_DescendantHandle("icon" + to_string(i + 1));
        m_handles[HPBACK_CHILD[i]] = pRoot->Get_DescendantHandle("hpBack" + to_string(i + 1));
        m_handles[HPFRONT_CHILD[i]] = pRoot->Get_DescendantHandle("hpFront" + to_string(i + 1));
        m_handles[SPECIAL_CHILD[i]] = pRoot->Get_DescendantHandle("special" + to_string(i + 1));
        m_handles[ULTIMATE_CHILD[i]] = pRoot->Get_DescendantHandle("ultimate" + to_string(i + 1));
    }

    m_handles[Child::CUR_HP_TEXT] = pRoot->Get_DescendantHandle("curHpText");
    m_handles[Child::MAX_HP_TEXT] = pRoot->Get_DescendantHandle("maxHpText");

    m_handles[Child::BOSS_ICON] = pRoot->Get_DescendantHandle("bossIcon");
    m_handles[Child::BOSS_HP_BACK] = pRoot->Get_DescendantHandle("bossHpBack");
    m_handles[Child::BOSS_HP_FRONT] = pRoot->Get_DescendantHandle("bossHpFront");
    m_handles[Child::BOSS_GROGGY] = pRoot->Get_DescendantHandle("bossGroggy");
    m_handles[Child::BOSS_GROGGY_TEXT] = pRoot->Get_DescendantHandle("bossGroggyText");
}

void CUI_BattleHUD::Set_Values(UI_STATUS_DESC desc)
{
    switch (desc.eOwner)
    {
    case UI_STATUS_OWNER::ROLE1:
        if (desc.eType == UI_STATUS_TYPE::HP)
        {
            Set_FillAmount(Child::HP_FRONT1, desc.value.fCurValue / desc.value.fMaxValue);
            Set_Text(Child::MAX_HP_TEXT, desc.value.fCurValue);
        }
        else if(desc.eType == UI_STATUS_TYPE::SPECIAL)
            Set_FillAmount(Child::SPECIAL1, desc.value.fCurValue / desc.value.fMaxValue);
        else if (desc.eType == UI_STATUS_TYPE::ULTIMATE)
            Set_FillAmount(Child::ULTIMATE1, desc.value.fCurValue / desc.value.fMaxValue);
        break;

    case UI_STATUS_OWNER::ROLE2:
        if (desc.eType == UI_STATUS_TYPE::HP)
            Set_FillAmount(Child::HP_FRONT2, desc.value.fCurValue / desc.value.fMaxValue);
        else if (desc.eType == UI_STATUS_TYPE::SPECIAL)
            Set_FillAmount(Child::SPECIAL2, desc.value.fCurValue / desc.value.fMaxValue);
        else if (desc.eType == UI_STATUS_TYPE::ULTIMATE)
            Set_FillAmount(Child::ULTIMATE2, desc.value.fCurValue / desc.value.fMaxValue);
        break;

    case UI_STATUS_OWNER::ROLE3:
        if (desc.eType == UI_STATUS_TYPE::HP)
            Set_FillAmount(Child::HP_FRONT3, desc.value.fCurValue / desc.value.fMaxValue);
        else if (desc.eType == UI_STATUS_TYPE::SPECIAL)
            Set_FillAmount(Child::SPECIAL3, desc.value.fCurValue / desc.value.fMaxValue);
        else if (desc.eType == UI_STATUS_TYPE::ULTIMATE)
            Set_FillAmount(Child::ULTIMATE3, desc.value.fCurValue / desc.value.fMaxValue);
        break;

    case UI_STATUS_OWNER::BOSS:
        if (desc.eType == UI_STATUS_TYPE::HP)
        {
            Set_FillAmount(Child::BOSS_HP_FRONT, desc.value.fCurValue / desc.value.fMaxValue);
        }
        else if (desc.eType == UI_STATUS_TYPE::GROGGY)
        {
            Set_FillAmount(Child::BOSS_GROGGY, desc.value.fCurValue / desc.value.fMaxValue);
            Set_Text(Child::BOSS_GROGGY_TEXT, desc.value.fCurValue);
        }
        break;
    }
}

void CUI_BattleHUD::Set_Values(UI_PLAYER_STATUS_DESC desc)
{
    switch (desc.eOwner)
    {
    case UI_STATUS_OWNER::ROLE1:
        Set_FillAmount(Child::HP_FRONT1, desc.hp.fCurValue / desc.hp.fMaxValue);
        Set_FillAmount(Child::SPECIAL1, desc.special.fCurValue / desc.special.fMaxValue);
        Set_FillAmount(Child::ULTIMATE1, desc.ultimate.fCurValue / desc.ultimate.fMaxValue);
        Set_Text(Child::CUR_HP_TEXT, desc.hp.fCurValue);
        Set_Text(Child::MAX_HP_TEXT, desc.hp.fMaxValue);
        break;
    case UI_STATUS_OWNER::ROLE2:
        Set_FillAmount(Child::HP_FRONT2, desc.hp.fCurValue / desc.hp.fMaxValue);
        Set_FillAmount(Child::SPECIAL2, desc.special.fCurValue / desc.special.fMaxValue);
        Set_FillAmount(Child::ULTIMATE2, desc.ultimate.fCurValue / desc.ultimate.fMaxValue);
        break;
    case UI_STATUS_OWNER::ROLE3:
        Set_FillAmount(Child::HP_FRONT2, desc.hp.fCurValue / desc.hp.fMaxValue);
        Set_FillAmount(Child::SPECIAL2, desc.special.fCurValue / desc.special.fMaxValue);
        Set_FillAmount(Child::ULTIMATE2, desc.ultimate.fCurValue / desc.ultimate.fMaxValue);
        break;
    }
}

void CUI_BattleHUD::Set_Text(Child child, _float fNum)
{
    ForChild(child, [&](CUI_Object* ui) 
        { 
            auto pTextSlot = ui->Get_Component<CTextSlot>();
            if (!pTextSlot)
                return;

            pTextSlot->Set_Text(Helper::ConvertToWideString(to_string(static_cast<_int>(fNum))));
        });
}

void CUI_BattleHUD::Set_FillAmount(Child child, _float fFillAmount)
{
    ForChild(child, [&](CUI_Object* ui)
        {
            auto pGauge = dynamic_cast<CGaugeUI*>(ui);
            if (!pGauge)
                return;

            pGauge->Set_FillAmount(fFillAmount);
        });
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