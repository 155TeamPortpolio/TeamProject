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
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_BattleHUDAction::Ready_PartObjects()
{
    auto pContainer = Get_Component<CObjectContainer>();
    const string& strLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

    Attach_Child(strLevelKey, "Proto_GameObject_PrimaryAction", "primary", nullptr, &m_handles[ENUM(Child::PRIMARY)]);
}

void CUI_BattleHUDAction::Attach_Child(const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, UI_DESC* pDesc, UI_HANDLE* pHandleOut)
{
    auto builder = Builder::Create_UIObject({ strLevelKey, strPrototypeTag });

    if (pDesc)
        builder.Add_UIDesc(pDesc);

    CUI_Object* pObj = builder.Build(strInstanceName);
    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);

    if (pHandleOut)
        *pHandleOut = pObj->Get_Handle();
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