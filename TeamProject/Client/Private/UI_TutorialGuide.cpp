#include "pch.h"
#include "UI_TutorialGuide.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "AudioSource.h"

#include "BattleSystem.h"
#include "UIDirector.h"
#include "DataBase.h" 

#include "UI_TutorialGuideSlot.h"
#include "UI_TutorialGuideStart.h"
#include "UI_LotteryResultBanner.h"

HRESULT CUI_TutorialGuide::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();
    Add_Component<CAudioSource>();
    Get_Component<CAudioSource>()->SoundFolder(G_GlobalLevelKey, "../Bin/Resources/Global/UI/Sound/");

    PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_TutorialGuideStart", CUI_TutorialGuideStart::Create());
    PrototypeManager()->Add_ProtoType("Tutorial_Level", "Proto_GameObject_TutorialGuideSlot", CUI_TutorialGuideSlot::Create());

	return S_OK;
}

HRESULT CUI_TutorialGuide::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("tutorial_guide.json")));

    Create_GuideStart();
    Create_SlotComplete();

    // 이벤트 : TUTORIAL_DESC
    Get_Component<CEventListener>()->Add_Listener<TUTORIAL_DESC>([this](const TUTORIAL_DESC& desc)
        {
            if (desc.eState != TUTORIAL_STATE::PLAY)
                return;

            Change_State(STATE::READY);
            Ready_Slots(desc.eType);
        });

    // 이벤트 : TUTORIAL_ACTION_COMPLETE
    Get_Component<CEventListener>()->Add_Listener<TUTORIAL_ACTION_COMPLETE>([this](const TUTORIAL_ACTION_COMPLETE& desc)
        {
            m_slotsProgress[desc.eAction] = true;

            const auto& actions = CDataBase::GetInstance()->GetTutorialActions(m_eType);
            for (auto& action : actions)
            {
                auto it = m_slotsProgress.find(action.eAction);
                if (it == m_slotsProgress.end() || !it->second)
                    return;
            }

            Change_State(STATE::DEACTIVATING);
        });

	return S_OK;
}

void CUI_TutorialGuide::Awake()
{
    Set_Alive(false);
}

void CUI_TutorialGuide::Update(_float dt)
{
    if (InputDevice()->Key_Tap('Z'))
    {
        TUTORIAL_ACTION_DESC desc = {};
        desc.eAction = TUTORIAL_ACTION::DODGE;
        EventSystem()->Broadcast<TUTORIAL_ACTION_DESC>({ desc });
    }

    if (InputDevice()->Key_Tap('X'))
    {
        TUTORIAL_ACTION_DESC desc = {};
        desc.eAction = TUTORIAL_ACTION::DODGE_COUNTER;
        EventSystem()->Broadcast<TUTORIAL_ACTION_DESC>({ desc });
    }

    if (InputDevice()->Key_Tap('C'))
    {
        TUTORIAL_ACTION_DESC desc = {};
        desc.eAction = TUTORIAL_ACTION::ASSIST;
        EventSystem()->Broadcast<TUTORIAL_ACTION_DESC>({ desc });
    }

    if (InputDevice()->Key_Tap('V'))
    {
        TUTORIAL_ACTION_DESC desc = {};
        desc.eAction = TUTORIAL_ACTION::ASSIST_CHARGE;
        EventSystem()->Broadcast<TUTORIAL_ACTION_DESC>({ desc });
    }

    switch (m_eState)
    {
    case STATE::READY:
        if (InputDevice()->Mouse_Tap(MOUSE_BTN::LB))
            Change_State(STATE::ACTIVE);
        break;
    case STATE::ACTIVE:
        if (InputDevice()->Key_Tap('T'))
            Change_State(STATE::DEACTIVATING);
        break;
    case STATE::DEACTIVATING:
        m_fTimer += dt;
        if (m_fTimer >= m_fDurationFadeout && !m_isFadeout)
        {
            UIDirector()->FadeOut_Screen(0.2f);
            m_isFadeout = true;
        } 
        if (m_fTimer >= m_fDurationWipeout)
            Change_State(STATE::INACTIVE);
        //if (!BattleSystem()->isVFXRunning(BATTLE_VFX_TYPE::WIPEOUT))
        //    Change_State(STATE::INACTIVE); 
        break;
    } 

	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_TutorialGuide::UI_Active(void* pArg)
{
}

void CUI_TutorialGuide::UI_DeActive(void* pArg)
{
}

HRESULT CUI_TutorialGuide::Create_GuideStart()
{
    auto pObj = Builder::Create_UIObject({ "Tutorial_Level", "Proto_GameObject_TutorialGuideStart"})
        .Build("start");

    if (!pObj)
        return E_FAIL;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pGuideStart = pObj;

    return S_OK;
}

HRESULT CUI_TutorialGuide::Create_SlotComplete()
{
    CUI_TutorialGuideSlot::SLOT_DESC* pDesc = new CUI_TutorialGuideSlot::SLOT_DESC;
    pDesc->desc.eAction = TUTORIAL_ACTION::END;

    auto pObj = Builder::Create_UIObject({ "Tutorial_Level", "Proto_GameObject_TutorialGuideSlot" })
        .Add_UIDesc(pDesc)
        .Build("complete");

    if (!pObj)
        return E_FAIL;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pSlotComplete = pObj;

    return S_OK;
}

HRESULT CUI_TutorialGuide::Show_ResultBanner()
{
    UIDirector()->FadeIn_Screen(0.2f);

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_LotteryResultBanner" })
        .Build("resultBanner");

    if (!pObj)
        return E_FAIL;
 
    UIManager()->Add_UIObject(pObj, LevelManager()->Get_NowLevelKey());

    pObj->Set_OnClick([]() { LevelManager()->Request_ChangeLevel("Scott_Level", true); });

    _uint iDenny = {};
    RuntimeBucket().Int64.TryGet(PersistScope::SaveSlot, "Denny", iDenny);
    iDenny += 10000;
    RuntimeBucket().Int64.Set(PersistScope::SaveSlot, "Denny", iDenny);

    CUI_LotteryResultBanner::RESULT_DESC desc = {};
    desc.iDenny = 10000;
    pObj->UI_Active(&desc);

    return S_OK;
}

void CUI_TutorialGuide::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;
    switch (eState)
    {
    case STATE::READY: 
        Set_Alive(true);
        if (m_pGuideStart)
            m_pGuideStart->UI_Active();
        for (auto& pair : m_slotsProgress)
            pair.second = false;
        break;
    case STATE::ACTIVE:
        if (m_pGuideStart)
            m_pGuideStart->UI_DeActive();
        UIDirector()->Hide_Mouse();
        GameInstance()->Set_EngineTimeScale(1.f);
        Get_Component<CAudioSource>()->Slot("UI_Tick.wav").Play();
        break;
    case STATE::DEACTIVATING:
        for (auto& pair : m_pSlots)
            pair.second->UI_DeActive();
        if (m_pSlotComplete)
            m_pSlotComplete->UI_Active();
        m_fTimer = 0.f;
        m_isFadeout = false;
        //GameInstance()->Set_EngineTimeScale(0.f);
        BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::WIPEOUT);
        break;
    case STATE::INACTIVE:
        AdvanceTutorial(); 
        Set_Alive(false);
        break;
    }
}

