#include "Engine_Defines.h"
#include "DynamicBone.h"
#include "Animator3D.h"
#include "ModelData.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Engine_Math.h"

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

		Group.PrevAnchorCombinedPos = Group.CurAnchorCombinedPos =
			m_pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, Group.AnchorBoneIndex);

		for (auto& Chain : Group.Chains) {
			// 앵커(0번)부터 부모포지션을 세팅해두면 편함
			for (_int i = 0; i < (_int)Chain.Nodes.size(); ++i) {
				auto& Node = Chain.Nodes[i];

				//_vector3 curPos = m_pAnimator->Get_BoneCombinedPosition(Node.BoneIndex);
				_vector3 curPos = m_pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, Node.BoneIndex);
				Node.TipPrevCombinedPos = Node.TipCurCombinedPos = curPos;
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
	m_pAnimator->Reset_DynamicBoneMatrices();

	for (auto& Group : m_ChainGroups) {
		/* 앵커본이 설정되었다면 무조건 하위 체인은 만들어졌다는 가정하에 업데이트 */
		if (Group.AnchorBoneIndex == -1)
			continue;

		LocalChain(Group, dt);
	}		
}

#pragma region WorldChain
void CDynamicBone::WorldChain(DYNAMIC_CHAIN_GROUP& Group, _float dt)
{
	/* 앵커 월드 업데이트 */
	Update_WorldAnchor(Group);

	/* 각 노드의 실제 애니매이션 데이터 수집 */
	Update_WorldNodes(Group);

	/* 앵커의 델타를 구해옴 */
	_vector3 AnchorDeltaPos{};
	_quaternion AnchorDeltaQuat{};
	Calc_AnchorDelta(Group, AnchorDeltaPos, AnchorDeltaQuat);

	/* 월드 좌표계 -> 앵커(다이나믹본상 루트) 좌표계로 정렬 */
	for (auto& Chain : Group.Chains) {
		for (int i = 0; i < Chain.Nodes.size(); ++i)
		{
			/* 부모의 월드 위치와 회전을 갖고옴 */
			/* 이전 포지션도을 돌리는 이유는 앵커의 월드가 변한거라
				다이나믹본의 계산 속도에 섞이면 안됌 같은 좌표계에 일치시키기 위해 둘다 이전 앵커포즈로 */
			/* ex) 기차에서 공굴릴때 기차속도가 공이 굴러가는 속도에 영향이 가면안됌 */

			/* 0번은 기준이기에 계산안함 */
			if (i == 0)
				continue;

			//Calc_DynamicNode(Chain.Nodes[i],
			//	Group.PrevAnchorWorldPos,
			//	AnchorDeltaPos,
			//	AnchorDeltaQuat);
		}
	}


	/* 하위 체인들 시뮬레이션 */
	for (auto& Chain : Group.Chains) {
		for (_int i = 0; i < Chain.Nodes.size(); ++i) {
			//_vector3 parentPos =
			//	(i == 0) ?	Group.CurAnchorWorldPos :
			//				Chain.Nodes[i - 1].DynamicCurPos;
			//
			//_quaternion parentQuat =
			//	(i == 0) ? Group.CurAnchorWorldQuat:
			//			   Chain.Nodes[i - 1].DynamicCurQuat;
			//
			//if (i == 0)
			//{
			//	Chain.Nodes[i].DynamicCurPos = Chain.Nodes[i].AnimWorldPos;
			//	Chain.Nodes[i].DynamicPrevPos = Chain.Nodes[i].AnimWorldPos;
			//
			//	Chain.Nodes[i].DynamicCurQuat = parentQuat;
			//	Chain.Nodes[i].DynamicCurQuat = Chain.Nodes[i].AnimWorldQuat;
			//
			//	//Simulate_WorldQuat(
			//	//	Chain.Nodes[i],
			//	//	Group.CurAnchorWorldQuat,
			//	//	Group.ChainParam
			//	//);
			//
			//	continue;
			//}
			//
			//// Inert : 얼마나 월드 움직임에 영향을 받을것인가 
			//Vector3 parentDelta =
			//	(i == 0)
			//	? AnchorDeltaPos
			//	: Chain.Nodes[i - 1].DynamicPosDelta;
			//
			//
			//Chain.Nodes[i].DynamicCurPos += parentDelta * (1.f - Group.ChainParam.Inert);
			//Chain.Nodes[i].DynamicPrevPos += parentDelta * (1.f - Group.ChainParam.Inert);

			/* 월드기준의 위치를 예측하는 계산을 함 */
			//Simulate_WorldNode(
			//	Chain.Nodes[i],
			//	parentPos,
			//	parentQuat,
			//	Group.ChainParam,
			//	dt
			//);
			
			//Simulate_WorldQuat(
			//	Chain.Nodes[i],
			//	parentQuat,
			//	Group.ChainParam
			//);
		}
	}

	/* 애니매이터 DynamicBoneMatrices에 회전 델타 매트릭스 추가*/
	ApplySimulatedWorldNode(Group);
}

