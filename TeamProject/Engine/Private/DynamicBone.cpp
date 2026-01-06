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
	return S_OK;
}

void CDynamicBone::Update(_float dt)
{
	if (m_ChainGroups.empty())
		return;

	for (auto& Group : m_ChainGroups) {
		/* 루트가 없으면 없는 그룹인거임 */
		if (-1 == Group.RootBoneIndex)
			continue;

		/* 루트가 설정되었다면 무조건 하위 체인은 만들어졌다는 가정하에 업데이트 */
		for (auto& Chain : Group.Chains) {
			for (_int i = 1; i < Chain.Nodes.size(); ++i) {
				_vector3 parentPosWS;

				if (i == 0)
					parentPosWS = m_pAnimator->Get_BoneCombinedPosition(Chain.Nodes[i].ParentIndex); // 1번
				else
					parentPosWS = Chain.Nodes[i - 1].WorldCurPos;

				SimulateNode(
					Chain.Nodes[i],
					parentPosWS,
					dt
				);
			}
		}
	}
}

/* 이미 저장된 데이터가 있으면 갖고옴 */
HRESULT CDynamicBone::Link_ChainData(const vector<DYNAMIC_CHAIN_GROUP>& ChainGrups)
{
	m_ChainGroups = ChainGrups;
	return S_OK;
}

/* 체인을 새로 생성하는 함수 */
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

void CDynamicBone::SimulateNode(DYNAMIC_NODE& Node, _vector3& ParentPos, _float dt)
{


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