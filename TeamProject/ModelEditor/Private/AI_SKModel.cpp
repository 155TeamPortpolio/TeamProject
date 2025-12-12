#include "AI_SKModel.h"

CAI_SKModel::CAI_SKModel()
	: CSkeletalModel{}
{
}

CAI_SKModel::CAI_SKModel(const CAI_SKModel& rhs)
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

const D3D11_INPUT_ELEMENT_DESC* CAI_SKModel::Get_ElementDesc(_uint DrawIndex)
{
	return nullptr;
}

const _uint CAI_SKModel::Get_ElementCount(_uint DrawIndex)
{
	return _uint();
}

const string_view CAI_SKModel::Get_ElementKey(_uint DrawIndex)
{
	return string_view();
}

HRESULT CAI_SKModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
	return S_OK;
}

HRESULT CAI_SKModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
	return S_OK;
}

_uint CAI_SKModel::Get_MeshCount()
{
	return _uint();
}

_uint CAI_SKModel::Get_MaterialIndex(_uint Index)
{
	return _uint();
}

_bool CAI_SKModel::isDrawable(_uint Index)
{
	return _bool();
}

void CAI_SKModel::SetDrawable(_uint Index, _bool isDraw)
{
}

_bool CAI_SKModel::isReadyToDraw()
{
	return _bool();
}

MINMAX_BOX CAI_SKModel::Get_LocalBoundingBox()
{
	return MINMAX_BOX();
}

vector<MINMAX_BOX> CAI_SKModel::Get_MeshBoundingBoxes()
{
	return vector<MINMAX_BOX>();
}

MINMAX_BOX CAI_SKModel::Get_MeshBoundingBox(_uint meshIndex)
{
	return MINMAX_BOX();
}

MINMAX_BOX CAI_SKModel::Get_WorldBoundingBox()
{
	return MINMAX_BOX();
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
	m_pAIScene->

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
