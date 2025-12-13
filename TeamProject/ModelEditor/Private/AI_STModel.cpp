#include "AI_STModel.h"
#include "AIModelData.h"

CAI_STModel::CAI_STModel()
	: CStaticModel{}
{
}

CAI_STModel::CAI_STModel(const CAI_STModel& rhs)	
	: CStaticModel(rhs)
{
}

HRESULT CAI_STModel::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CAI_STModel::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

void CAI_STModel::Render_GUI()
{
	__super::Render_GUI();
}

HRESULT CAI_STModel::Load_AIModel(const aiScene* pAIScene, string fileName)
{
	if (nullptr == pAIScene)
		return E_FAIL;
	Release_Mesh();
	_uint meshNum = pAIScene->mNumMeshes;
	m_DrawableMeshes.resize(meshNum, true);
	m_fileName = fileName;

	if (FAILED(Ready_AIModelData(pAIScene)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAI_STModel::Ready_AIModelData(const aiScene* pAIScene)
{
	m_pData = CAIModelData::Create(MESH_TYPE::NONANIM, pAIScene);
	if (nullptr == m_pData)
		return E_FAIL;

	return S_OK;
}


HRESULT CAI_STModel::Release_Mesh()
{
	Safe_Release(m_pData);
	vector<bool> v;
	m_DrawableMeshes.swap(v);

	return S_OK;
}
CAI_STModel* CAI_STModel::Create()
{
	CAI_STModel* instance = new CAI_STModel();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CAI_STModel Create Failed : CAI_STModel");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CAI_STModel::Clone()
{
	CAI_STModel* instance = new CAI_STModel(*this);
	return instance;
}

void CAI_STModel::Free()
{
	__super::Free();
}
