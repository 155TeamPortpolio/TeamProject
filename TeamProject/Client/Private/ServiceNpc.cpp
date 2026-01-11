#include "pch.h"
#include "ServiceNpc.h"

#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "CharacterController.h"

CServiceNpc::CServiceNpc()
	:CNpc()
{
}

CServiceNpc::CServiceNpc(const CServiceNpc& rhs)
	:CNpc(rhs)
{
}

HRESULT CServiceNpc::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	return S_OK;
}

HRESULT CServiceNpc::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CServiceNpc::Awake()
{
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
