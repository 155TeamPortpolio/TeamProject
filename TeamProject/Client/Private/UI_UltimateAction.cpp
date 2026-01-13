#include "pch.h"
#include "UI_UltimateAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "GaugeUI.h"

const string CUI_UltimateAction::INSTANCENAMES[ENUM(CHILD::END)] = { "group1", "bg", "uv", "group2", "black", "star", "q" };

HRESULT CUI_UltimateAction::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

    return S_OK;
}

HRESULT CUI_UltimateAction::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
    const string& filePath = pResourceMgr->Get_ResourcePath("hud_battle_ultimateAction.json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
        m_hChildren[i] = Get_DescendantHandle(INSTANCENAMES[i]);

    // 액션 이벤트
    Get_Component<CEventListener>()->Add_Listner<UI_ACTION_DESC>([&](const UI_ACTION_DESC& desc)
        {
            if (desc.eType != UI_ACTION_TYPE::ULTIMATE)
                return;

            if (desc.eState == UI_ACTION_STATE::DISABLE)
                Set_InteractState(INTERACT_STATE::DISABLED);
            else if (desc.eState == UI_ACTION_STATE::ENABLE)
                Set_InteractState(INTERACT_STATE::ENABLED);
            else if (desc.eState == UI_ACTION_STATE::EXECUTING)
                Execute();
        });

    return S_OK;
}

void CUI_UltimateAction::Update(_float dt)
{
    // 이벤트 테스트 코드
    if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('M'))
    {
        UI_ACTION_DESC desc = {};
        desc.eType = UI_ACTION_TYPE::ULTIMATE;
        desc.eState = UI_ACTION_STATE::DISABLE;
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }
    
    if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('N'))
    {
        UI_ACTION_DESC desc = {};
        desc.eType = UI_ACTION_TYPE::ULTIMATE;
        desc.eState = UI_ACTION_STATE::ENABLE;
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }
    
    if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('B'))
    {
        UI_ACTION_DESC desc = {};
        desc.eType = UI_ACTION_TYPE::ULTIMATE;
        desc.eState = UI_ACTION_STATE::EXECUTING;
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_UltimateAction::UI_Active(void* pArg)
{
    Set_InteractState(INTERACT_STATE::ENABLED);
}

void CUI_UltimateAction::UI_DeActive(void* pArg)
{
    Set_InteractState(INTERACT_STATE::DISABLED);
}

void CUI_UltimateAction::Set_InteractState(INTERACT_STATE state)
{
    m_interactState = state;
    Refresh_Visual();
}

void CUI_UltimateAction::Execute()
{
    //if (m_interactState != INTERACT_STATE::ENABLED)
    //    return;

    m_interactState = INTERACT_STATE::ENABLED;
    Refresh_Visual();
    Set_Animation(CHILD::GROUP1, 0);
    Set_Animation(CHILD::GROUP2, 0);
    Set_Animation(CHILD::BLACK, 0);
    Set_Animation(CHILD::STAR, 0); 
}

void CUI_UltimateAction::Refresh_Visual()
{
    if (m_interactState == INTERACT_STATE::DISABLED)
    {
        Apply_DisableVisual();
        return;
    }
    
    Apply_EnableVisual();
}

void CUI_UltimateAction::Apply_DisableVisual()
{
    Set_Color(CHILD::BG, UI_GRAY_MEDIUM);
    Set_Alive(CHILD::UV, false);
}

void CUI_UltimateAction::Apply_EnableVisual()
{
    Set_Color(CHILD::BG, UI_GRAY_DARKEST);
    Set_Alive(CHILD::UV, true);
}

void CUI_UltimateAction::Set_Alive(CHILD child, _bool isAlive)
{
    ForChild(child, [isAlive](CUI_Object* ui) { ui->Set_Alive(isAlive); });
}

void CUI_UltimateAction::Set_Color(CHILD child, _float4 vColor)
{
    ForChild(child, [vColor](CUI_Object* ui) { ui->Set_Color(vColor); });
}

void CUI_UltimateAction::Set_Animation(CHILD child, _int iIndex)
{
    ForChild(child, [iIndex](CUI_Object* ui) { ui->Set_Animation(iIndex); });
}

CGameObject* CUI_UltimateAction::Create()
{
    CUI_UltimateAction* pInstance = new CUI_UltimateAction();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_UltimateAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_UltimateAction::Clone(INIT_DESC* pArg)
{
    CUI_UltimateAction* pInstance = new CUI_UltimateAction(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_EvadeAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}