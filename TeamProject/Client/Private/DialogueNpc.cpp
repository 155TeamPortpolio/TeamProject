#include "pch.h"
#include "DialogueNpc.h"

#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "CharacterController.h"
#include "EventListener.h"
#include "DataBase.h"

#include "FieldCharacter.h"
#include "FieldSystem.h"

#include "ObjectContainer.h"

CDialogueNpc::CDialogueNpc()
    :CNpc()
{
}

CDialogueNpc::CDialogueNpc(const CDialogueNpc& rhs)
    :CNpc(rhs)
{
}

void CDialogueNpc::Process_Event(const NPC_INTERACT_DESC& desc)
{
    if (desc.strName != m_strName) return;
    m_iCurSequenceID = desc.iCurSequenceID;
    m_iNextSequceID = desc.iNextSequenceID;
}

OBJECT_HANDLE CDialogueNpc::Get_PartnerHandle()
{
    auto pChild = Get_Component<CObjectContainer>()->Find_ObjectByName("Partner");
    if (pChild == nullptr) return OBJECT_HANDLE{};

    return pChild->Get_Handle();
}

HRESULT CDialogueNpc::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CAnimator3D>();
    Add_Component<CSkeletalModel>();
    Add_Component<CMaterial>();
    Add_Component<CCharacterController>();
    Add_Component<CEventListener>();
    return S_OK;
}

HRESULT CDialogueNpc::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CEventListener>()->Add_Listener<NPC_INTERACT_DESC>([&](const NPC_INTERACT_DESC& desc)
        {
            Process_Event(desc);
        });
    return S_OK;
}

void CDialogueNpc::Awake()
{
    __super::Awake();

    m_DiagloueData = CDataBase::GetInstance()->GetNpcIDData(m_strName);
    Add_NameIndicator();
}

void CDialogueNpc::Priority_Update(_float dt)
{
}

void CDialogueNpc::Update(_float dt)
{
    __super::Update(dt);
    Get_Component<CCharacterController>()->Update(dt);
    Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CDialogueNpc::Late_Update(_float dt)
{
    Get_Component<CCharacterController>()->Late_Update(dt);
}

void CDialogueNpc::Free()
{
    __super::Free();
}