void CDynamicBone::Update_WorldAnchor(DYNAMIC_CHAIN_GROUP& Group)
{
	//Group.CurAnchorCombinedPos = m_pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, Group.AnchorBoneIndex);
	//Group.CurAnchorCombinedQuat = m_pAnimator->Get_BoneQuaternion(CAnimator3D::BoneSpace::COMBINED, Group.AnchorBoneIndex);
	//
	//Group.PrevAnchorWorldPos = Group.CurAnchorWorldPos;
	//Group.PrevAnchorWorldQuat = Group.CurAnchorWorldQuat;
	//
	//Group.CurAnchorWorldPos = _vector3::Transform(Group.CurAnchorCombinedPos, m_pOwner->Get_WorldMatrix());
	//Group.CurAnchorWorldQuat = m_pOwner->Get_WorldQuat() * Group.CurAnchorCombinedQuat;
}

void CDynamicBone::Update_WorldNodes(DYNAMIC_CHAIN_GROUP& Group)
{
	//Matrix OwnerWorldMat = m_pOwner->Get_WorldMatrix();
	//_quaternion OwnerWorldQuat = m_pOwner->Get_WorldQuat();
	//
	//for (auto& Chain : Group.Chains) {
	//	for (auto& Node : Chain.Nodes) {
	//		Node.CombinedPrevPos = Node.CombinedCurPos;
	//
	//		// 애니메이션 로컬 위치
	//		Node.CombinedCurPos =
	//			m_pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, Node.BoneIndex);
	//
	//		// 이번프레임 그 본의 월드위치
	//		Node.AnimWorldPos =
	//			_vector3::Transform(Node.CombinedCurPos, OwnerWorldMat);
	//
	//		// 이번프레임 그 본의 회전
	//		Node.AnimWorldQuat =
	//			m_pOwner->Get_WorldQuat() *
	//			m_pAnimator->Get_BoneQuaternion(CAnimator3D::BoneSpace::COMBINED, Node.BoneIndex);
	//		Node.AnimWorldQuat.Normalize();
	//	}
	//}
}

void CDynamicBone::Calc_AnchorDelta(DYNAMIC_CHAIN_GROUP& Group, _vector3& outDeltaPos, _quaternion& outDeltaQuat)
{
	//outDeltaPos = Group.CurAnchorWorldPos - Group.PrevAnchorWorldPos;
	//
	//_quaternion prevInv;
	//Group.PrevAnchorWorldQuat.Inverse(prevInv);
	//
	//outDeltaQuat = Group.CurAnchorWorldQuat * prevInv;
	//outDeltaQuat.Normalize();
}

