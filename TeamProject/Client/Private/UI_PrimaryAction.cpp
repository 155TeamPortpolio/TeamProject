#include "pch.h"
#include "UI_PrimaryAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"

HRESULT CUI_PrimaryAction::Initialize_Prototype()
{
	__super::Initialize_Prototype();

    Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CUI_PrimaryAction::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    Load_Json("hud_battle_primaryAction.json");
    Cache_Children();
    Bind_EventListener();

	return S_OK;
}

void CUI_PrimaryAction::Update(_float dt)
{
    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_PrimaryAction::UI_Active(void* pArg)
{
    Set_InteractState(INTERACT_STATE::ENABLE);
}

void CUI_PrimaryAction::UI_DeActive(void* pArg)
{
    Set_InteractState(INTERACT_STATE::DISABLE);
}

void CUI_PrimaryAction::Load_Json(const string& resourceKey)
{
    // json 로드
    auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
    const string& filePath = pResourceMgr->Get_ResourcePath(resourceKey);
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));
}

void CUI_PrimaryAction::Cache_Children()
{
    // 자식 UI 오브젝트 포인터를 배열에 캐싱
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
        m_pChildren[i] = dynamic_cast<CUI_Object*>(Get_Component<CObjectContainer>()->Find_Descendant(INSTANCENAMES[i]));
}

void CUI_PrimaryAction::Bind_EventListener()
{
    // 모드 변경 이벤트
    Get_Component<CEventListener>()->Add_Listener<UI_ACTION_PRIMARY_DESC>([&](const UI_ACTION_PRIMARY_DESC& desc)
        {
            switch (desc.eMode)
            {
            case UI_ACTION_PRIMARY_MODE::ATTACK:
                Set_ActionMode(MODE::ATTACK);
                break;
            case UI_ACTION_PRIMARY_MODE::INTERACT:
                Set_ActionMode(MODE::INTERACT);
                break;
            }
        });

    // 액션 이벤트
    Get_Component<CEventListener>()->Add_Listener<UI_ACTION_DESC>([&](const UI_ACTION_DESC& desc)
        {
            if (desc.eType != UI_ACTION_TYPE::PRIMARY)
                return;

            if (desc.eState == UI_ACTION_STATE::DISABLE)
                Set_InteractState(INTERACT_STATE::DISABLE);
            else if (desc.eState == UI_ACTION_STATE::ENABLE)
                Set_InteractState(INTERACT_STATE::ENABLE);
            else if (desc.eState == UI_ACTION_STATE::AVAILABLE)
                Set_InteractState(INTERACT_STATE::AVAILABLE);
        });
}

void CUI_PrimaryAction::Set_ActionMode(MODE eMode)
{
    if (m_mode == eMode)
        return;

    m_mode = eMode;
    _bool isAttackMode = (m_mode == MODE::ATTACK) ? true : false;

    Set_ChildAlive(CHILD::ATTACK, isAttackMode);
    Set_ChildAlive(CHILD::INTERACT, !isAttackMode);

    if (!isAttackMode)
        Set_ChildAnimation(CHILD::INTERACT, 0);
}

void CUI_PrimaryAction::Set_InteractState(INTERACT_STATE state)
{
    m_interactState = state;
    Refresh_Visual();
}

void CUI_PrimaryAction::Refresh_Visual()
{
    if (m_interactState == INTERACT_STATE::DISABLE)
    {
        Apply_DisableVisual();
        return;
    }

    if (m_interactState == INTERACT_STATE::AVAILABLE)
    {
        Apply_AvailableVisual();
        return;
    }

    Apply_EnableVisual();
}

void CUI_PrimaryAction::Apply_DisableVisual()
{
    switch (m_mode)
    {
    case MODE::ATTACK:
        Set_ChildColor(CHILD::ATTACK_BG, UI_GRAY_MEDIUM);
        Set_ChildColor(CHILD::ATTACK_ICON, UI_GRAY_DARK);
        Set_ChildColor(CHILD::ATTACK_MOUSE, UI_TRANSPARENT);
        break;
    case MODE::INTERACT:
        Set_ChildAlive(CHILD::INTERACT_GRADIENT, false);
        break;
    }
}

void CUI_PrimaryAction::Apply_EnableVisual()
{
    switch (m_mode)
    {
    case MODE::ATTACK:
        Set_ChildColor(CHILD::ATTACK_BG, UI_GRAY_DARKEST);
        Set_ChildColor(CHILD::ATTACK_ICON, UI_GRAY_LIGHTEST);
        Set_ChildColor(CHILD::ATTACK_MOUSE, UI_WHITE);
        break;
    case MODE::INTERACT:
        Set_ChildAlive(CHILD::INTERACT_GRADIENT, false);
        break;
    }
}

void CUI_PrimaryAction::Apply_AvailableVisual()
{
    switch (m_mode)
    {
    case MODE::ATTACK:
        break;
    case MODE::INTERACT:
        Set_ChildAlive(CHILD::INTERACT_GRADIENT, true);
        Set_ChildAnimation(CHILD::INTERACT_GRADIENT, 0);
        break;
    }
}

void CUI_PrimaryAction::Set_ChildAlive(CHILD child, _bool isAlive)
{
    if (!m_pChildren[ENUM(child)])
        return;

    m_pChildren[ENUM(child)]->Set_Alive(isAlive);
}

void CUI_PrimaryAction::Set_ChildColor(CHILD child, _float4 vColor)
{
    if (!m_pChildren[ENUM(child)])
        return;

    m_pChildren[ENUM(child)]->Set_Color(vColor);
}

void CUI_PrimaryAction::Set_ChildAnimation(CHILD child, _int iIndex)
{
    if (!m_pChildren[ENUM(child)])
        return;

    m_pChildren[ENUM(child)]->Set_Animation(iIndex);
}

CGameObject* CUI_PrimaryAction::Create()
{
    CUI_PrimaryAction* pInstance = new CUI_PrimaryAction();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_PrimaryAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_PrimaryAction::Clone(INIT_DESC* pArg)
{
    CUI_PrimaryAction* pInstance = new CUI_PrimaryAction(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_PrimaryAction");
        Safe_Release(pInstance);
    }
    return pInstance;
}