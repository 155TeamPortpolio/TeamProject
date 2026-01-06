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

void CDynamicBone::Init_Update()
{
	for (auto& Group : m_ChainGroups) {
		for (auto& Chain : Group.Chains) {
			for (auto& Node : Chain.Nodes) {
				_vector3 curPos = m_pAnimator->Get_BoneCombinedPosition(Node.BoneIndex);

				Node.CombinedCurPos = curPos;
				Node.CombinedPrevPos = curPos;
			}
		}
	}

	m_bInitUpdated = true;
}

void CDynamicBone::Update(_float dt)
{
	if (m_ChainGroups.empty())
		return;

	if (false == m_bInitUpdated) {
		Init_Update();
		return;
	}

	/*업데이트가 안돌았으면 굳이 밑에 apply 시도할 필요가 없음*/
	_bool bUpdatedOnce = false;
	for (auto& Group : m_ChainGroups) {
		/* 루트가 없으면 없는 그룹인거임 */
		if (-1 == Group.RootBoneIndex)
			continue;

		/* 루트가 설정되었다면 무조건 하위 체인은 만들어졌다는 가정하에 업데이트 */
		for (auto& Chain : Group.Chains) {
			for (_int i = 0; i < Chain.Nodes.size(); ++i) {

				/* 첫번째본의 부모본은 루트본이니 직접 루트에서 갖고옴 */
				_vector3 parentPos =
					(i == 0)
					? m_pAnimator->Get_BoneCombinedPosition(Group.RootBoneIndex)
					: Chain.Nodes[i - 1].CombinedCurPos;

				_quaternion parentQuat =
					(i == 0)
					? m_pAnimator->Get_BoneCombinedQuaternion(Group.RootBoneIndex)
					: m_pAnimator->Get_BoneCombinedQuaternion(Chain.Nodes[i - 1].BoneIndex);

				/* 계산을 미리해봄 순서는 무조건 본의 위에서 아래로*/
				SimulateNode(
					Chain.Nodes[i],
					parentPos,
					parentQuat,
					Group.ChainParam,
					dt
				);
			}	
		}
		bUpdatedOnce = true;
	}

	/* 애니매이터 Manipulate에 직접 델타 매트릭스 추가*/
	if (bUpdatedOnce)
		ApplySimulatedNode();
}

void CDynamicBone::SimulateNode(DYNAMIC_NODE& Node,
	const _vector3& parentPos,
	const _quaternion& parentQuat,
	const CHAIN_PARAM& ChainParam,
	_float dt)
{
	/* 현재 / 이전 위치 */
	_vector3 cur = Node.CombinedCurPos;
	_vector3 prev = Node.CombinedPrevPos;

	/* 관성 : 다음 위치는 현재에서 속도와 감속만큼 곱한 위치를 미리계산 */
	_vector3 velocity = cur - prev;
	_vector3 next = cur + velocity * (1.f - ChainParam.fDamping);

	/* 부모로부터 멀어지면 안되니 
	(부모->노드)의 방향을 구하고 부모로부터 길이만큼 곱한 위치가 진짜 위치가 될거임*/
	_vector3 dir = next - parentPos;
	dir.Normalize();

	// 레스트 포즈로 복원
	_vector3 restDir =
		_vector3::Transform(Node.RestLocalDir, parentQuat);
	restDir.Normalize();

	dir = _vector3::Lerp(dir, restDir, ChainParam.fStiffness);
	dir.Normalize();

	next = parentPos + dir * Node.fLength;

	// 상태 갱신
	Node.CombinedPrevPos = cur;
	Node.CombinedCurPos = next;
}

