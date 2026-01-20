#include "pch.h"
#include "UI_UltimateAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "GaugeUI.h"

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

    Load_Json("hud_battle_ultimateAction.json");
    Cache_Children();
    Bind_EventListener();

    Apply_DisableVisual();

    return S_OK;
}

void CUI_UltimateAction::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_UltimateAction::UI_Active(void* pArg)
{
    Set_InteractState(INTERACT_STATE::DISABLE);
}

void CUI_UltimateAction::UI_DeActive(void* pArg)
{
    Set_InteractState(INTERACT_STATE::DISABLE);
}

void CUI_UltimateAction::Load_Json(const string& resourceKey)
{
    // json 로드
    auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
    const string& filePath = pResourceMgr->Get_ResourcePath(resourceKey);
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));
}

void CUI_UltimateAction::Cache_Children()
{
    // 자식 UI 오브젝트 포인터를 배열에 캐싱
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
        m_pChildren[i] = dynamic_cast<CUI_Object*>(Get_Component<CObjectContainer>()->Find_Descendant(INSTANCENAMES[i]));
}

void CUI_UltimateAction::Bind_EventListener()
{
    // 모드 변경 이벤트
    Get_Component<CEventListener>()->Add_Listener<UI_ACTION_PRIMARY_DESC>([&](const UI_ACTION_PRIMARY_DESC& desc)
        {
            Set_InteractState(INTERACT_STATE::DISABLE);
        });

    // 액션 이벤트
    Get_Component<CEventListener>()->Add_Listener<UI_ACTION_DESC>([&](const UI_ACTION_DESC& desc)
        {
            if (desc.eType != UI_ACTION_TYPE::ULTIMATE)
                return;

            if (desc.eState == UI_ACTION_STATE::DISABLE)
                Set_InteractState(INTERACT_STATE::DISABLE);
            else if (desc.eState == UI_ACTION_STATE::AVAILABLE)
                Execute();
            else if (desc.eState == UI_ACTION_STATE::EXECUTING)
            {
                Set_InteractState(INTERACT_STATE::DISABLE);
                Set_ChildAnimation(CHILD::BLINK, 0);
            }
        });
}

void CUI_UltimateAction::Set_InteractState(INTERACT_STATE state)
{
    m_interactState = state;
    Refresh_Visual();
}

void CUI_UltimateAction::Execute()
{
    //if (m_interactState == INTERACT_STATE::DISABLE)
    //    return;

    m_interactState = INTERACT_STATE::ENABLE;
    Refresh_Visual();
    Set_ChildAnimation(CHILD::GROUP1, 0);
    Set_ChildAnimation(CHILD::GROUP2, 0);
    Set_ChildAnimation(CHILD::BLACK, 0);
    Set_ChildAnimation(CHILD::STAR, 0);
    Set_ChildAnimation(CHILD::STAR1, 0);
    Set_ChildAnimation(CHILD::STAR2, 0);
    Set_ChildAnimation(CHILD::STAR3, 0);
}

void CUI_UltimateAction::Refresh_Visual()
{
    if (m_interactState == INTERACT_STATE::DISABLE)
    {
        Apply_DisableVisual();
        return;
    }
    
    Apply_EnableVisual();
}

void CUI_UltimateAction::Apply_DisableVisual()
{
    Set_ChildAlive(CHILD::UV, false);
    Set_ChildColor(CHILD::BG, UI_GRAY_MEDIUM);
    Set_ChildColor(CHILD::Q, UI_GRAY_LIGHTEST); 
}

void CUI_UltimateAction::Apply_EnableVisual()
{
    Set_ChildAlive(CHILD::UV, true);
    Set_ChildColor(CHILD::BG, UI_GRAY_DARKEST);
    Set_ChildColor(CHILD::Q, UI_WHITE); 
}

void CUI_UltimateAction::Set_ChildAlive(CHILD child, _bool isAlive)
{
    if (!m_pChildren[ENUM(child)])
        return;

    m_pChildren[ENUM(child)]->Set_Alive(isAlive);
}

void CUI_UltimateAction::Set_ChildColor(CHILD child, _float4 vColor)
{
    if (!m_pChildren[ENUM(child)])
        return;

    m_pChildren[ENUM(child)]->Set_Color(vColor);
}

void CUI_UltimateAction::Set_ChildAnimation(CHILD child, _int iIndex)
{
    if (!m_pChildren[ENUM(child)])
        return;

    m_pChildren[ENUM(child)]->Set_Animation(iIndex);
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