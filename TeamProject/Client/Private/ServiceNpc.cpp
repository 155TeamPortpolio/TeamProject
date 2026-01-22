#include "pch.h"
#include "ServiceNpc.h"

#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "CharacterController.h"
#include "EventListener.h"
#include "DataBase.h"

CServiceNpc::CServiceNpc()
	:CNpc()
{
}

CServiceNpc::CServiceNpc(const CServiceNpc& rhs)
	:CNpc(rhs)
{
}

void CServiceNpc::Process_Event(const NPC_INTERACT_DESC& desc)
{
	if (desc.strName != m_strName) return;
	m_iNextSequceID = desc.iSequenceID;

	switch (desc.eResult)
	{
	case DialogueResult::Success:
		Success();
		break;
	case DialogueResult::Running:
		Running();
		break;
	case DialogueResult::Fail:
		Fail();
		break;
	}
}

HRESULT CServiceNpc::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();
	Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CServiceNpc::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Get_Component<CEventListener>()->Add_Listener<NPC_INTERACT_DESC>([&](const NPC_INTERACT_DESC& desc)
		{
			Process_Event(desc);
		});
	return S_OK;
}

void CServiceNpc::Awake()
{
	m_DiagloueData = CDataBase::GetInstance()->GetNpcIDData(m_strName);
	Add_NameIndicator();
}

void CServiceNpc::Priority_Update(_float dt)
{
}

void CServiceNpc::Update(_float dt)
{
	__super::Update(dt);
	Get_Component<CCharacterController>()->Update(dt);
	Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CServiceNpc::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CServiceNpc::Free()
{
	__super::Free();
}
