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

HRESULT CAI_STModel::Initialize(string fbxFilePath)
{
	if (FAILED(Load_AIModel(fbxFilePath)))
		return E_FAIL;

	return S_OK;
}

void CAI_STModel::Render_GUI()
{
	__super::Render_GUI();
}

HRESULT CAI_STModel::Load_AIModel(string fbxFilePath)
{
	unsigned int iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast | aiProcess_PreTransformVertices };

	m_pAIScene = m_Importer.ReadFile(fbxFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Ready_AIModelData()))
		return E_FAIL;

	if (FAILED(Ready_AIMaterials()))
		return E_FAIL;

	if (FAILED(Ready_AIAnimations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAI_STModel::Ready_AIModelData()
{
	m_pData = CAIModelData::Create(MESH_TYPE::NONANIM, m_pAIScene);
	if (nullptr == m_pData)
		return E_FAIL;

	return S_OK;
}

HRESULT CAI_STModel::Ready_AIMaterials()
{
	return S_OK;
}

HRESULT CAI_STModel::Ready_AIAnimations()
{
	return S_OK;
}

CAI_STModel* CAI_STModel::Create(string fbxFilePath)
{
	CAI_STModel* instance = new CAI_STModel();

	if (FAILED(instance->Initialize(fbxFilePath))) {
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