void CDynamicBone::Calc_DynamicNode(DYNAMIC_NODE& Node, const _vector3& AnchorPrevPos, const _vector3& AnchorDeltaPos, const _quaternion& AnchorDeltaQuat)
{
	// 회전 기준점 = 이전 프레임 앵커 위치
	// 앵커의 이전포지션이 곧 회전이 얼마나됬냐의 축이 될거임
	//_vector3 pivot = AnchorPrevPos; 
	//_quaternion InvAnchorDeltaQuat;
	//AnchorDeltaQuat.Inverse(InvAnchorDeltaQuat);
	//
	//_vector3 vCur = Node.DynamicCurPos - pivot;
	//_vector3 vPrev = Node.DynamicPrevPos - pivot;
	//
	//vCur = _vector3::Transform(vCur, InvAnchorDeltaQuat);
	//vPrev = _vector3::Transform(vPrev, InvAnchorDeltaQuat);
	//
	//Node.DynamicCurPos = pivot + vCur;
	//Node.DynamicPrevPos = pivot + vPrev;
}

void CDynamicBone::Simulate_WorldNode(DYNAMIC_NODE& Node,
	const _vector3& parentPos,
	const _quaternion& parentQuat,
	const CHAIN_PARAM& ChainParam,
	_float dt)
{
	/* 현재 / 이전 위치 */
	//_vector3 curPos = Node.DynamicCurPos;
	//_vector3 prevPos = Node.DynamicPrevPos;
	//
	///* 1. 관성(Verlet) : 속력 계산 */
	//_vector3 Velocity = curPos - prevPos;
	//
	///* 2. 중력(Gravity) : 중력 방향으로 이동 (월드기준) */
	//Velocity += ChainParam.GravityDir * (ChainParam.GravityScale);
	//
	//// 3. 탄성(Elasticity) : 애니매이션으로 돌아오는 힘
	//Velocity += (Node.AnimWorldPos - curPos) * (ChainParam.Elasticity);
	//
	///* 4. 감쇄(Damping) : 다음 위치는 현재에서 속도와 감속만큼 곱한 위치를 미리계산 */
	//Velocity *= (1.f - ChainParam.Damping);
	//
	///* 5. 위에 모든 계산을 한 예측된 다음 위치 예측 */
	//_vector3 nextPos = curPos + Velocity;
	//
	///* 부모로부터 멀어지면 안되니 부모뼈 위치에서 계산 뼈가 길어지면 안돼잖음 그래서 길이제한을 함
	//(부모->노드)의 방향을 구하고 부모로부터 길이만큼 곱한 위치가 진짜 위치가 될거임*/
	///* 6. 본의 길이는 달라지면 안되니 재계산해서 길이 제한 */
	//_vector3 dir = nextPos - parentPos;
	//dir.Normalize();
	//
	//
	//// 7. 강성 (Stiffness)
	//Vector3 animDir = (Node.AnimWorldPos - parentPos);
	//animDir.Normalize();
	//
	//Vector3 restDir = Vector3::TransformNormal(Node.RestLocalDir, Matrix::CreateFromQuaternion(parentQuat));
	//restDir.Normalize();
	//
	//Vector3 targetDir;
	//(m_pAnimator->Get_CurAnimIndex() == -1) ? targetDir = restDir : targetDir = animDir;
	//
	//dir = Vector3::Lerp(dir, targetDir, ChainParam.Stiffness);
	//dir.Normalize();
	//
	//nextPos = parentPos + dir * Node.fLength;
	//
	//// 8. 상태 갱신
	//Node.DynamicPrevPos = curPos;
	//Node.DynamicCurPos = nextPos;
	//Node.DynamicPosDelta = Node.DynamicCurPos - Node.DynamicPrevPos;
}