void CUI_TutorialGuide::Ready_Slots(TUTORIAL_TYPE eType)
{
    m_eType = eType;

    const auto& actions = CDataBase::GetInstance()->GetTutorialActions(eType);

    for (auto& pair : m_pSlots)
        pair.second->Set_Alive(false);

    for (auto& action : actions)
        Activate_Slot(action);
}

void CUI_TutorialGuide::Activate_Slot(TUTORIAL_ACTION_DESC desc)
{
    auto iter = m_pSlots.find(desc.eAction);
    if (iter != m_pSlots.end())
    {
        iter->second->UI_Active();
        return;
    } 

    Create_Slot(desc);
}

HRESULT CUI_TutorialGuide::Create_Slot(TUTORIAL_ACTION_DESC desc)
{
    CUI_TutorialGuideSlot::SLOT_DESC* pDesc = new CUI_TutorialGuideSlot::SLOT_DESC;
    pDesc->desc = desc;

    auto pSlot = Builder::Create_UIObject({ "Tutorial_Level", "Proto_GameObject_TutorialGuideSlot" })
        .Add_UIDesc(pDesc)
        .Build("slot");

    if (!pSlot)
        return E_FAIL;

    Get_Component<CObjectContainer>()->Add_Child(pSlot);
    m_pSlots.emplace(desc.eAction, pSlot);
    pSlot->UI_Active();

    return S_OK;
}

void CUI_TutorialGuide::AdvanceTutorial()
{ 
    auto next = GetNextTutorialType(m_eType);
    
    if (next == TUTORIAL_TYPE::END)
        Show_ResultBanner();// LevelManager()->Request_ChangeLevel("Scott_Level", true);
    else
    {
        TUTORIAL_DESC desc = {};
        desc.eType = next;
        desc.eState = TUTORIAL_STATE::INFO;
        EventSystem()->Broadcast<TUTORIAL_DESC>({ desc });
    }
}

TUTORIAL_TYPE CUI_TutorialGuide::GetNextTutorialType(TUTORIAL_TYPE eType)
{
    switch (eType)
    {
    case TUTORIAL_TYPE::EXTREME_EVADE:      return TUTORIAL_TYPE::EXTREME_SUPPORT;
    case TUTORIAL_TYPE::EXTREME_SUPPORT:      return TUTORIAL_TYPE::DECIBEL_ULTIMATE;
    case TUTORIAL_TYPE::DECIBEL_ULTIMATE:      return TUTORIAL_TYPE::GROGGY_COMBO;
    case TUTORIAL_TYPE::GROGGY_COMBO:      return TUTORIAL_TYPE::END;
    default : return TUTORIAL_TYPE::EXTREME_EVADE;
    }
}

CGameObject* CUI_TutorialGuide::Create()
{
    CUI_TutorialGuide* pInstance = new CUI_TutorialGuide();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_TutorialGuide");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_TutorialGuide::Clone(INIT_DESC* pArg)
{
    CUI_TutorialGuide* pInstance = new CUI_TutorialGuide(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_TutorialGuide");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_TutorialGuide::Free()
{
    __super::Free();

    m_pSlots.clear();
}