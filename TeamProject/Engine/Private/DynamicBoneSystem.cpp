#include "Engine_Defines.h"
#include "DynamicBone.h"
#include "DynamicBoneSystem.h"
#include "ModelData.h"

CDynamicBoneSystem::CDynamicBoneSystem()
{
}

HRESULT CDynamicBoneSystem::Initialize(CAnimator3D* pAnimator)
{
	if (nullptr == pAnimator)
		return E_FAIL;

	m_pAnimator = pAnimator;
	Safe_AddRef(m_pAnimator);

	return S_OK;
}

HRESULT CDynamicBoneSystem::Create_Chain(AnimArg BoneArg)
{
	_int iRootIndex = m_pAnimator->Resolve_BoneIndex(BoneArg);
	if(-1 == iRootIndex) return E_FAIL;

	DynamicChain Chain{};
	Chain.RootBone = m_pAnimator->Get_ModelData()->Get_BoneNames()[iRootIndex];
	Chain.RootBoneIndex = iRootIndex;


    DYNAMIC_NODE node;
    
	return S_OK;
}

CDynamicBoneSystem* CDynamicBoneSystem::Create(CAnimator3D* pAnimator)
{
	CDynamicBoneSystem* instance = new CDynamicBoneSystem();

	if (FAILED(instance->Initialize(pAnimator))) {
		Safe_Release(instance);
	}

	return instance;
}

void CDynamicBoneSystem::Free()
{
	__super::Free();
	Safe_Release(m_pAnimator);
}