void CDynamicBone::Simulate_WorldQuat(DYNAMIC_NODE& Node, const _quaternion& parentQuat, const CHAIN_PARAM& ChainParam)
{
	//// 1. 현재 / 이전 다이나믹 회전
	//Quaternion curQuat = Node.DynamicCurQuat;
	//Quaternion InvcurQuat; curQuat.Inverse(InvcurQuat);
	//
	//Quaternion prevQuat = Node.DynamicPrevQuat;
	//Quaternion InvprevQuat; prevQuat.Inverse(InvprevQuat);
	//
	//// 2. 각속도 (회전 베를레)
	//Quaternion angularVel = curQuat * InvprevQuat;
	//angularVel.Normalize();
	//
	//// 3. 회전 감쇠 (관성 유지)
	//angularVel = Quaternion::Slerp(Quaternion::Identity, angularVel, 1.f - ChainParam.Damping);
	//angularVel.Normalize();
	//
	//// 4. 애니메이션 목표 회전 (부모 기준)
	//_quaternion InvParentQuat;
	//parentQuat.Inverse(InvParentQuat);
	//
	//Quaternion target =	Node.AnimWorldQuat * InvParentQuat;
	//target.Normalize();
	//
	//// 5. 현재 → 목표 회전 스프링
	//Quaternion InvCurQuat;
	//curQuat.Inverse(InvCurQuat);
	//
	//Quaternion toTarget = target * InvCurQuat;
	//toTarget.Normalize();
	//
	//Quaternion spring =
	//	Quaternion::Slerp(Quaternion::Identity, toTarget, ChainParam.Stiffness);
	//spring.Normalize();
	//
	//// 6. 다음 회전 예측
	//Quaternion nextQuat = spring * angularVel * curQuat;
	//nextQuat.Normalize();
	//
	//// 7. 상태 갱신
	//Node.DynamicPrevQuat = curQuat;
	//Node.DynamicCurQuat = nextQuat;
}

void CDynamicBone::ApplySimulatedWorldNode(DYNAMIC_CHAIN_GROUP& Group)
{
	for (auto& Chain : Group.Chains) {
		for (_int i = 0; i < Chain.Nodes.size(); ++i) {
			///* *여기서의 계산은 모델 스페이스상 계산* */
			//auto& Node = Chain.Nodes[i];
			//
			///* ---------- 부모 월드 위치 ---------- */
			//Vector3 parentWorldPos =
			//	(i == 0)
			//	? Group.CurAnchorWorldPos
			//	: Chain.Nodes[i - 1].DynamicCurPos;
			//
			///* ---------- 부모 월드 회전 ---------- */
			//Quaternion parentWorldQuat =
			//	(i == 0)
			//	? Group.CurAnchorWorldQuat
			//	: m_pOwner->Get_WorldQuat() * m_pAnimator->Get_BoneQuaternion(CAnimator3D::BoneSpace::COMBINED,	Node.ParentIndex);
			//
			//parentWorldQuat.Normalize();
			//
			///* --- 부모 로컬 기준으로 방향 변환 --- */
			//Quaternion parentInv = parentWorldQuat;
			//parentInv.Inverse(parentInv);
			//
			//// 애니메이션 기준 방향 (부모 로컬)
			//_vector3 animDirL = _vector3::Transform(Node.AnimWorldPos - parentWorldPos, parentInv);
			//animDirL.Normalize();
			//
			//// 시뮬레이션 결과 방향 (부모 로컬)
			//_vector3 simDirL = _vector3::Transform(Node.DynamicCurPos - parentWorldPos, parentInv);
			//simDirL.Normalize();
			//
			///* --- 로컬 기준 회전 델타 --- */
			//_quaternion deltaLocal = _quaternion::FromToRotation(animDirL, simDirL);
			//deltaLocal.Normalize();
			//
			///* --- Additive용 로컬 델타 저장 --- */
			////m_pAnimator->Get_DynamicBoneMatricesPtr()[Node.BoneIndex] =
			////	Matrix::CreateFromQuaternion(deltaLocal);
			//m_pAnimator->Get_DynamicBoneMatricesPtr()[Node.BoneIndex] = Matrix::Identity;
		}
	}
}
#pragma endregion

