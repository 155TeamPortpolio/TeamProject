#include "Engine_Defines.h"
#include "DynamicBone.h"
#include "Animator3D.h"
#include "ModelData.h"

CDynamicBone::CDynamicBone()
{
}

HRESULT CDynamicBone::Initialize(CAnimator3D* pAnimator)
{
	if (nullptr == pAnimator)
		return E_FAIL;

	m_pAnimator = pAnimator;
	Safe_AddRef(m_pAnimator);

	return S_OK;
}

HRESULT CDynamicBone::Link_ChainData(const vector<DYNAMIC_CHAIN_GROUP>& ChainGrups)
{
	m_ChainGroups = ChainGrups;
	return S_OK;
}

HRESULT CDynamicBone::Create_Chain(_int RootIndex)
{
	if(-1 == RootIndex) return E_FAIL;
	
	for (auto Group : m_ChainGroups) {
		if (RootIndex == Group.RootBoneIndex)
			return S_OK;
	}
	
	DYNAMIC_CHAIN_GROUP Group;
	Group.RootBone = m_pAnimator->Get_ModelData()->Get_BoneNames()[RootIndex];
	Group.RootBoneIndex = RootIndex;

	vector<_int> Index;
	Index.push_back(RootIndex);

 	Create_Node(Index, Group);
	m_ChainGroups.push_back(Group);

	return S_OK;
}

void CDynamicBone::Create_Node(vector<_int> Indices, DYNAMIC_CHAIN_GROUP& ChineGroup)
{
	auto pModelData = m_pAnimator->Get_ModelData();

	const _int iBoneCount = pModelData->Get_BoneCount();
	_int iParentIndex = Indices.back();
	vector<_int> Childs;

	for (_int i = iParentIndex + 1; i < iBoneCount; ++i) {
		if (iParentIndex == pModelData->Get_BoneParentIndex(i))
			Childs.push_back(i);
	}

	if (Childs.empty()) {
		DYNAMIC_CHAIN chain;
		for (_int i = 1; i < Indices.size(); i++) {
			DYNAMIC_NODE Node{};
			Node.ParentIndex = Indices[i - 1];
			Node.BoneIndex = Indices[i];

			Matrix ParentMat = m_pAnimator->Get_TPose()[Node.ParentIndex];
			Matrix NodeMat = m_pAnimator->Get_TPose()[Node.BoneIndex];

			Node.fLength = (NodeMat.Translation() - ParentMat.Translation()).Length();

			Vector3 WorldDir = NodeMat.Translation() - ParentMat.Translation();
			Matrix parentRot = ParentMat;
			parentRot.Translation(Vector3::Zero);
			Vector3 LocalDir =	Vector3::TransformNormal(WorldDir, parentRot.Invert());

			Node.RestLocalDir = LocalDir;
			Node.RestLocalDir.Normalize();

			chain.Nodes.push_back(Node);
		}
		ChineGroup.Chains.push_back(chain);
		return;
	}

	for (_int i = 0; i < Childs.size(); i++) {
		vector<_int> newIndices = Indices;
		newIndices.push_back(Childs[i]);
		Create_Node(newIndices, ChineGroup);
	}
}

CDynamicBone* CDynamicBone::Create(CAnimator3D* pAnimator)
{
	CDynamicBone* instance = new CDynamicBone();

	if (FAILED(instance->Initialize(pAnimator))) {
		Safe_Release(instance);
	}

	return instance;
}

void CDynamicBone::Free()
{
	__super::Free();
	Safe_Release(m_pAnimator);
}