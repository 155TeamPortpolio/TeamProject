#include "Engine_Defines.h"
#include "DynamicBone.h"
#include "Animator3D.h"
#include "ModelData.h"
#include "GameObject.h"

CDynamicBone::CDynamicBone()
{
}

HRESULT CDynamicBone::Initialize(CAnimator3D* pAnimator)
{
	if (nullptr == pAnimator)
		return E_FAIL;

	m_pAnimator = pAnimator;
	m_pOwner = m_pAnimator->Get_Owner();

	return S_OK;
}

void CDynamicBone::Init_Update()
{
	for (auto& Group : m_ChainGroups) {
		//앵커
		Matrix OwnerWorldMat = m_pOwner->Get_WorldMatrix();

		Group.AnchorCombinedPos = m_pAnimator->Get_BoneCombinedPosition(Group.AnchorBoneIndex);
		Group.AnchorCombinedQuat = m_pAnimator->Get_BoneCombinedQuaternion(Group.AnchorBoneIndex);
		
		Group.AnchorWorldPos = _vector3::Transform(Group.AnchorCombinedPos, OwnerWorldMat);
		Group.AnchorWorldQuat = m_pOwner->Get_WorldQuat() * Group.AnchorCombinedQuat;

		for (auto& Chain : Group.Chains) {
			// 앵커(0번)부터 부모포지션을 세팅해두면 편함
			for (_int i = 0; i < (_int)Chain.Nodes.size(); ++i) {
				auto& Node = Chain.Nodes[i];

				_vector3 curPos = m_pAnimator->Get_BoneCombinedPosition(Node.BoneIndex);
				Node.CombinedPrevPos = Node.CombinedCurPos = curPos;

				if (Group.bWorldSpace) {
					Node.WorldPrevPos = Node.WorldCurPos = _vector3::Transform(curPos, OwnerWorldMat);
					Node.WorldCurQuat =
						m_pOwner->Get_WorldQuat() * _quaternion(m_pAnimator->Get_BoneCombinedQuaternion(Node.BoneIndex));
				}
			}
		}
	}

	m_bInitUpdated = true;
}

void CDynamicBone::Update(_float dt)
{
	if (m_ChainGroups.empty())
		return;

	/* *무조건* 이전프레임과 차이값으로 움직이기에 첫번째는 무조건 업데이트를 돌지 않음 */
	if (false == m_bInitUpdated) {
		Init_Update();
		return;
	}

	/* 업데이트가 안돌았으면 굳이 밑에 apply 시도할 필요가 없음 */
	_bool bUpdatedOnce = false;
	m_pAnimator->Reset_DynamicBoneMatrices();

	for (auto& Group : m_ChainGroups) {
		/* 앵커본이 설정되었다면 무조건 하위 체인은 만들어졌다는 가정하에 업데이트 */
		if (Group.AnchorBoneIndex == -1)
			continue;

		if (Group.bWorldSpace)
			WorldChain(Group, dt);
		else
			LocalChain(Group, dt);

		bUpdatedOnce = true;
	}

	/* 애니매이터 DynamicBoneMatrices에 회전 델타 매트릭스 추가*/
	if (bUpdatedOnce)
		ApplySimulatedNode();
}

#pragma region WorldChain
void CDynamicBone::WorldChain(DYNAMIC_CHAIN_GROUP& Group, _float dt)
{
	/* 앵커 노드 위치 업데이트 */
	Update_WorldAnchorNode(Group);

	/* 하위 체인들 시뮬레이션 */
	for (auto& Chain : Group.Chains) {
		for (_int i = 0; i < Chain.Nodes.size(); ++i) {
			/* 부모의 월드 위치와 회전을 갖고옴 */
			_vector3 parentPos =
				(i == 0) ?
				Group.AnchorWorldPos :
				Chain.Nodes[i - 1].WorldCurPos;

			_quaternion parentQuat =
				(i == 0) ? 
				Group.AnchorWorldQuat :
				Chain.Nodes[i - 1].WorldCurQuat;

			/* 계산을 미리해봄 순서는 무조건 본의 위에서 아래로*/
			Simulate_WorldNode(
				Chain.Nodes[i],
				parentPos,
				parentQuat,
				Group.ChainParam,
				dt
			);
		}
	}
}

