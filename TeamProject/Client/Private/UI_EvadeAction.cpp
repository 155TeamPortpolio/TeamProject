#include "pch.h"
#include "UI_EvadeAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "GaugeUI.h"
#include "TextSlot.h"

HRESULT CUI_EvadeAction::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();
	Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CUI_EvadeAction::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    Load_Json("hud_battle_evadeAction.json");
    Cache_Children();
    Bind_EventListener();

	return S_OK;
}

void CUI_EvadeAction::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_EvadeAction::UI_Active(void* pArg)
{
    Set_InteractState(INTERACT_STATE::ENABLE);
}

void CUI_EvadeAction::UI_DeActive(void* pArg)
{
    Set_InteractState(INTERACT_STATE::DISABLE);
}

void CUI_EvadeAction::Load_Json(const string& resourceKey)
{
    // json 로드
    auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
    const string& filePath = pResourceMgr->Get_ResourcePath(resourceKey);
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));
}

void CUI_EvadeAction::Cache_Children()
{ 
    auto pContainer = Get_Component<CObjectContainer>();

    // 자식 UI 오브젝트 포인터를 배열에 캐싱
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        const string& strInstanceName = INSTANCENAMES[i];
        if (strInstanceName.empty())
            continue;

        auto pObj = pContainer->Find_Descendant(strInstanceName);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
    } 

    // 캐스팅이 필요한 자식, 컴포넌트는 별도로 캐싱
    m_pGauge = dynamic_cast<CGaugeUI*>(m_pChildren[ENUM(CHILD::GAUGE)]);
    m_pTextSlot = m_pChildren[ENUM(CHILD::TEXT)]->Get_Component<CTextSlot>();
}

void CUI_EvadeAction::Bind_EventListener()
{
    // 모드 변경 이벤트
    Get_Component<CEventListener>()->Add_Listener<UI_ACTION_PRIMARY_DESC>([&](const UI_ACTION_PRIMARY_DESC& desc)
        {
            m_eMode = desc.eMode;
            switch (desc.eMode)
            {
            case UI_ACTION_PRIMARY_MODE::ATTACK:
                Set_InteractState(INTERACT_STATE::ENABLE);
                break;
            case UI_ACTION_PRIMARY_MODE::INTERACT:
                Set_InteractState(INTERACT_STATE::DISABLE);
                break;
            }
        });

    // 액션 이벤트
    Get_Component<CEventListener>()->Add_Listener<UI_ACTION_DESC>([&](const UI_ACTION_DESC& desc)
        {
            if (m_eMode == UI_ACTION_PRIMARY_MODE::INTERACT)
                return;

            if (desc.eType != UI_ACTION_TYPE::EVADEPERFECT &&
                desc.eType != UI_ACTION_TYPE::EVADE)
                return;

            m_isPerfect = desc.eType == UI_ACTION_TYPE::EVADEPERFECT;

            if (desc.eState == UI_ACTION_STATE::DISABLE)
                Set_InteractState(INTERACT_STATE::DISABLE);
            else if (desc.eState == UI_ACTION_STATE::ENABLE || desc.eState == UI_ACTION_STATE::EXECUTING)
            {
                Set_InteractState(INTERACT_STATE::ENABLE);
                Set_Gauge(desc.fFillAmount);
            }
        });
}

void CUI_EvadeAction::Set_InteractState(INTERACT_STATE state)
{
    m_interactState = state;
    Refresh_Visual();
}

void CUI_EvadeAction::Set_Gauge(_float fFillAmount)
{
    if (m_pGauge)
        m_pGauge->Set_FillAmount(fFillAmount);

    if (m_isPerfect && m_pTextSlot)
    {
        char szBuffer[32] = {};
        sprintf_s(szBuffer, "%.1f", fFillAmount);
        
        m_pTextSlot->Set_Text(Helper::ConvertToWideString(szBuffer));
    }
}

void CUI_EvadeAction::Refresh_Visual()
{
    if (m_interactState == INTERACT_STATE::DISABLE)
    {
        Apply_DisableVisual();
        return;
    }

    Apply_EnableVisual();
}

void CUI_EvadeAction::Apply_DisableVisual()
{
    Set_ChildAlive(CHILD::TEXT, false);
    Set_ChildAlive(CHILD::MOUSE, false);
    Set_ChildColor(CHILD::BG, UI_GRAY_MEDIUM); 
    Set_ChildColor(CHILD::GAUGE_BG, UI_GRAY_DARKEST);
    Set_ChildColor(CHILD::ICON, UI_GRAY_DARK);
    Set_ChildColor(CHILD::GAUGE, _vector4(UI_GRAY_DARK) / _vector4(UI_GRAY_LIGHTEST)); 
}

void CUI_EvadeAction::Apply_EnableVisual()
{
    Set_ChildAlive(CHILD::ICON, !m_isPerfect);
    Set_ChildAlive(CHILD::TEXT, m_isPerfect);
    Set_ChildAlive(CHILD::MOUSE, true);
    Set_ChildColor(CHILD::BG, UI_GRAY_DARKEST); 
    Set_ChildColor(CHILD::GAUGE_BG, UI_GRAY_DARK);
    Set_ChildColor(CHILD::ICON, UI_GRAY_LIGHTEST);
    Set_ChildColor(CHILD::GAUGE, UI_WHITE); 
}

void CUI_EvadeAction::Set_ChildAlive(CHILD child, _bool isAlive)
{
    if (!m_pChildren[ENUM(child)])
        return;

    m_pChildren[ENUM(child)]->Set_Alive(isAlive);
}

void CUI_EvadeAction::Set_ChildColor(CHILD child, _float4 vColor)
{
    if (!m_pChildren[ENUM(child)])
        return;

    m_pChildren[ENUM(child)]->Set_Color(vColor);
}

CGameObject* CUI_EvadeAction::Create()
{
    CUI_EvadeAction* pInstance = new CUI_EvadeAction();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_EvadeAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_EvadeAction::Clone(INIT_DESC* pArg)
{
    CUI_EvadeAction* pInstance = new CUI_EvadeAction(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_EvadeAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}