#include "pch.h"
#include "UI_PrimaryAction.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "ObjectContainer.h"
#include "EventListener.h"

const string CUI_PrimaryAction::INSTANCENAMES[ENUM(Child::END)] = { "attack", "attackBg", "attackIcon", "attackMouse", "interact", "interactGradient" };

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

    for (_int i = 0; i < ENUM(Child::END); ++i)
        m_handles[i] = Get_DescendantHandle(INSTANCENAMES[i]);

	return S_OK;
}

void CUI_PrimaryAction::Update(_float dt)
{
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('M'))
    //    Set_ActionMode(MODE::ATTACK);
    //
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('N'))
    //    Set_ActionMode(MODE::INTERACT);
    //
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('B'))
    //    Set_AttackActive(true);
    //
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('V'))
    //    Set_AttackActive(false);
    //
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('C'))
    //    Set_InteractActive(true);
    //
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('X'))
    //    Set_InteractActive(false);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_PrimaryAction::UI_Active(void* pArg)
{
    Set_AttackActive(true);
}

void CUI_PrimaryAction::UI_DeActive(void* pArg)
{
    Set_AttackActive(false);
}

void CUI_PrimaryAction::Set_ActionMode(MODE eMode)
{
    if (m_eMode == eMode)
        return;

    m_eMode = eMode;
    _bool isAttackMode = (m_eMode == MODE::ATTACK) ? true : false;

    Set_Alive(Child::ATTACK, isAttackMode);
    Set_Alive(Child::INTERACT, !isAttackMode);

    if (!isAttackMode)
        Set_Animation(Child::INTERACT, 0);
}

void CUI_PrimaryAction::Set_AttackActive(_bool isActive)
{
    if (m_eMode != MODE::ATTACK)
        return;

    if (isActive)
    {
        Set_Color(Child::ATTACK_BG, UI_GRAY_DARKEST);
        Set_Color(Child::ATTACK_ICON, UI_GRAY_LIGHTEST);
        Set_Color(Child::ATTACK_MOUSE, UI_WHITE);
    }
    else
    {
        Set_Color(Child::ATTACK_BG, UI_GRAY_MEDIUM);
        Set_Color(Child::ATTACK_ICON, UI_GRAY_DARK);
        Set_Color(Child::ATTACK_MOUSE, UI_TRANSPARENT);
    }
}

void CUI_PrimaryAction::Set_InteractActive(_bool isActive)
{
    if (m_eMode != MODE::INTERACT)
        return;

    if (isActive)
    {
        Set_Alive(Child::INTERACT_GRADIENT, true);
        Set_Animation(Child::INTERACT_GRADIENT, 0);
    } 
    else
    {
        Set_Alive(Child::INTERACT_GRADIENT, false);
    } 
}

void CUI_PrimaryAction::Set_Alive(Child child, _bool isAlive)
{
    ForChild(child, [isAlive](CUI_Object* ui) { ui->Set_Alive(isAlive); });
}

void CUI_PrimaryAction::Set_Color(Child child, _float4 vColor)
{
    ForChild(child, [vColor](CUI_Object* ui) { ui->Set_Color(vColor); });
}

void CUI_PrimaryAction::Set_Animation(Child child, _int iIndex)
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