#pragma region LocalChain
void CDynamicBone::LocalChain(DYNAMIC_CHAIN_GROUP& Group, _float dt)
{
	/* 앵커 위치 업데이트 */
	Update_LocalAnchorNode(Group);

	/* 하위 체인들 시뮬레이션 */
	for (auto& Chain : Group.Chains) {
		for (_int i = 0; i < Chain.Nodes.size(); ++i) {
			/* 부모의 Combined된 위치와 회전을 갖고옴 */
			_vector3 parentPos =
				(i == 0) ?
				m_pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, Group.AnchorBoneIndex)
				: Chain.Nodes[i - 1].TipCurCombinedPos;

			_quaternion parentQuat =
				(i == 0)
				? m_pAnimator->Get_BoneQuaternion(CAnimator3D::BoneSpace::COMBINED,	Group.AnchorBoneIndex)
				: m_pAnimator->Get_BoneQuaternion(CAnimator3D::BoneSpace::COMBINED,	Chain.Nodes[i - 1].BoneIndex);

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

	/* 애니매이터 DynamicBoneMatrices에 회전 델타 매트릭스 추가*/
	ApplySimulatedLocalNode(Group);
}

void CDynamicBone::Update_LocalAnchorNode(DYNAMIC_CHAIN_GROUP& Group)
{
	Group.PrevAnchorCombinedPos = Group.CurAnchorCombinedPos;
	Group.CurAnchorCombinedPos = m_pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, Group.AnchorBoneIndex);
}

//void CDynamicBone::Calc_Distribution(_int NodeCount, DYNAMIC_CHAIN_GROUP& Group)
//{
//	float eased = Math::ApplyEase(d.Ease, t); // Linear, EaseIn, EaseOut 등
//	return eased * d.Strength;
//}

void CDynamicBone::Simulate_LocalNode(DYNAMIC_NODE& Node,
	const _vector3& parentPos,
	const _quaternion& parentQuat,
	const CHAIN_PARAM& ChainParam,
	_float dt)
{
	/* 현재 / 이전 위치 */
	_vector3 curPos = Node.TipCurCombinedPos;
	_vector3 prevPos = Node.TipPrevCombinedPos;

	/* 속력 계산 */
	_vector3 Velocity = curPos - prevPos;

	/* 관성(Damping) : 다음 위치는 현재에서 속도와 감속만큼 곱한 위치를 미리계산 */
	Velocity *= (1.f - ChainParam.Damping);

	/* 중력(Gravity) : */
	Velocity += ChainParam.GravityDir * ChainParam.GravityScale;

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

	dir = _vector3::Lerp(dir, restDir, ChainParam.Stiffness);
	dir.Normalize();

	/* 본의 길이는 달라지면 안되니 재계산*/
	nextPos = parentPos + dir * Node.fLength;

	// 상태 갱신
	Node.TipPrevCombinedPos = Node.TipCurCombinedPos;
	Node.TipCurCombinedPos = nextPos;
}

