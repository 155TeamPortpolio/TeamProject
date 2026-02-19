#include "pch.h"
#include "UI_TutorialGuide.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"

#include "BattleSystem.h"

HRESULT CUI_TutorialGuide::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CUI_TutorialGuide::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("tutorial_bubble.json")));

    // ¿Ã∫•∆Æ : TUTORIAL_DESC
    Get_Component<CEventListener>()->Add_Listener<TUTORIAL_DESC>([&](const TUTORIAL_DESC& desc)
        {
            if (desc.eState != TUTORIAL_STATE::PLAY)
                return;

            Change_State(STATE::ACTIVE);
            m_eType = desc.eType;
        });

	return S_OK;
}

void CUI_TutorialGuide::Awake()
{
    Set_Alive(false);
}

void CUI_TutorialGuide::Update(_float dt)
{
    if (InputDevice()->Key_Tap('M'))
        Change_State(STATE::DEACTIVATING);

    if (m_eState == STATE::DEACTIVATING && !BattleSystem()->isVFXRunning(BATTLE_VFX_TYPE::WIPEOUT))
        Change_State(STATE::INACTIVE); 

	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_TutorialGuide::UI_Active(void* pArg)
{
}

void CUI_TutorialGuide::UI_DeActive(void* pArg)
{
}

void CUI_TutorialGuide::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;
    switch (eState)
    {
    case STATE::ACTIVE:
        Set_Alive(true);
        break;
    case STATE::DEACTIVATING:
        BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::WIPEOUT);
        break;
    case STATE::INACTIVE:
        AdvanceTutorial();
        Set_Alive(false);
        break;
    }
}

void CUI_TutorialGuide::AdvanceTutorial()
{ 
    auto next = GetNextTutorialType(m_eType);
    
    if (next == TUTORIAL_TYPE::END)
        LevelManager()->Request_ChangeLevel("Scott_Level", true);
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
}