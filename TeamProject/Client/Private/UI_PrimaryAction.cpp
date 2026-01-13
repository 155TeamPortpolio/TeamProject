#include "pch.h"
#include "UI_PrimaryAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"

const string CUI_PrimaryAction::INSTANCENAMES[ENUM(CHILD::END)] = { "attack", "attackBg", "attackIcon", "attackMouse", "interact", "interactGradient" };

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

    auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
    const string& filePath = pResourceMgr->Get_ResourcePath("hud_battle_primaryAction.json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
        m_handles[i] = Get_DescendantHandle(INSTANCENAMES[i]);

    // 모드 변경 이벤트
    Get_Component<CEventListener>()->Add_Listner<UI_ACTION_PRIMARY_DESC>([&](const UI_ACTION_PRIMARY_DESC& desc)
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
    Get_Component<CEventListener>()->Add_Listner<UI_ACTION_DESC>([&](const UI_ACTION_DESC& desc)
        {
            if (desc.eType != UI_ACTION_TYPE::PRIMARY)
                return;

            if(desc.eState == UI_ACTION_STATE::DISABLE)
                Set_InteractState(INTERACT_STATE::DISABLE);
            else if (desc.eState == UI_ACTION_STATE::ENABLE)
                Set_InteractState(INTERACT_STATE::ENABLE);
            else if (desc.eState == UI_ACTION_STATE::AVAILABLE)
                Set_InteractState(INTERACT_STATE::AVAILABLE);
        });

	return S_OK;
}

void CUI_PrimaryAction::Update(_float dt)
{
    // 이벤트 테스트 코드
    if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('M'))
    {
        UI_ACTION_PRIMARY_DESC desc = {};
        desc.eMode = UI_ACTION_PRIMARY_MODE::ATTACK;
        EventSystem()->Broadcast<UI_ACTION_PRIMARY_DESC>({ desc });
    }
    
    if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('N'))
    {
        UI_ACTION_PRIMARY_DESC desc = {};
        desc.eMode = UI_ACTION_PRIMARY_MODE::INTERACT;
        EventSystem()->Broadcast<UI_ACTION_PRIMARY_DESC>({ desc });
    }
    
    if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('B'))
    {
        UI_ACTION_DESC desc = {};
        desc.eType = UI_ACTION_TYPE::PRIMARY;
        desc.eState = UI_ACTION_STATE::DISABLE;
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }
    
    if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('V'))
    {
        UI_ACTION_DESC desc = {};
        desc.eType = UI_ACTION_TYPE::PRIMARY;
        desc.eState = UI_ACTION_STATE::ENABLE;
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }
    
    if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('C'))
    {
        UI_ACTION_DESC desc = {};
        desc.eType = UI_ACTION_TYPE::PRIMARY;
        desc.eState = UI_ACTION_STATE::AVAILABLE;
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }

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

void CUI_PrimaryAction::Set_ActionMode(MODE eMode)
{
    if (m_mode == eMode)
        return;

    m_mode = eMode;
    _bool isAttackMode = (m_mode == MODE::ATTACK) ? true : false;

    Set_Alive(CHILD::ATTACK, isAttackMode);
    Set_Alive(CHILD::INTERACT, !isAttackMode);

    if (!isAttackMode)
        Set_Animation(CHILD::INTERACT, 0);
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
        Set_Color(CHILD::ATTACK_BG, UI_GRAY_MEDIUM);
        Set_Color(CHILD::ATTACK_ICON, UI_GRAY_DARK);
        Set_Color(CHILD::ATTACK_MOUSE, UI_TRANSPARENT);
        break;
    case MODE::INTERACT:
        Set_Alive(CHILD::INTERACT_GRADIENT, false);
        break;
    }
}

void CUI_PrimaryAction::Apply_EnableVisual()
{
    switch (m_mode)
    {
    case MODE::ATTACK:
        Set_Color(CHILD::ATTACK_BG, UI_GRAY_DARKEST);
        Set_Color(CHILD::ATTACK_ICON, UI_GRAY_LIGHTEST);
        Set_Color(CHILD::ATTACK_MOUSE, UI_WHITE);
        break;
    case MODE::INTERACT:
        Set_Alive(CHILD::INTERACT_GRADIENT, false);
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
        Set_Alive(CHILD::INTERACT_GRADIENT, true);
        Set_Animation(CHILD::INTERACT_GRADIENT, 0);
        break;
    }
}

void CUI_PrimaryAction::Set_Alive(CHILD child, _bool isAlive)
{
    ForChild(child, [isAlive](CUI_Object* ui) { ui->Set_Alive(isAlive); });
}

void CUI_PrimaryAction::Set_Color(CHILD child, _float4 vColor)
{
    ForChild(child, [vColor](CUI_Object* ui) { ui->Set_Color(vColor); });
}

void CUI_PrimaryAction::Set_Animation(CHILD child, _int iIndex)
{
    ForChild(child, [iIndex](CUI_Object* ui) { ui->Set_Animation(iIndex); });
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