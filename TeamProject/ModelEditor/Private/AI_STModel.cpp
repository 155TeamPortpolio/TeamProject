#include "AI_STModel.h"

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
	return S_OK;
}

HRESULT CAI_STModel::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

void CAI_STModel::Render_GUI()
{
}

HRESULT CAI_STModel::Load_AIModel(string fbxFilePath)
{
	unsigned int iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast | aiProcess_PreTransformVertices };

	m_pAIScene = m_Importer.ReadFile(fbxFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	return S_OK;
}

HRESULT CAI_STModel::Ready_Skeleton(const aiNode* pAINode)
{ 
	return S_OK;
}

HRESULT CAI_STModel::Ready_Meshes()
{
	return S_OK;
}

HRESULT CAI_STModel::Ready_Materials()
{
	return S_OK;
}

CAI_STModel* CAI_STModel::Create(string fbxFilePath)
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

	m_Importer.FreeScene();
}