/* 계산된 값에대한 회전 델타를 넘기는 작업 */
void CDynamicBone::ApplySimulatedLocalNode(DYNAMIC_CHAIN_GROUP& Group)
{
	for (auto& Chain : Group.Chains) {
		for (_int i = 0; i < Chain.Nodes.size(); ++i) {
			/* *여기서의 계산은 모델 스페이스상 계산* */
			auto& Node = Chain.Nodes[i];
			
			/* 이 노드는 어느 본에 매달려있는지?  ex) 1번노드 => 0번노드(루트노드) 본에 매달림 */
			_vector3 parentPos =
				(i == 0) ?
				m_pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, Group.AnchorBoneIndex) :
				Chain.Nodes[i - 1].TipCurCombinedPos;

			/* 부모는 어느방향을 바라보고 있는지?*/
			/* 부모->자식의 방향이 필요하기에 그냥 Combined에서 회전만 갖고와도 무방 */
			_quaternion parentQuat =
				m_pAnimator->Get_BoneQuaternion(CAnimator3D::BoneSpace::COMBINED, Chain.Nodes[i].ParentIndex);

			/* RestLocal은 부모 본 로컬기준이고 비교대상은 Combined 모델스페이스상 방향이기에 */
			/* 실제로 부모가 회전한 만큼 기준을 돌려서 Combined 기준의 방향을 만들어줌 */
			_vector3 baseDirection = _vector3::Transform(Node.RestLocalDir, parentQuat);
			baseDirection.Normalize();

			/* 부모기준으로 노드가 실제로 움직이는 방향 (시뮬레이션 결과값)*/
			_vector3 simulatedDirection = Node.TipCurCombinedPos - parentPos;
			simulatedDirection.Normalize();

			/* 두 회전의 델타값을 구함 */
			_quaternion deltaRotation = _quaternion::FromToRotation(baseDirection, simulatedDirection);
			deltaRotation.Normalize();

			auto& pDynamicBoneMatrix = m_pAnimator->Get_DynamicBoneMatricesPtr()[Node.BoneIndex];
			pDynamicBoneMatrix = Matrix::CreateFromQuaternion(deltaRotation);
		}
	}
}
#pragma endregion

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

			Node.ParentIndex = (i == 0) ?  -1 : Indices[i - 1];
			Node.BoneIndex = Indices[i];

			//if (Node.ParentIndex == -1) {
			//	Node.ParentIndex = -1;
			//	Node.fLength = 0.f;
			//	Node.RestLocalDir = _vector3::Zero;
			//
			//	chain.Nodes.push_back(Node);
			//
			//	continue;
			//}

			Matrix ParentMat = m_pAnimator->Get_TPose()[Node.ParentIndex];
			Matrix NodeMat = m_pAnimator->Get_TPose()[Node.BoneIndex];

			Node.fLength = (NodeMat.Translation() - ParentMat.Translation()).Length();

			_vector3 WorldDir = NodeMat.Translation() - ParentMat.Translation();
			Matrix parentRot = ParentMat;
			parentRot.Translation(_vector3::Zero);
			_vector3 LocalDir = _vector3::TransformNormal(WorldDir, parentRot.Invert());

			Node.RestLocalDir = LocalDir;
			Node.RestLocalDir.Normalize();

			//Node.CombinedPrevPos = _vector3(NodeMat._41, NodeMat._42, NodeMat._43);
			//Node.CombinedCurPos = _vector3(NodeMat._41, NodeMat._42, NodeMat._43);
			
			//Node.WorldPrevPos = _vector3::Transform(Node.CombinedPrevPos, m_pOwner->Get_WorldMatrix());
			//Node.WorldCurPos = _vector3::Transform(Node.CombinedCurPos, m_pOwner->Get_WorldMatrix());

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

