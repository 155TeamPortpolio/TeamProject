#include "pch.h"
#include "AmbientActor.h"

#include "Animator3D.h"
#include "SkeletalModel.h"
#include "ModelData.h"
#include "Material.h"
#include "AudioSource.h"

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
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAmbientActor::Initialize(INIT_DESC* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CAmbientActor::Free()
{
	__super::Free();
}
