#include "pch.h"
#include "UI_Switch.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "TextSlot.h"

#include "UI_SwitchGauge.h"
#include "UI_SwitchRole.h"

#include "UIDirector.h"

HRESULT CUI_Switch::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_SwitchGauge", CUI_SwitchGauge::Create());
    PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_SwitchRole", CUI_SwitchRole::Create());

    return S_OK;
}

HRESULT CUI_Switch::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("switch.json")));
    Cache();

    Create_Gauge();
    Create_Roles();

    Set_Alive(false);

    return S_OK;
}

void CUI_Switch::Awake()
{
}

void CUI_Switch::Update(_float dt)
{
    switch (m_eState)
    {
    case STATE::ACTIVE:
        Update_Timer(dt);
        {
            _float fRatio = m_fTimer / m_fDuration;

            Update_TimerText();
            if (m_pGauge)
                m_pGauge->Set_Gauge(fRatio);

            if (m_fTimer <= 0.f)
                UI_DeActive();
        } 
        break;
    case STATE::DEACTIVATING:
        if (Is_AnimFinished())
            Set_Alive(false);
        break;
    }     

    __super::Update(dt);
    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_Switch::UI_Active(void* pArg)
{
    if (!pArg)
        return;

    SWITCH_DESC* pDesc = static_cast<SWITCH_DESC*>(pArg);
    if (!pDesc)
        return;

    m_characters[ENUM(ROLE::LEFT)] = pDesc->left;
    m_characters[ENUM(ROLE::RIGHT)] = pDesc->right;

    Change_State(STATE::ACTIVE);
}

void CUI_Switch::UI_DeActive(void* pArg)
{
    Change_State(STATE::DEACTIVATING);
}

void CUI_Switch::Cache()
{
    auto pTimer = Get_Component<CObjectContainer>()->Find_Descendant("timer");
    if (pTimer)
        m_pTimerText = pTimer->Get_Component<CTextSlot>();
}

void CUI_Switch::Create_Gauge()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_SwitchGauge" })
        .Build("gauge");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pGauge = dynamic_cast<CUI_SwitchGauge*>(pObj);
}

void CUI_Switch::Create_Roles()
{
    for (_int i = 0; i < ENUM(ROLE::END); ++i)
    {
        auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_SwitchRole" })
            .Build("role" + std::to_string(i));

        if (!pObj)
            continue;

        auto pRole = dynamic_cast<CUI_SwitchRole*>(pObj);
        if (!pRole)
            continue;

        m_pRoles[i] = pRole;
        Get_Component<CObjectContainer>()->Add_Child(pObj);

        if (i == ENUM(ROLE::LEFT))
        {
            pRole->Set_Anchor(ANCHOR::Left);
            pRole->Set_AnchorOffset({ 50.f, 0.f });
            pRole->Set_Side(CUI_SwitchRole::SIDE::LEFT);
        }
        else
        {
            pRole->Set_Anchor(ANCHOR::Right);
            pRole->Set_AnchorOffset({ -50.f, 0.f });
            pRole->Set_Side(CUI_SwitchRole::SIDE::RIGHT);
        }
    } 
}

void CUI_Switch::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;
    switch (m_eState)
    {
    case STATE::ACTIVE:
        Set_Alive(true);
        Set_Animation(0);
        Change_RoleIcon(ROLE::LEFT, m_characters[ENUM(ROLE::LEFT)]);
        Change_RoleIcon(ROLE::RIGHT, m_characters[ENUM(ROLE::RIGHT)]);
        if (m_pGauge) m_pGauge->UI_Active();
        m_fTimer = m_fDuration;
        break;
    case STATE::DEACTIVATING:
        Set_Animation(1);
        m_fTimer = 0.f;
        break;
    }
}

void CUI_Switch::Update_Timer(_float dt)
{
    m_fTimer -= dt;
    m_fTimer = max(0.f, m_fTimer);
}

void CUI_Switch::Update_TimerText()
{
    if (!m_pTimerText)
        return;

    _int iMilliSeconds = static_cast<_int>(m_fTimer * 1000.f);

    _int iSeconds = iMilliSeconds / 1000;
    _int iCentiSeconds = (iMilliSeconds % 1000) / 10;

    WCHAR bufSeconds[32];
    Helper::Format_FixedZeroPad(bufSeconds, _countof(bufSeconds), iSeconds, 2);

    WCHAR buffCentiSeconds[32];
    Helper::Format_FixedZeroPad(buffCentiSeconds, _countof(buffCentiSeconds), iCentiSeconds, 2);

    WCHAR bufFinal[64];

    swprintf_s(bufFinal, L"00:%s:%s", bufSeconds, buffCentiSeconds);

    m_pTimerText->Set_Text(bufFinal);
}

void CUI_Switch::Change_RoleIcon(ROLE eRole, CHARACTER eCharacter)
{
    auto pRole = m_pRoles[ENUM(eRole)];
    if (!pRole)
        return;

    pRole->Change_RoleIcon(eCharacter);
}

CGameObject* CUI_Switch::Create()
{
    CUI_Switch* pInstance = new CUI_Switch();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Switch");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Switch::Clone(INIT_DESC* pArg)
{
    CUI_Switch* pInstance = new CUI_Switch(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Switch");
        Safe_Release(pInstance);
    }
    return pInstance;
}