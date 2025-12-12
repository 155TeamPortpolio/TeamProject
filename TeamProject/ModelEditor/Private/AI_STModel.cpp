#include "AI_STModel.h"

CAI_STModel::CAI_STModel()
	: CSkeletalModel{}
{
}

CAI_STModel::CAI_STModel(const CAI_STModel& rhs)
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

const D3D11_INPUT_ELEMENT_DESC* CAI_STModel::Get_ElementDesc(_uint DrawIndex)
{
	return nullptr;
}

const _uint CAI_STModel::Get_ElementCount(_uint DrawIndex)
{
	return _uint();
}

const string_view CAI_STModel::Get_ElementKey(_uint DrawIndex)
{
	return string_view();
}

HRESULT CAI_STModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
	return S_OK;
}

HRESULT CAI_STModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
	return S_OK;
}

_uint CAI_STModel::Get_MeshCount()
{
	return _uint();
}

_uint CAI_STModel::Get_MaterialIndex(_uint Index)
{
	return _uint();
}

_bool CAI_STModel::isDrawable(_uint Index)
{
	return _bool();
}

void CAI_STModel::SetDrawable(_uint Index, _bool isDraw)
{
}

_bool CAI_STModel::isReadyToDraw()
{
	return _bool();
}

MINMAX_BOX CAI_STModel::Get_LocalBoundingBox()
{
	return MINMAX_BOX();
}

vector<MINMAX_BOX> CAI_STModel::Get_MeshBoundingBoxes()
{
	return vector<MINMAX_BOX>();
}

MINMAX_BOX CAI_STModel::Get_MeshBoundingBox(_uint meshIndex)
{
	return MINMAX_BOX();
}

MINMAX_BOX CAI_STModel::Get_WorldBoundingBox()
{
	return MINMAX_BOX();
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
	  m_pAIScene->mRootNode

	return S_OK;
}

HRESULT CAI_STModel::Ready_Meshes()
{
	return E_NOTIMPL;
}

HRESULT CAI_STModel::Ready_Materials()
{
	return E_NOTIMPL;
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