void CDynamicBone::Update_WorldAnchorNode(DYNAMIC_CHAIN_GROUP& Group)
{
	Group.AnchorCombinedPos = m_pAnimator->Get_BoneCombinedPosition(Group.AnchorBoneIndex);
	Group.AnchorCombinedQuat = m_pAnimator->Get_BoneCombinedQuaternion(Group.AnchorBoneIndex);

	Group.AnchorWorldPos = _vector3::Transform(Group.AnchorCombinedPos, m_pOwner->Get_WorldMatrix());
	Group.AnchorWorldQuat = m_pOwner->Get_WorldQuat() * Group.AnchorCombinedQuat;
}

void CDynamicBone::Simulate_WorldNode(DYNAMIC_NODE& Node, const _vector3& parentPos, const _quaternion& parentQuat, const CHAIN_PARAM& ChainParam, _float dt)
{
	/* 현재 / 이전 위치 */
	_vector3 curPos = Node.CombinedCurPos;
	_vector3 prevPos = Node.CombinedPrevPos;

	/* 속력 계산 */
	_vector3 Velocity = curPos - prevPos;

	/* 관성(Damping) : 다음 위치는 현재에서 속도와 감속만큼 곱한 위치를 미리계산 */
	Velocity *= (1.f - ChainParam.fDamping);

	/* 중력(Gravity) : */
	_vector3 modelGravity(0.f, -1.f, 0.f);
	Velocity += modelGravity * ChainParam.fGravityScale * dt;

	/* 위에 모든 계산을 한 예측된 다음 위치*/
	_vector3 nextPos = curPos + Velocity;

	/* 부모로부터 멀어지면 안되니 부모뼈 스페이스위치에서 계산
	(부모->노드)의 방향을 구하고 부모로부터 길이만큼 곱한 위치가 진짜 위치가 될거임*/
	_vector3 dir = nextPos - parentPos;
	dir.Normalize();

	// 복원력(Stiffness) : 미리 구해둔 RestLocalDir로 점점 돌아오는 힘
	_vector3 restDir =
		_vector3::Transform(Node.RestLocalDir, parentQuat);
	restDir.Normalize();

	dir = _vector3::Lerp(dir, restDir, ChainParam.fStiffness);
	dir.Normalize();

	/* 본의 길이는 달라지면 안되니 재계산*/
	nextPos = parentPos + dir * Node.fLength;

	// 상태 갱신
	Node.CombinedPrevPos = curPos;
	Node.CombinedCurPos = nextPos;
}
#pragma endregion

#pragma region LocalChain
void CDynamicBone::LocalChain(DYNAMIC_CHAIN_GROUP& Group, _float dt)
{
	/* 앵커 노드 위치 업데이트 */
	Update_LocalAnchorNode(Group);

	/* 하위 체인들 시뮬레이션 */
	for (auto& Chain : Group.Chains) {
		for (_int i = 0; i < Chain.Nodes.size(); ++i) {
			/* 부모의 Combined된 위치와 회전을 갖고옴 */
			_vector3 parentPos =
				(i == 0) ?
				m_pAnimator->Get_BoneCombinedPosition(Group.AnchorBoneIndex) :
				Chain.Nodes[i - 1].CombinedCurPos;

			_quaternion parentQuat =
				m_pAnimator->Get_BoneCombinedQuaternion(Chain.Nodes[i].ParentIndex);

			/* 계산을 미리해봄 순서는 무조건 본의 위에서 아래로*/
			Simulate_LocalNode(
				Chain.Nodes[i],
				parentPos,
				parentQuat,
				Group.ChainParam,
				dt
			);
		}
	}
}

void CDynamicBone::Update_LocalAnchorNode(DYNAMIC_CHAIN_GROUP& Group)
{
	Group.AnchorCombinedPos = m_pAnimator->Get_BoneCombinedPosition(Group.AnchorBoneIndex);
	Group.AnchorCombinedQuat = m_pAnimator->Get_BoneCombinedQuaternion(Group.AnchorBoneIndex);
}

