#include "AI_SKModel.h"
#include "AIModelData.h"

CAI_SKModel::CAI_SKModel()
	: CSkeletalModel{}
{
}

CAI_SKModel::CAI_SKModel(const CAI_SKModel& rhs)
	: CSkeletalModel(rhs)
{
}

HRESULT CAI_SKModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_SKModel::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

void CAI_SKModel::Render_GUI()
{
	__super::Render_GUI();
}

HRESULT CAI_SKModel::Load_AIModel(const aiScene* pAIScene, string fileName)
{
	if (nullptr == pAIScene)
		return E_FAIL;
	Release_Mesh();

	_uint meshNum = pAIScene->mNumMeshes;
	m_DrawableMeshes.resize(meshNum, true);

	if (FAILED(Ready_AIModelData(pAIScene)))
		return E_FAIL;

	m_fileName = fileName;
	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	m_TransfromationMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_CombinedMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_FinalMatices.resize(m_pData->Get_BoneCount(), IdentityMatrix);

	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		int parent = m_pData->Get_BoneParentIndex(i);

		if (parent == -1) {
			m_CombinedMatrices[i] = m_TransfromationMatrices[i];
		}
		else {
			_matrix ParentCombine = XMLoadFloat4x4(&m_CombinedMatrices[parent]);
			_matrix MyTransformation = XMLoadFloat4x4(&m_TransfromationMatrices[i]);
			XMStoreFloat4x4(&m_CombinedMatrices[i], MyTransformation * ParentCombine);
		}
	}

	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		XMStoreFloat4x4(&m_FinalMatices[i], m_pData->Get_OffsetMatrix(i) * XMLoadFloat4x4(&m_CombinedMatrices[i]));
	}
	return S_OK;
}


HRESULT CAI_SKModel::Ready_AIModelData(const aiScene* pAIScene)
{
	m_pData = CAIModelData::Create(MESH_TYPE::ANIM, pAIScene);

	if (nullptr == m_pData)
		return E_FAIL;

	return S_OK;
}

HRESULT CAI_SKModel::Release_Mesh()
{
	Safe_Release(m_pData);
	vector<bool> v;
	m_DrawableMeshes.swap(v);

	return S_OK;
}

CAI_SKModel* CAI_SKModel::Create()
{
	CAI_SKModel* instance = new CAI_SKModel();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CAI_SKModel Create Failed : CAI_SKModel");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CAI_SKModel::Clone()
{
	CAI_SKModel* instance = new CAI_SKModel(*this);
	return instance;
}

void CAI_SKModel::Free()
{
	__super::Free();
}
