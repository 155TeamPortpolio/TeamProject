#include "AI_SKModel.h"

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
}



HRESULT CAI_SKModel::Load_AIModel(string fbxFilePath)
{
	unsigned int iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	m_pAIScene = m_Importer.ReadFile(fbxFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	//애들 생성


	return S_OK;
}

HRESULT CAI_SKModel::Ready_Skeleton(const aiNode* pAINode)
{
	return S_OK;
}

HRESULT CAI_SKModel::Ready_Meshes()
{
	return E_NOTIMPL;
}

HRESULT CAI_SKModel::Ready_Materials()
{
	return E_NOTIMPL;
}

HRESULT CAI_SKModel::Ready_Animations()
{
	return E_NOTIMPL;
}

CAI_SKModel* CAI_SKModel::Create(string fbxFilePath)
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

	m_Importer.FreeScene();
}