#pragma region GUI
void CDynamicBone::Render_GUI()
{
	ImGui::SeparatorText("DynamicBone");
	if (m_ChainGroups.empty())
		return;

	auto& Group = m_ChainGroups[0];

	if (!ImGui::Begin("DynamicBone Debug"))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("=== ChainGroup[0] ===");

	/* ---------------- Anchor ---------------- */
	ImGui::Separator();
	ImGui::Text("Anchor : %d", Group.AnchorBoneIndex);
	ImGui::Text("CombPos : %.3f %.3f %.3f",
		Group.CurAnchorCombinedPos.x,
		Group.CurAnchorCombinedPos.y,
		Group.CurAnchorCombinedPos.z);

	/* ---------------- Chain Param ---------------- */
	ImGui::Separator();
	ImGui::Text("Chain Parameters");

	ImGui::DragFloat("Inert", &Group.ChainParam.Inert, 0.001f, 0.f, 1.f, "%.3f");
	ImGui::DragFloat("Damping",	&Group.ChainParam.Damping,	0.001f, 0.f, 1.f, "%.3f");
	ImGui::DragFloat("Elasticity", &Group.ChainParam.Elasticity, 0.001f, 0.f, 1.f, "%.3f");
	ImGui::DragFloat("Stiffness", &Group.ChainParam.Stiffness, 0.001f, 0.f, 1.f, "%.3f");
	ImGui::DragFloat("Gravity Scale", &Group.ChainParam.GravityScale, 0.001f, 0.f, 10.f, "%.3f");

	static bool bDrawDebugPoints = true;
	ImGui::Checkbox("Draw DynamicBone Points",
		&bDrawDebugPoints);

	static float dotsize = 2.f;
	ImGui::DragFloat("DotSize", &dotsize, 0.01f, 0.f, 10.f, "%.2f");

	for (auto& Group : m_ChainGroups) {
		Group.ChainParam = m_ChainGroups[0].ChainParam;
	}

	/* ---------------- Nodes ---------------- */
	ImGui::Separator();
	ImGui::Text("Nodes (World)");

	for (size_t c = 0; c < Group.Chains.size(); ++c)
	{
		auto& Chain = Group.Chains[c];

		if (ImGui::TreeNode((void*)(intptr_t)c, "Chain %zu", c))
		{
			for (size_t i = 0; i < Chain.Nodes.size(); ++i)
			{
				auto& Node = Chain.Nodes[i];

				ImGui::PushID((int)i);

				ImGui::Text("Node %zu (Bone %d)", i, Node.BoneIndex);

				ImGui::Text("Prev Pos : %.3f %.3f %.3f",
					Node.TipPrevCombinedPos.x,
					Node.TipPrevCombinedPos.y,
					Node.TipPrevCombinedPos.z);

				ImGui::Text("Cur Pos : %.3f %.3f %.3f",
					Node.TipCurCombinedPos.x,
					Node.TipCurCombinedPos.y,
					Node.TipCurCombinedPos.z);


				ImGui::Text("Len : %.3f", Node.fLength);

				ImGui::Separator();
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}

	if (bDrawDebugPoints)
	{
		ImGuizmo::SetGizmoSizeClipSpace(0.005f);

		for (auto Group : m_ChainGroups) {
			Render_DynamicBone(Group.CurAnchorCombinedPos, dotsize, IM_COL32(255, 255, 0, 255));
			for (auto& Chain : Group.Chains) {
				for (int i = 0; i < Chain.Nodes.size(); i++) {
					if(i == 0 && i == Chain.Nodes.size() - 1)
						Render_DynamicBone(Chain.Nodes[i].TipCurCombinedPos, dotsize, IM_COL32(200, 200, 200, 255));
					else if (i == 0)
						Render_DynamicBone(Chain.Nodes[i].TipCurCombinedPos, dotsize, IM_COL32(100, 100, 255, 255));
					else if (i == Chain.Nodes.size() - 1)
						Render_DynamicBone(Chain.Nodes[i].TipCurCombinedPos, dotsize, IM_COL32(100, 255, 100, 255));
					else
						Render_DynamicBone(Chain.Nodes[i].TipCurCombinedPos, dotsize);
				}
			}
		}

		ImGuizmo::SetGizmoSizeClipSpace(0.1f);
	}

	ImGui::End();
}

void CDynamicBone::Render_DynamicBone(_vector3 vPos, _float dotSize, ImU32 color)
{
	ImGuizmo::BeginFrame();
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

	GUI_CONTEXT* Context = CGameInstance::GetInstance()->Get_GUISystem()->Get_Context();

	const Matrix& view = *Context->pCameraManager->Get_ViewMatrix();
	const Matrix& proj = *Context->pCameraManager->Get_ProjMatrix();

	// World → Clip
	Vector4 clip = Vector4::Transform(_vector4(vPos.x, vPos.y, vPos.z, 1.f), view * proj);

	// 카메라 뒤면 무시
	if (clip.w <= 0.f)
		return;

	// NDC
	float ndcX = clip.x / clip.w;
	float ndcY = clip.y / clip.w;

	// Screen 좌표
	float sx = (ndcX * 0.5f + 0.5f) * Context->viewPort.x;
	float sy = (-ndcY * 0.5f + 0.5f) * Context->viewPort.y;

	// 점 찍기
	ImGui::GetBackgroundDrawList()->AddCircleFilled(
		ImVec2(sx, sy),
		dotSize,     // 점 크기
		color    // 색
	);
}
#pragma endregion

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