void CDynamicBone::Simulate_LocalNode(DYNAMIC_NODE& Node,
	const _vector3& parentPos,
	const _quaternion& parentQuat,
	const CHAIN_PARAM& ChainParam,
	_float dt)
{
	/* 현재 / 이전 위치 */
	_vector3 curPos = Node.CombinedCurPos;
	_vector3 prevPos = Node.CombinedPrevPos;

	/* 속력 계산 */
	_vector3 Velocity = curPos - prevPos;

	/* 관성(Damping) : 다음 위치는 현재에서 속도와 감속만큼 곱한 위치를 미리계산 */
	Velocity *= (1.f - ChainParam.fDamping);

	/* 중력(Gravity) : */
	_vector3 modelGravity(0.f, -1.f, 0.f);
	Velocity += modelGravity * ChainParam.fGravityScale * dt;

	/* 위에 모든 계산을 한 예측된 다음 위치*/
	_vector3 nextPos = curPos + Velocity;

	/* 부모로부터 멀어지면 안되니 부모뼈 스페이스위치에서 계산
	(부모->노드)의 방향을 구하고 부모로부터 길이만큼 곱한 위치가 진짜 위치가 될거임*/
	_vector3 dir = nextPos - parentPos;
	dir.Normalize();

	// 복원력(Stiffness) : 미리 구해둔 RestLocalDir로 점점 돌아오는 힘
	_vector3 restDir =
		_vector3::Transform(Node.RestLocalDir, parentQuat);
	restDir.Normalize();

	dir = _vector3::Lerp(dir, restDir, ChainParam.fStiffness);
	dir.Normalize();

	/* 본의 길이는 달라지면 안되니 재계산*/
	nextPos = parentPos + dir * Node.fLength;

	// 상태 갱신
	Node.CombinedPrevPos = curPos;
	Node.CombinedCurPos = nextPos;
}
#pragma endregion

/* 계산된 값에대한 회전 델타를 넘기는 작업 */
void CDynamicBone::ApplySimulatedNode()
{
	for (auto& Group : m_ChainGroups) {
		for (auto& Chain : Group.Chains) {
			for (_int i = 0; i < Chain.Nodes.size(); ++i) {
				/* *여기서의 계산은 모델 스페이스상 계산* */
				auto& Node = Chain.Nodes[i];
				
				/* 이 노드는 어느 본에 매달려있는지?  ex) 1번노드 => 0번노드(루트노드) 본에 매달림 */
				_vector3 parentPos =
					(i == 0) ?
					m_pAnimator->Get_BoneCombinedPosition(Group.AnchorBoneIndex) :
					Chain.Nodes[i - 1].CombinedCurPos;

				/* 부모는 어느방향을 바라보고 있는지?*/
				/* 부모->자식의 방향이 필요하기에 그냥 Combined에서 회전만 갖고와도 무방 */
				_quaternion parentQuat =
					m_pAnimator->Get_BoneCombinedQuaternion(Chain.Nodes[i].ParentIndex);

				/* RestLocal은 부모 본 로컬기준이고 비교대상은 Combined 모델스페이스상 방향이기에 */
				/* 실제로 부모가 회전한 만큼 기준을 돌려서 Combined 기준의 방향을 만들어줌 */
				_vector3 baseDirection = _vector3::Transform(Node.RestLocalDir, parentQuat);
				baseDirection.Normalize();

				/* 부모기준으로 노드가 실제로 움직이는 방향 (시뮬레이션 결과값)*/
				_vector3 simulatedDirection = Node.CombinedCurPos - parentPos;
				simulatedDirection.Normalize();

				/* 두 회전의 델타값을 구함 */
				_quaternion deltaRotation = _quaternion::FromToRotation(baseDirection, simulatedDirection);
				deltaRotation.Normalize();

				auto& pDynamicBoneMatrix = m_pAnimator->Get_DynamicBoneMatricesPtr()[Node.BoneIndex];
				pDynamicBoneMatrix = Matrix::CreateFromQuaternion(deltaRotation);
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
HRESULT CDynamicBone::Create_Chain(_int AnchorIndex)
{
	if(-1 == AnchorIndex) return E_FAIL;
	
	/* 이미 찍은 루트노드가 존재하면 무조건 체인이 만들어져있을것 */
	for (auto Group : m_ChainGroups) {
		if (AnchorIndex == Group.AnchorBoneIndex)
			return S_OK;
	}
	
	DYNAMIC_CHAIN_GROUP Group;
	Group.AnchorBoneIndex = AnchorIndex;

	/* 	노드 생성을 하기 위한 리스트를 저장한 벡터를 하나 만들어둠 그 데이터로 재귀*/
	vector<_int> Index;
	Index.push_back(AnchorIndex);
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

			Node.WorldPrevPos = _vector3::Transform(Node.CombinedPrevPos, m_pOwner->Get_WorldMatrix());
			Node.WorldCurPos = _vector3::Transform(Node.CombinedCurPos, m_pOwner->Get_WorldMatrix());

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