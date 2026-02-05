#include "pch.h"
#include "UI_Lottery.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "Sprite2D.h"
#include "ButtonUI.h" 
#include "UI_ScratchCard.h"
#include "UI_BackButton.h"
#include "UI_LotteryResultBanner.h"

#include "FieldSystem.h"

HRESULT CUI_Lottery::Initialize_Prototype()
{
	__super::Initialize_Prototype();

    Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CUI_Lottery::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    // JSON 로드
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("lottery.json"))); 
    Cache();

    Create_Newspaper();
    Create_ScratchCard();
    Create_BackButton();
    Create_ResultBanner();

    if (m_pButtons[ENUM(BTN::BTN_REFRESH)])
        m_pButtons[ENUM(BTN::BTN_REFRESH)]->Set_OnClick([this]() { OnClick_RefreshNews(); });

    if (m_pButtons[ENUM(BTN::BTN_SCRATCH)])
        m_pButtons[ENUM(BTN::BTN_SCRATCH)]->Set_OnClick([this]() { OnClick_OpenScratch(); });

    Change_State(STATE::READY);

    Set_Alive(false);

    // 이벤트 : DAYPHASE_DESC
    Get_Component<CEventListener>()->Add_Listener<DAYPHASE_DESC>([&](const DAYPHASE_DESC& desc)
        {
            if (desc.dayPhase != DayPhase::EarlyMorning)
                return;

            Change_State(STATE::READY);
        });

	return S_OK;
}

void CUI_Lottery::Awake()
{
}

void CUI_Lottery::Update(_float dt)
{
	__super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if(Is_ChildAnimationFinished(CHILD::ICON_SCRATCH))
        Set_ChildAnimation(CHILD::ICON_SCRATCH, 1);
}

void CUI_Lottery::Late_Update(_float dt)
{
}

void CUI_Lottery::UI_Active(void* pArg)
{
    Set_Alive(true);
    Set_ChildAnimation(CHILD::ICON_SCRATCH, 0);
    Set_ChildAnimation(CHILD::NEWSPAPER, 0);
    if(m_iState == STATE::READY)
        m_iReward = rand() % ENUM(REWARD::END);
}

void CUI_Lottery::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

void CUI_Lottery::Create_Newspaper()
{
    auto pContainer = Get_Component<CObjectContainer>();
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_Newspaper" }).Build("newspaper");
    if (!pObj)
        return;

    pContainer->Add_Child(pObj); 
    pContainer->Set_Order_First(pObj);
    m_pChildren[ENUM(CHILD::NEWSPAPER)] = pObj;
}

void CUI_Lottery::Create_ScratchCard()
{
    CUI_ScratchCard::SCRATCH_DESC* pDesc = new CUI_ScratchCard::SCRATCH_DESC;
    pDesc->pParentState = &m_iState;
    pDesc->onScratchCompleted = [this]() { OnScratch_Complete(); };

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_ScratchCard" })
        .Add_UIDesc(pDesc)
        .Build("scratchCard");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pChildren[ENUM(CHILD::SCRATCH)] = pObj;
}

void CUI_Lottery::Create_BackButton()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_BackButton" })
        .Build("buttonBack");

    if (!pObj)
        return;

    pObj->Set_OnClick([this]() { OnClick_Back(); });
    Get_Component<CObjectContainer>()->Add_Child(pObj); 
}

void CUI_Lottery::Create_ResultBanner()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_LotteryResultBanner" })
        .Build("resultBanner");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pResultBanner = pObj;
}

void CUI_Lottery::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();
     
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
    }

    for (_int i = 0; i < ENUM(BTN::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(BTN_NAMES[i]);
        if (!pObj)
            continue;

        m_pButtons[i] = dynamic_cast<CButtonUI*>(pObj);
    }
}

void CUI_Lottery::Change_State(STATE eState)
{
    if (m_iState == static_cast<_uint>(eState))
        return;

    m_iState = static_cast<_uint>(eState);
    switch (m_iState)
    {
    case STATE::READY:
        Change_ChildTexture(CHILD::ICON_SCRATCH, "ScratchCardIcon.png");
        break;
    case STATE::USED:
        Change_ChildTexture(CHILD::ICON_SCRATCH, "ScratchCardIconReceived.png");
        break;
    }
}

void CUI_Lottery::OnClick_Back()
{
    if (Is_ChildAlive(CHILD::SCRATCH)) 
    {
        Set_ChildAnimation(CHILD::OVERLAY, 1);
        Set_ChildUIDeActive(CHILD::SCRATCH);
    }
    else
        FieldSystem()->RequestExitTop();
}

void CUI_Lottery::OnClick_RefreshNews()
{
    if (Is_ChildAlive(CHILD::SCRATCH))
        return;

    Set_ChildAnimation(CHILD::OVERLAY_REFRESH, 0);
    Set_ChildAnimation(CHILD::ICON_REFRESH, 0);

    Set_ChildUIActive(CHILD::NEWSPAPER);
}

void CUI_Lottery::OnClick_OpenScratch()
{
    if (Is_ChildAlive(CHILD::SCRATCH))
        return;

    Set_ChildAnimation(CHILD::OVERLAY, 0);

    CUI_ScratchCard::ACTIVE_DESC desc = {};
    desc.strTextureKey = (m_iReward < ENUM(REWARD::END)) ? REWARD_TEXTURES[m_iReward] : REWARD_TEXTURES[0];
    Set_ChildUIActive(CHILD::SCRATCH, &desc);
}

void CUI_Lottery::OnScratch_Complete()
{
    Change_State(STATE::USED);

    _uint iDennyReward = (m_iReward < ENUM(REWARD::END)) ? REWARD_DENY[m_iReward] : 0;

    _uint iDenny = {};
    RuntimeBucket().Int64.TryGet(PersistScope::SaveSlot, "Denny", iDenny);

    iDenny += iDennyReward;
    RuntimeBucket().Int64.Set(PersistScope::SaveSlot, "Denny", iDenny);

    if (m_pResultBanner)
    {
        CUI_LotteryResultBanner::RESULT_DESC desc = {};
        desc.iDenny = iDennyReward;
        m_pResultBanner->UI_Active(&desc);
    } 
}

void CUI_Lottery::Set_ChildUIActive(CHILD child, void* pArg)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->UI_Active(pArg);
}

void CUI_Lottery::Set_ChildUIDeActive(CHILD child, void* pArg)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->UI_DeActive(pArg);
}

void CUI_Lottery::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

void CUI_Lottery::Change_ChildTexture(CHILD child, const string& strTextureKey)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    auto pSprite = pChild->Get_Component<CSprite2D>();
    if (!pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

_bool CUI_Lottery::Is_ChildAlive(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_Alive();
}

_bool CUI_Lottery::Is_ChildAnimationFinished(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_AnimFinished();
}

CGameObject* CUI_Lottery::Create()
{
    CUI_Lottery* pInstance = new CUI_Lottery();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Lottery");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Lottery::Clone(INIT_DESC* pArg)
{
    CUI_Lottery* pInstance = new CUI_Lottery(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Lottery");
        Safe_Release(pInstance);
    }
    return pInstance;
}