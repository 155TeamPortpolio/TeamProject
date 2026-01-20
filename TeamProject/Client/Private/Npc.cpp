#include "pch.h"
#include "Npc.h"

#include "UI_NameIndicator.h"
#include "FieldSystem.h"
#include "GameInstance.h"
#include "CharacterController.h"

CNpc::CNpc()
{
}

CNpc::CNpc(const CNpc& rhs)
    :CGameObject(rhs)
{
}

HRESULT CNpc::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CNpc::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CNpc::Update(_float dt)
{
}

HRESULT CNpc::Add_NameIndicator()
{
    CUI_NameIndicator::INDICATOR_DESC* pDesc = new CUI_NameIndicator::INDICATOR_DESC;
    pDesc->strName = m_strName;
    pDesc->pCCT = Get_Component<CCharacterController>();
    auto pUI = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_NameIndicator" })
        .Add_UIDesc(pDesc)
        .Build("nameIndicator");
    UIManager()->Add_UIObject(pUI, LevelManager()->Get_NowLevelKey());

    return S_OK;
}

void CNpc::Free()
{
    __super::Free();
}
