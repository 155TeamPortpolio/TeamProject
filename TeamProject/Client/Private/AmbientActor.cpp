#include "pch.h"
#include "AmbientActor.h"

#include "Animator3D.h"
#include "SkeletalModel.h"
#include "ModelData.h"
#include "Material.h"

CAmbientActor::CAmbientActor()
	:CGameObject()
{
}

CAmbientActor::CAmbientActor(const CAmbientActor& rhs)
	:CGameObject(rhs)
{
}

HRESULT CAmbientActor::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CAmbientActor::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void CAmbientActor::Free()
{
	__super::Free();
}