void CDynamicBone::ApplySimulatedNode()
{
	for (auto& Group : m_ChainGroups) {
		for (auto& Chain : Group.Chains) {
			for (_int i = 0; i < Chain.Nodes.size(); ++i) {
				/* *여기서의 계산은 모델 스페이스상 계산* */
				auto& Node = Chain.Nodes[i];
				
				/* 이 노드는 어느 본에 매달려있는지?  ex) 0번노드 => 루트본에 매달림 */
				_vector3 parentPos =
					(i == 0)
					? m_pAnimator->Get_BoneCombinedPosition(Node.ParentIndex)
					: Chain.Nodes[i - 1].CombinedCurPos;

				/* 부모는 어느방향을 바라보고 있는지?*/
				/* 부모->자식의 방향이 필요하기에 그냥 Combined에서 회전만 갖고와도 무방 */
				_quaternion parentQuat =
					(i == 0)
					? m_pAnimator->Get_BoneCombinedQuaternion(Group.RootBoneIndex)
					: m_pAnimator->Get_BoneCombinedQuaternion(Chain.Nodes[i - 1].BoneIndex);

				/* RestLocal은 부모 본 로컬기준이고 비교대상은 Combined 모델스페이스상 방향이기에 */
				/* 실제로 부모가 회전한 만큼 기준을 돌려서 Combined 기준의 방향을 만들어줌 */
				_vector3 baseDirection =
					_vector3::Transform(Node.RestLocalDir, parentQuat);
				baseDirection.Normalize();

				/* 부모기준으로 노드가 실제로 움직이는 방향 (시뮬레이션 결과값)*/
				_vector3 simulatedDirection =
					Node.CombinedCurPos - parentPos;
				simulatedDirection.Normalize();

				/* 두 회전의 델타값을 구함 */
				_quaternion deltaRotation =
					_quaternion::FromToRotation(baseDirection, simulatedDirection);
				deltaRotation.Normalize();

				auto& pManipulateMat = (*m_pAnimator->Get_ManipulateBoneMatrices_Ptr())[Node.BoneIndex];
				pManipulateMat = Matrix::CreateFromQuaternion(deltaRotation);
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
	
	/* 이미 찍은 루트노드가 존재하면 무조건 체인이 만들어져있을것 */
	for (auto Group : m_ChainGroups) {
		if (RootIndex == Group.RootBoneIndex)
			return S_OK;
	}
	
	DYNAMIC_CHAIN_GROUP Group;
	Group.RootBone = m_pAnimator->Get_ModelData()->Get_BoneNames()[RootIndex];
	Group.RootBoneIndex = RootIndex;

	/* 	노드 생성을 하기 위한 리스트를 저장한 벡터를 하나 만들어둠 그 데이터로 재귀*/
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

	//인덱스중 마지막게 계산하는 부모노드가 될것
	_int iParentIndex = Indices.back();
	vector<_int> Childs;

	//트리계층이기에 무조건 자식인덱스는 부모보다 높은숫자임
	for (_int i = iParentIndex + 1; i < iBoneCount; ++i) {
		if (iParentIndex == pModelData->Get_BoneParentIndex(i))
			Childs.push_back(i);
	}

	//자식노드가 더이상 없으니 만듬
	if (Childs.empty()) {
		DYNAMIC_CHAIN chain;
		for (_int i = 1; i < Indices.size(); i++) {
			DYNAMIC_NODE Node{};
			Node.ParentIndex = Indices[i - 1];
			Node.BoneIndex = Indices[i];

			Matrix ParentMat = m_pAnimator->Get_TPose()[Node.ParentIndex];
			Matrix NodeMat = m_pAnimator->Get_TPose()[Node.BoneIndex];

			Node.fLength = (NodeMat.Translation() - ParentMat.Translation()).Length();

			_vector3 WorldDir = NodeMat.Translation() - ParentMat.Translation();
			Matrix parentRot = ParentMat;
			parentRot.Translation(_vector3::Zero);
			_vector3 LocalDir = _vector3::TransformNormal(WorldDir, parentRot.Invert());

			Node.RestLocalDir = LocalDir;
			Node.RestLocalDir.Normalize();

			Node.CombinedPrevPos = _vector3(NodeMat._41, NodeMat._42, NodeMat._43);
			Node.CombinedCurPos = _vector3(NodeMat._41, NodeMat._42, NodeMat._43);

			chain.Nodes.push_back(Node);
		}
		ChineGroup.Chains.push_back(chain);
		return;
	}

	//자식이 있으면 받아온 인덱스들을 그대로 복사해서 재귀
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
}