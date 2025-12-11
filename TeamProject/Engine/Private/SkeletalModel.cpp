#include "Engine_Defines.h"
#include "SkeletalModel.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "ModelData.h"
#include"GameObject.h"
#include "Transform.h"

CSkeletalModel::CSkeletalModel()
{
}

CSkeletalModel::CSkeletalModel(const CSkeletalModel& rhs)
	:CModel(rhs), m_pData(rhs.m_pData),
	m_DrawableMeshes(rhs.m_DrawableMeshes),
	m_TransfromationMatrices(rhs.m_TransfromationMatrices),
	m_CombinedMatrices(rhs.m_CombinedMatrices),
	m_FinalMatices(rhs.m_FinalMatices)
{
	Safe_AddRef(m_pData);
}

HRESULT CSkeletalModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkeletalModel::Initialize(COMPONENT_DESC* pArg)
{

	return S_OK;
}

HRESULT CSkeletalModel::Link_Model(const string& levelKey, const string& modelDataKey)
{
	Safe_Release(m_pData);
	m_pData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(levelKey, modelDataKey);
	Safe_AddRef(m_pData);
	m_DrawableMeshes.resize(m_pData->Get_MeshCount(), true);

	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());
	m_TransfromationMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_CombinedMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_ManipulateMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_FinalMatices.resize(m_pData->Get_BoneCount(), IdentityMatrix);

	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		m_TransfromationMatrices[i] = m_pData->Get_TransformMatrix(i);
	}
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

HRESULT CSkeletalModel::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
	/*모델은 렌더의 역할만 수행할 것임*/
	return m_pData->Render_Mesh(pContext, Index);
}

const D3D11_INPUT_ELEMENT_DESC* CSkeletalModel::Get_ElementDesc(_uint DrawIndex)
{
	return m_pData->Get_ElementDesc(DrawIndex);
}

const _uint CSkeletalModel::Get_ElementCount(_uint DrawIndex)
{
	return m_pData->Get_ElementCount(DrawIndex);
}

const string_view CSkeletalModel::Get_ElementKey(_uint DrawIndex)
{
	return m_pData->Get_ElementKey(DrawIndex);
}

_uint CSkeletalModel::Get_MeshCount()
{
	return m_pData->Get_MeshCount();
}

_uint CSkeletalModel::Get_MaterialIndex(_uint Index)
{
	return m_pData->Get_MaterialIndex(Index);
}

_bool CSkeletalModel::isDrawable(_uint Index)
{
	return m_DrawableMeshes[Index];
}

void CSkeletalModel::SetDrawable(_uint Index, _bool isDraw)
{
	if (Index >= m_DrawableMeshes.size())
		return;

	m_DrawableMeshes[Index] = isDraw;
}

MINMAX_BOX CSkeletalModel::Get_LocalBoundingBox()
{
	return m_pData->Get_LocalBoundingBox();
}

 const vector<_float4x4>& CSkeletalModel::Get_BoneMatrices()
{
	 if (!m_bDirty) {
		 return m_FinalMatices;
	 }
	 else{
		 for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
		 {
			 int parent = m_pData->Get_BoneParentIndex(i);

			 if (parent == -1) {
				 _matrix MyTransformation =
					 XMLoadFloat4x4(&m_ManipulateMatrices[i]) *
					 XMLoadFloat4x4(&m_TransfromationMatrices[i]);

				 XMStoreFloat4x4(&m_CombinedMatrices[i], MyTransformation);
			 }
			 else {
				 _matrix ParentCombine = XMLoadFloat4x4(&m_CombinedMatrices[parent]);
				 _matrix MyTransformation =
					 XMLoadFloat4x4(&m_ManipulateMatrices[i])
					 * XMLoadFloat4x4(&m_TransfromationMatrices[i]);

				 XMStoreFloat4x4(&m_CombinedMatrices[i], MyTransformation * ParentCombine);
			 }
		 }

		 for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
		 {
			 XMStoreFloat4x4(&m_FinalMatices[i], m_pData->Get_OffsetMatrix(i) * XMLoadFloat4x4(&m_CombinedMatrices[i]));
		 }

		 m_bDirty = false;
		 return m_FinalMatices;
	 }
}

 _float4x4* CSkeletalModel::Get_BoneMatrixPtr(const string& boneName)
 {
	 _int Index = m_pData->Find_BoneIndexByName(boneName);
	 if (Index == -1)  return nullptr;
	 else {
		 return &m_FinalMatices[Index];
	 }
 }

MINMAX_BOX CSkeletalModel::Get_WorldBoundingBox()
{
	MINMAX_BOX wordlBox = m_pData->Get_LocalBoundingBox();
	_float4x4* pWorldMat = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix_Ptr();
	XMStoreFloat3(&wordlBox.vMin, XMVector3TransformCoord(XMLoadFloat3(&wordlBox.vMin), XMLoadFloat4x4(pWorldMat)));
	XMStoreFloat3(&wordlBox.vMax, XMVector3TransformCoord(XMLoadFloat3(&wordlBox.vMax), XMLoadFloat4x4(pWorldMat)));
	return wordlBox;
}

vector<MINMAX_BOX> CSkeletalModel::Get_MeshBoundingBox()
{
	vector<MINMAX_BOX> boxes;

	for (size_t i = 0; i < m_pData->Get_MeshCount(); i++)
	{
		boxes.push_back(m_pData->Get_MeshBoundingBox(i));
	}
	return boxes;
}

MINMAX_BOX CSkeletalModel::Get_MeshBoundingBox(_uint index)
{
	return m_pData->Get_MeshBoundingBox(index);
}

_bool CSkeletalModel::isReadyToDraw()
{
	return m_pData != nullptr; 
}

void CSkeletalModel::Control_Bone(const string& boneName, _fmatrix BoneMatrix)
{
	_int Index = m_pData->Find_BoneIndexByName(boneName);
	if (Index == -1) return;

	else {
		XMStoreFloat4x4(&m_ManipulateMatrices[Index], BoneMatrix);
	}
	m_bDirty = true;
}

void CSkeletalModel::Control_BoneByIndex(_uint Index, _fmatrix BoneMatrix)
 {

	if (Index >= m_ManipulateMatrices.size()) return;
	else {
		XMStoreFloat4x4(&m_ManipulateMatrices[Index], BoneMatrix);
	}
	m_bDirty = true;
}

void CSkeletalModel::Hide_MehsByName(const string& name)
{
	_int index = m_pData->Find_MeshIndex(name);

	if (index == -1)
		return;

	else
		m_DrawableMeshes[index] = false;
}

void CSkeletalModel::Render_GUI()
{
	ImGui::SeparatorText("Animate Model");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * (m_pData->Get_MeshCount() + 4)) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##Animate ModelChild", ImVec2{ 0, childHeight }, true);
		m_pData->Render_GUI();
		string ID = "HideMesh : ";
		for (size_t i = 0; i < m_pData->Get_MeshCount(); i++)
		{

			if (ImGui::Button((ID + to_string(i)).c_str()))
			{
				if (m_DrawableMeshes.size() >= 1) {
					m_DrawableMeshes[i] = !m_DrawableMeshes[i];
				}
			}
		}
	ImGui::EndChild();
}


CSkeletalModel* CSkeletalModel::Create()
{
	CSkeletalModel* instance = new CSkeletalModel();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CStaticModel Create Failed : CStaticModel");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CSkeletalModel::Clone()
{
	CSkeletalModel* instance = new CSkeletalModel(*this);
	return instance;
}

void CSkeletalModel::Free()
{
	__super::Free();
	if(m_pData)
	Safe_Release(m_pData);
}
