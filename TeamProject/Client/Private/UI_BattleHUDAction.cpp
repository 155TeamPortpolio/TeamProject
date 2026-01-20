#include "pch.h"
#include "UI_BattleHUDAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"

HRESULT CUI_BattleHUDAction::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CUI_BattleHUDAction::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
     
    Ready_PartObjects();
    Bind_EventListener();

    Set_Size(_float2(340.f, 224.f));

    return S_OK;
}

void CUI_BattleHUDAction::Update(_float dt)
{
    __super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_BattleHUDAction::Ready_PartObjects()
{
    auto pContainer = Get_Component<CObjectContainer>();
    const string& strLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

    Attach_Child(strLevelKey, "Proto_GameObject_PrimaryAction", "primary", CHILD::PRIMARY, _float2(0.f, m_vOffset.y));
    Attach_Child(strLevelKey, "Proto_GameObject_EvadeAction", "evade", CHILD::EVADE, m_vOffset);
    Attach_Child(strLevelKey, "Proto_GameObject_SpecialAction", "special", CHILD::SPECIAL, _float2(m_vOffset.x * 2.f, m_vOffset.y));
    Attach_Child(strLevelKey, "Proto_GameObject_SwitchAction", "switch", CHILD::SWITCH, _float2(m_vOffset.x * 3.f, m_vOffset.y));
    Attach_Child(strLevelKey, "Proto_GameObject_UltimateAction", "ultimate", CHILD::ULTIMATE, _float2(m_vOffset.x * 3.f, 0.f));
}

void CUI_BattleHUDAction::Bind_EventListener()
{
    // 액션 이벤트
    Get_Component<CEventListener>()->Add_Listener<UI_ACTION_DESC>([&](const UI_ACTION_DESC& desc)
        {
            if (desc.eType != UI_ACTION_TYPE::ALL)
                return;

            if (desc.eState == UI_ACTION_STATE::DISABLE)
                Set_EnableAll(false);
            else if (desc.eState == UI_ACTION_STATE::ENABLE)
                Set_EnableAll(true);
        });
}

void CUI_BattleHUDAction::Attach_Child(const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, CHILD child, _float2 vOffset)
{
    CUI_Object* pObj = Builder::Create_UIObject({ strLevelKey, strPrototypeTag })
            .Build(strInstanceName);

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);

    pObj->Set_AnchorOffset(vOffset);
    m_pChildren[ENUM(child)] = pObj;
}

void CUI_BattleHUDAction::Set_EnableAll(_bool isActive)
{
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
        Set_Enable(static_cast<CHILD>(i), isActive);
}

void CUI_BattleHUDAction::Set_Enable(CHILD child, _bool isActive)
{
    auto pChild =  m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    if (isActive)
        pChild->UI_Active();
    else
        pChild->UI_DeActive();
}

CGameObject* CUI_BattleHUDAction::Create()
{
    CUI_BattleHUDAction* pInstance = new CUI_BattleHUDAction();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_BattleHUDAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BattleHUDAction::Clone(INIT_DESC* pArg)
{
    CUI_BattleHUDAction* pInstance = new CUI_BattleHUDAction(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_BattleHUDAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}