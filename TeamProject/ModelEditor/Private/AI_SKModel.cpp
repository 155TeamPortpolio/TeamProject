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

HRESULT CAI_SKModel::Initialize(string fbxFilePath)
{
	if (FAILED(Load_AIModel(fbxFilePath)))
		return E_FAIL;

	return S_OK;
}

void CAI_SKModel::Render_GUI()
{
	__super::Render_GUI();
}

HRESULT CAI_SKModel::Load_AIModel(string fbxFilePath)
{
	unsigned int iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	m_pAIScene = m_Importer.ReadFile(fbxFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	//애들 생성
	if (FAILED(Ready_AIModelData()))
		return E_FAIL;

	if (FAILED(Ready_AIMaterials()))
		return E_FAIL;

	if (FAILED(Ready_AIAnimations()))
		return E_FAIL;

	return S_OK;
}


HRESULT CAI_SKModel::Ready_AIModelData()
{
	m_pData = CAIModelData::Create(MESH_TYPE::ANIM, m_pAIScene);

	if (nullptr == m_pData)
		return E_FAIL;

	return S_OK;
}

HRESULT CAI_SKModel::Ready_AIMaterials()
{
	return S_OK;
}

HRESULT CAI_SKModel::Ready_AIAnimations()
{
	return S_OK;
}

CAI_SKModel* CAI_SKModel::Create(string fbxFilePath)
{
	CAI_SKModel* instance = new CAI_SKModel();

	if (FAILED(instance->Initialize(fbxFilePath))) {
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

	m_Importer.FreeScene();
}
