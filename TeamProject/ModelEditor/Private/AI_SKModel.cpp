#include "AI_SKModel.h"
#include "AIModelData.h"
#include "Helper_Func.h"

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
	m_ManipulateMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);

	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
		m_TransfromationMatrices[i] = m_pData->Get_TransformMatrix(i);

	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		int parent = m_pData->Get_BoneParentIndex(i);

		if (parent == -1) 
			m_CombinedMatrices[i] = m_TransfromationMatrices[i];
		else 
		{
			_matrix ParentCombine = XMLoadFloat4x4(&m_CombinedMatrices[parent]);
			_matrix MyTransformation = XMLoadFloat4x4(&m_TransfromationMatrices[i]);
			XMStoreFloat4x4(&m_CombinedMatrices[i], MyTransformation * ParentCombine);
		}
	}

	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		XMStoreFloat4x4(&m_FinalMatices[i], m_pData->Get_OffsetMatrix(i) * XMLoadFloat4x4(&m_CombinedMatrices[i]));
	}

	// -----------------------------
	auto data = dynamic_cast<CAIModelData*>(m_pData);

	auto ApplyDefaultClears = [&](const vector<_uint>& indices)
		{
			for (_uint index : indices)
				m_DrawableMeshes[index] = false;
		};

	if (data)
	{
		ApplyDefaultClears(data->Get_ProxyIndex());
		ApplyDefaultClears(data->Get_Eff_Index());
	}
	return S_OK;
}

CAIModelData* CAI_SKModel::Get_AIModelData()
{
	return static_cast<CAIModelData*>(m_pData);
}

HRESULT CAI_SKModel::Save_Model(const string& SavePath, _fmatrix WorldMatrix)
{
	string path = SavePath + m_fileName + ".model";
	ofstream ofs(path.c_str(), ios::binary);
	if (!ofs.is_open())
		return E_FAIL;

	MODEL_FILE_HEADER fileHeader = {};
	fileHeader.isAnimate = true;
	fileHeader.MeshCount = m_pData->Get_MeshCount();
	strcpy_s(fileHeader.ModelKey, sizeof(fileHeader.ModelKey), m_fileName.data());
	ofs.write(reinterpret_cast<char*>(&fileHeader), sizeof(MODEL_FILE_HEADER));

	static_cast<CAIModelData*>(m_pData)->Save_File(ofs, WorldMatrix);
	ofs.close();
	return S_OK;
}

HRESULT CAI_SKModel::Ready_AIModelData(const aiScene* pAIScene)
{
	_uint meshNum = pAIScene->mNumMeshes;
	m_pData = CAIModelData::Create(MESH_TYPE::ANIM, pAIScene);
	m_DrawableMeshes.resize(m_pData->Get_MeshCount(), true);

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

void CAI_SKModel::Clear_Proxy()
{
	auto proxy = dynamic_cast<CAIModelData*>(m_pData)->Get_ProxyIndex();

	for (_uint Index : proxy) 
		m_DrawableMeshes[Index] = !m_DrawableMeshes[Index];
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