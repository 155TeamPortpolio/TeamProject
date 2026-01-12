#include "pch.h"
#include "UI_BattleHUDAction.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_BattleHUDAction::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_BattleHUDAction::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Set_Size(_float2(340.f, 224.f));

    Ready_PartObjects();

    return S_OK;
}

void CUI_BattleHUDAction::Update(_float dt)
{
    __super::Update(dt);

    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('P'))
    //{
    //    for (_int i = 0; i < ENUM(Child::END); ++i)
    //    {
    //        Set_Active(static_cast<Child>(i), true);
    //    }
    //}
    //
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('O'))
    //{
    //    for (_int i = 0; i < ENUM(Child::END); ++i)
    //    {
    //        Set_Active(static_cast<Child>(i), false);
    //    }
    //}

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_BattleHUDAction::Ready_PartObjects()
{
    auto pContainer = Get_Component<CObjectContainer>();
    const string& strLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

    Attach_Child(strLevelKey, "Proto_GameObject_PrimaryAction", "primary", &m_handles[ENUM(Child::PRIMARY)], _float2(0.f, m_vOffset.y));
    Attach_Child(strLevelKey, "Proto_GameObject_EvadeAction", "evade", &m_handles[ENUM(Child::EVADE)], m_vOffset);
    Attach_Child(strLevelKey, "Proto_GameObject_SpecialAction", "special", &m_handles[ENUM(Child::SPECIAL)], _float2(m_vOffset.x * 2.f, m_vOffset.y));
}

void CUI_BattleHUDAction::Attach_Child(const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, UI_HANDLE* pHandleOut, _float2 vOffset)
{
    CUI_Object* pObj = Builder::Create_UIObject({ strLevelKey, strPrototypeTag })
            .Build(strInstanceName);

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);

    pObj->Set_AnchorOffset(vOffset);

    if (pHandleOut)
        *pHandleOut = pObj->Get_Handle();
}

void CUI_BattleHUDAction::Set_Active(Child child, _bool isActive)
{
    if(isActive)    
        ForChild(child, [](CUI_Object* ui) { ui->UI_Active(); });
    else
        ForChild(child, [](CUI_Object* ui) { ui->UI_DeActive(); });
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