#include "SkeletonFollower.h"
#include "SkeletalModel.h"
#include "ModelData.h"
#include "Animator3D.h"
#include "GameInstance.h"
#include "IResourceService.h"

CSkeletonFollower::CSkeletonFollower()
{
}

CSkeletonFollower::CSkeletonFollower(const CSkeletonFollower& rhs)
{
}

CSkeletonFollower::~CSkeletonFollower()
{
}

HRESULT CSkeletonFollower::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkeletonFollower::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

HRESULT CSkeletonFollower::Link_MyModel(const string& levelKey, const string& modelDataKey)
{
	Safe_Release(m_pMyData);
	m_pMyData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(levelKey, modelDataKey);
	Safe_AddRef(m_pMyData);
	/*일단 나의 정보를 받아옴*/
	m_ReflectedIndices.resize(m_pMyData->Get_BoneCount());
	_float4x4 baseMatrix = {};
	XMStoreFloat4x4(&baseMatrix, XMMatrixIdentity());
	m_ReflectedMatices.resize(m_pMyData->Get_BoneCount(), baseMatrix);
	return S_OK;
}

HRESULT CSkeletonFollower::Link_MasterModel(const string& levelKey, const string& modelDataKey)
{
	Safe_Release(m_pMasterData);
	m_pMasterData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(levelKey, modelDataKey);
	Safe_AddRef(m_pMasterData);

	m_ReflectedIndices= m_pMyData->GenerateFollowingIndices(m_pMasterData);

	return S_OK;
}

HRESULT CSkeletonFollower::Link_MyModel(CSkeletalModel* pModel)
{
	Safe_Release(m_pMyData);
	m_pMyData = pModel->Get_ModelData();
	Safe_AddRef(m_pMyData);
	m_ReflectedIndices.resize(m_pMyData->Get_BoneCount(),0);
	_float4x4 baseMatrix = {};
	XMStoreFloat4x4(&baseMatrix, XMMatrixIdentity());
	m_ReflectedMatices.resize(m_pMyData->Get_BoneCount(), baseMatrix);
	return S_OK;
}

HRESULT CSkeletonFollower::Link_MasterModel(CSkeletalModel* pMasterModel)
{
	Safe_Release(m_pMasterData);
	m_pMasterData = pMasterModel->Get_ModelData();
	Safe_AddRef(m_pMasterData);
	m_ReflectedIndices = m_pMyData->GenerateFollowingIndices(m_pMasterData);

	return S_OK;
}


void CSkeletonFollower::Set_MasterAnimator(CAnimator3D* pAnimator)
{
		Safe_Release(m_pMasterAnimator);
		m_pMasterAnimator = pAnimator;
		Safe_AddRef(m_pMasterAnimator);
}

HRESULT CSkeletonFollower::ReLink_WithMaster()
{
	if (m_pMasterData) {
		m_ReflectedIndices = m_pMyData->GenerateFollowingIndices(m_pMasterData);
	}
	return S_OK;
}

void CSkeletonFollower::Sync_Bones(_float dt)
{
	auto& masterFinalVec = m_pMasterAnimator->Get_BoneMatrices();        // SkinMatrix (World*Offset)
	auto& masterCombinedVec = m_pMasterAnimator->Get_CombinedBoneMatrices(); // Combined (World)

	for (size_t i = 0; i < m_ReflectedIndices.size(); i++)
	{
		int reflected = m_ReflectedIndices[i];

		if (reflected == -1)
		{
			// 내 로컬, 오프셋, 부모 월드
			_float4x4 myLocal = m_pMyData->Get_TransformMatrix(i);
			_matrix matLocal = XMLoadFloat4x4(&myLocal);
			_matrix matOffset = m_pMyData->Get_OffsetMatrix(i);

			/*내 행렬 상의 부모*/
			_int parentIdx = m_pMyData->Get_BoneParentIndex(i);
			_int masterParentIdx = -1;

			if (parentIdx != -1){
			masterParentIdx = m_ReflectedIndices[parentIdx];//내 행렬의 부모와 매칭되는 마스터의 행렬 인덱스
			if(masterParentIdx != -1)string name = m_pMasterData->Find_BoneNameByIndex(masterParentIdx);
			}
			_matrix matParent = (masterParentIdx != -1)
				? XMLoadFloat4x4(&masterCombinedVec[masterParentIdx])
				: XMMatrixIdentity();


			// 마지막에 Offset 곱
			_matrix matFinal = matOffset * (matLocal * matParent);

			XMStoreFloat4x4(&m_ReflectedMatices[i], matFinal);
		}
		else
		{
			// 매칭된 본은 마스터의 최종 스킨 행렬 사용
			m_ReflectedMatices[i] = masterFinalVec[reflected];
		}
	}
}

CSkeletonFollower* CSkeletonFollower::Create()
{
	CSkeletonFollower* instance = new CSkeletonFollower();
	if (FAILED(instance->Initialize_Prototype())) {
		Safe_Release(instance);
	}
	return instance;
}

CComponent* CSkeletonFollower::Clone()
{
	return new CSkeletonFollower(*this);
}

void CSkeletonFollower::Free()
{
	Safe_Release(m_pMyData);
	Safe_Release(m_pMasterData);
	Safe_Release(m_pMasterAnimator);
}
