#include "pch.h"
#include "UI_EvadeAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "GaugeUI.h"

const string CUI_EvadeAction::INSTANCENAMES[ENUM(CHILD::END)] = { "bg", "icon", "gaugeBg", "gauge", "mouse" };

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

	auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
	const string& filePath = pResourceMgr->Get_ResourcePath("hud_battle_evadeAction.json");
	Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

	for (_int i = 0; i < ENUM(CHILD::END); ++i)
        m_hChildren[i] = Get_DescendantHandle(INSTANCENAMES[i]);

    // 액션 이벤트
    Get_Component<CEventListener>()->Add_Listner<UI_ACTION_DESC>([&](const UI_ACTION_DESC& desc)
        {
            if (desc.eType != UI_ACTION_TYPE::EVADE)
                return;

            if (desc.eState == UI_ACTION_STATE::DISABLE)
                Set_Active(false);
            else if (desc.eState == UI_ACTION_STATE::ENABLE)
                Set_Active(true);
            else if (desc.eState == UI_ACTION_STATE::EXECUTING)
                Set_FillAmount(desc.fFillAmount);
        });

	return S_OK;
}

void CUI_EvadeAction::Update(_float dt)
{
    // 이벤트 테스트 코드
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('M'))
    //{
    //    UI_ACTION_DESC desc = {};
    //    desc.eType = UI_ACTION_TYPE::EVADE;
    //    desc.eState = UI_ACTION_STATE::DISABLE;
    //    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    //}
    //
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('N'))
    //{
    //    UI_ACTION_DESC desc = {};
    //    desc.eType = UI_ACTION_TYPE::EVADE;
    //    desc.eState = UI_ACTION_STATE::ENABLE;
    //    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    //
    //}
    //
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('B'))
    //{
    //    UI_ACTION_DESC desc = {};
    //    desc.eType = UI_ACTION_TYPE::EVADE;
    //    desc.eState = UI_ACTION_STATE::EXECUTING;
    //    desc.fFillAmount = 0.75f;
    //    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    //}

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_EvadeAction::UI_Active(void* pArg)
{
    Set_Active(true);
}

void CUI_EvadeAction::UI_DeActive(void* pArg)
{
    Set_Active(false);
}

void CUI_EvadeAction::Set_Active(_bool isActive)
{
    if (isActive)
    {
        Set_Color(CHILD::BG, UI_GRAY_DARKEST);
        Set_Color(CHILD::ICON, UI_GRAY_LIGHTEST);
        Set_Color(CHILD::GAUGE_BG, UI_GRAY_MEDIUM);
        Set_Color(CHILD::GAUGE, UI_GRAY_LIGHTEST);
    } 
    else
    {
        Set_Color(CHILD::BG, UI_GRAY_MEDIUM);
        Set_Color(CHILD::ICON, UI_GRAY_DARK);
        Set_Color(CHILD::GAUGE_BG, UI_GRAY_DARKEST);
        Set_Color(CHILD::GAUGE, UI_GRAY_DARK);
    }    

    Set_Alive(CHILD::MOUSE, isActive);
}

void CUI_EvadeAction::Set_FillAmount(_float fFillAmount)
{
    if (m_hChildren[ENUM(CHILD::GAUGE)].isValid())
    {
        auto pGauge = dynamic_cast<CGaugeUI*>(m_hChildren[ENUM(CHILD::GAUGE)].Get());
        if (!pGauge)
            return;

        pGauge->Set_FillAmount(fFillAmount);
    }
}

void CUI_EvadeAction::Set_Alive(CHILD child, _bool isAlive)
{
    ForChild(child, [isAlive](CUI_Object* ui) { ui->Set_Alive(isAlive); });
}

void CUI_EvadeAction::Set_Color(CHILD child, _float4 vColor)
{
    ForChild(child, [vColor](CUI_Object* ui) { ui->Set_Color(vColor); });
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