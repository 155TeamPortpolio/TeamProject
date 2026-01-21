#include "pch.h"
#include "NpcInteractZone.h"

#include "Child.h"
#include "Npc.h"
#include "IInteract.h"

CNpcInteractZone::CNpcInteractZone()
    :CGameObject()
{
}

CNpcInteractZone::CNpcInteractZone(const CNpcInteractZone& rhs)
    :CGameObject(rhs)
{
}

HRESULT CNpcInteractZone::Initialize_Prototype()
{
    __super::Initialize_Prototype();

	Add_Component<CCollider>();
	
    return S_OK;
}

HRESULT CNpcInteractZone::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CNpcInteractZone::Awake()
{
	Get_Component<CCollider>()->Set_CompActive(true);
}

void CNpcInteractZone::Priority_Update(_float dt)
{
}

void CNpcInteractZone::Update(_float dt)
{
}

void CNpcInteractZone::Late_Update(_float dt)
{
}

void CNpcInteractZone::Interact()
{
	auto pParent = Get_Component<CChild>()->Get_Parent();
	if (pParent == nullptr) return;

	auto pNpc = dynamic_cast<CNpc*>(pParent);
	if (pNpc == nullptr) return;

	pNpc->Execute();
}

void CNpcInteractZone::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;
	
	//UI Interact Call 보내기
}

void CNpcInteractZone::OnTriggerStay(CGameObject* pOther)
{
}

void CNpcInteractZone::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;
	//UI InteractX Call 보내기
}

CNpcInteractZone* CNpcInteractZone::Create()
{
	CNpcInteractZone* instance = new CNpcInteractZone();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CJaeger");
	}

	return instance;
}

CGameObject* CNpcInteractZone::Clone(INIT_DESC* pArg)
{
	CNpcInteractZone* instance = new CNpcInteractZone(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CJaeger");
	}
	return instance;
}

void CNpcInteractZone::Free()
{
	__super::Free();
}
