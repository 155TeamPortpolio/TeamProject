#include "Engine_Defines.h"
#include "Animator3D.h"
#include "AnimationClip.h"
#include "ModelData.h"
#include "GameInstance.h"
#include "IResourceService.h"

CAnimator3D::CAnimator3D()
{
}

CAnimator3D::CAnimator3D(const CAnimator3D& rhs)
	:CComponent(rhs), m_pAnimClips(rhs.m_pAnimClips), m_pData{ rhs.m_pData },
	m_TransformationMatrices{ rhs.m_TransformationMatrices },
	m_CombinedMatrices{ rhs.m_CombinedMatrices }
{
	for (auto& Clip : m_pAnimClips) {
		Safe_AddRef(Clip);
	}
	Safe_AddRef(m_pData);
}

HRESULT CAnimator3D::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimator3D::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

void CAnimator3D::LinkAnimate_Model(const string& LevelKey, const string& ModelKey)
{
	if (m_pData) {
		Reset_Anim();
	}

	m_pData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(LevelKey, ModelKey);
	Safe_AddRef(m_pData);
	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	m_TransformationMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_ManipulateMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_CombinedMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);

	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		m_TransformationMatrices[i] = m_pData->Get_TransformMatrix(i);
	}
	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		int parent = m_pData->Get_BoneParentIndex(i);

		if (parent == -1) {
			m_CombinedMatrices[i] = m_TransformationMatrices[i];
		}
		else {
			_matrix ParentCombine = XMLoadFloat4x4(&m_CombinedMatrices[parent]);
			_matrix MyTransformation = XMLoadFloat4x4(&m_TransformationMatrices[i]);
			XMStoreFloat4x4(&m_CombinedMatrices[i], MyTransformation * ParentCombine);
		}
	}

	m_TPose = m_CombinedMatrices;
	m_BasePose.resize(m_pData->Get_BoneCount(), IdentityMatrix);
}

HRESULT CAnimator3D::Link_MetaData(const string& LevelKey, const string& MetaClipKey)
{
	ANIMATION_META Meta = CGameInstance::GetInstance()->Get_ResourceMgr()->
		Load_MetaClip(LevelKey, MetaClipKey);

	if (Meta.pClips.empty()) {
		string msg = "Anim Add Failed: " + MetaClipKey + "\n";
		OutputDebugStringA(msg.c_str());
		return E_FAIL;
	}

	m_PreTransform = Meta.PreTransform;
	m_pAnimClips = Meta.pClips;

	for (auto& pClip : Meta.pClips)
		Safe_AddRef(pClip);

	Resize_Layer(1);
	//0번 레이어는 베이스레이어, Layer.BaseLayer는 웬만하면 건들지 말것
	m_AnimLayers[0].BaseLayer = true;
	m_AnimLayers[0].eLayerType = ANIM_LAYER_STATE::BASE;
	m_AnimLayers[0].fLayerWeight = 1.f;
	m_AnimLayers[0].iRootBoneIndex = m_pData->Find_BoneIndexByName("Root");

	return S_OK;
}

HRESULT CAnimator3D::Resize_Layer(_uint iLayerCount)
{
	if (isExistLayer(iLayerCount))
		return S_OK;

	_int iCreateCount = iLayerCount - m_AnimLayers.size();
	while (0 < iCreateCount)
	{
		_float4x4 IdentityMatrix;
		XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

		ANIM_LAYER Layer{};
		Layer.LocalMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
		Layer.BlendMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
		Layer.FinalLocalMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);

		m_AnimLayers.push_back(Layer);

		iCreateCount--;
	}

	return S_OK;
}

void CAnimator3D::Update_Animation(_float dt)
{
	if (m_pAnimClips.empty()) return;

	Clear_Events();

	for (auto& Layer : m_AnimLayers) {
		if (Layer.bPause) continue;
		if (Layer.fLayerWeight <= 0) continue;

		Layer.bApplied = false;

		if (Layer.bBlending)
			Animation_Convert(Layer, dt);
		else
			Animation_Run(Layer, dt);
	}

	BuildBone(dt);
}

SetAnimBuild CAnimator3D::Set_Animation(AnimArg ClipArg)
{
	return SetAnimBuild(0, Resolve_ClipIndex(ClipArg), this);
}

SetAnimBuild CAnimator3D::Set_Animation(_uint LayerIndex, AnimArg ClipArg)
{
	return SetAnimBuild(LayerIndex, Resolve_ClipIndex(ClipArg), this);
}

ChangeAnimBuild CAnimator3D::Change_Animation(AnimArg ClipArg)
{
	return ChangeAnimBuild(0, Resolve_ClipIndex(ClipArg), this);
}

ChangeAnimBuild CAnimator3D::Change_Animation(_uint LayerIndex, AnimArg ClipArg)
{
	return ChangeAnimBuild(LayerIndex, Resolve_ClipIndex(ClipArg), this);
}

void CAnimator3D::Reset_Layer(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex))
		return;

	ANIM_LAYER& Layer = m_AnimLayers[LayerIndex];

	Layer.iStartBoneIndex = -1;
	Layer.AffectedBonesIndices.clear();

	Layer.iClipIndex = -1;
	Layer.fPrevTrackPosition = 0.f;
	Layer.fCurrentTrackPosition = 0.f;
	Layer.ePlayEaseType = { EaseType::None };
	Layer.bLoop = false;
	Layer.bisFinished = true;

	Layer.bBlending = { false };
	Layer.iNextClipIndex = { -1 };
	Layer.fBlendElapsed = 0.f;
	Layer.fBlendDuration = 0.f;
	Layer.eBlendEaseType = { EaseType::None };

	Matrix identityMat = XMMatrixIdentity();

	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());
	Layer.LocalMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	Layer.BlendMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	Layer.FinalLocalMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
}

HRESULT CAnimator3D::Stop_Animation(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return E_FAIL;

	m_AnimLayers[LayerIndex].bPause = true;

	return S_OK;
}

HRESULT CAnimator3D::StopAll_Animation()
{
	for (auto& Layer : m_AnimLayers)
		Layer.bPause = true;

	return S_OK;
}

#pragma region AnimationDatas

_bool CAnimator3D::isCurrentAnimEnd(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex))
		return true;

	ANIM_LAYER& Layer = m_AnimLayers[LayerIndex];

	if (!isExistClip(Layer.iClipIndex))
		return true;

	if (Layer.bBlending)
		return false;

	if (Layer.bLoop)
		return false;

	return Layer.bisFinished;
}

_bool CAnimator3D::isOverClipTiming(_float percent, _uint LayerIndex)
{
	if (!isExistLayer(LayerIndex))
		return true;

	ANIM_LAYER& Layer = m_AnimLayers[LayerIndex];

	if (Layer.bBlending) { //Blending
		if (!isExistClip(Layer.iNextClipIndex))
			return true;

		auto& nextClip = m_pAnimClips[Layer.iNextClipIndex];

		_float threshold = nextClip->Get_Duration() * percent;

		return Layer.fBlendTrackPosition >= threshold;
	}
	else { //NonBlending
		if (!isExistClip(Layer.iClipIndex))
			return true;

		auto& nowClip = m_pAnimClips[Layer.iClipIndex];

		_float threshold = nowClip->Get_Duration() * percent;

		return Layer.fCurrentTrackPosition >= threshold;
	}
}

_bool CAnimator3D::isBlending(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex))
		return false;

	return m_AnimLayers[LayerIndex].bBlending;
}

_float CAnimator3D::Get_CurAnimDuration(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex))
		return 0.f;

	ANIM_LAYER& Layer = m_AnimLayers[LayerIndex];

	if (Layer.bBlending) {
		if (!isExistClip(Layer.iNextClipIndex))
			return 0.f;

		auto& nextClip = m_pAnimClips[Layer.iNextClipIndex];

		return Layer.fBlendTrackPosition / nextClip->Get_Duration();
	}
	else {
		if (!isExistClip(Layer.iClipIndex))
			return 0.f;

		auto& nowClip = m_pAnimClips[Layer.iClipIndex];

		return Layer.fCurrentTrackPosition / nowClip->Get_Duration();
	}
}

string CAnimator3D::Get_CurAnimName(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return "";

	ANIM_LAYER& Layer = m_AnimLayers[LayerIndex];

	if (Layer.bBlending) {
		if (isExistClip(Layer.iNextClipIndex))
			return m_pAnimClips[Layer.iNextClipIndex]->Get_Name();
	}
	else {
		if (isExistClip(Layer.iClipIndex))
			return m_pAnimClips[Layer.iClipIndex]->Get_Name();
	}

	return "";
}

_int CAnimator3D::Get_CurAnimIndex(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return -1;

	return m_AnimLayers[LayerIndex].bBlending ?
		m_AnimLayers[LayerIndex].iNextClipIndex : m_AnimLayers[LayerIndex].iClipIndex;
}

_int CAnimator3D::Get_NumLayer()
{
	return (_int)m_AnimLayers.size();
}

const vector<EVENT_INST>& CAnimator3D::Get_EventBus() const
{
	return m_EventBus;
}

_float3 CAnimator3D::Get_RootBoneMoveDelta() const
{
	for (auto& Layer : m_AnimLayers)
		if (Layer.BaseLayer)
			return Layer.vRootMoveDelta;

	return _float3();
}

_float4 CAnimator3D::Get_RootBoneQuatDelta() const
{
	for (auto& Layer : m_AnimLayers)
		if (Layer.BaseLayer)
			return Layer.vRootQuatDelta;

	return Quaternion::Identity;
}

_float4 CAnimator3D::Get_RootBoneEndQuat() const
{
	for (auto& Layer : m_AnimLayers)
		if (Layer.BaseLayer)
			return Layer.vRootEndQuat;

	return Quaternion::Identity;
}

_vector CAnimator3D::Get_MotionBoneDelta(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return XMVectorZero();

	if (-1 == m_AnimLayers[LayerIndex].iMotionBoneIndex)
		return XMVectorZero();

	_float4x4 MoveBoneMat =
		m_AnimLayers[LayerIndex].LocalMatrices[m_AnimLayers[LayerIndex].iMotionBoneIndex];

	_float3 fCurAnimPos = { MoveBoneMat._41, MoveBoneMat._42, MoveBoneMat._43 };

	return (XMLoadFloat3(&fCurAnimPos) - XMLoadFloat3(&m_AnimLayers[LayerIndex].vPrevMotionBonePos));
}

_float CAnimator3D::Get_EaseDuration(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return 0.f;

	auto& Layer = m_AnimLayers[LayerIndex];
	if (EaseType::None == Layer.ePlayEaseType)
		return 0.f;

	return Layer.fEaseElapsed / Layer.fEaseDuration;
}

_float CAnimator3D::Get_AnimSpeed(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return 0.f;

	return m_AnimLayers[LayerIndex].fAnimSpeed;
}

_bool CAnimator3D::Get_isPause(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return true;

	return m_AnimLayers[LayerIndex].bPause;
}

void CAnimator3D::Set_MotionBone(_int MoveBoneIndex)
{
	for (auto& Layer : m_AnimLayers) {
		if (!Layer.BaseLayer) continue;

		Layer.iMotionBoneIndex = MoveBoneIndex;
	}
}

void CAnimator3D::Set_ExtractMotionboneMovement(AXIS eAxis)
{
	for (auto& Layer : m_AnimLayers) {
		if (!Layer.BaseLayer) continue;

		Layer.eExtractMoveAxis = eAxis;
	}
}

void CAnimator3D::Reset_ExtractBoneMovement()
{
	for (auto& Layer : m_AnimLayers) {
		if (!Layer.BaseLayer) continue;

		Layer.eExtractMoveAxis = AXIS::NONE;
	}
}

void CAnimator3D::Set_Pause(_bool bPause, _uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return;
	m_AnimLayers[LayerIndex].bPause = bPause;
}

void CAnimator3D::Set_StartBone(_int StartBoneIndex, _uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return;

	m_AnimLayers[LayerIndex].iStartBoneIndex = StartBoneIndex;
	m_pData->Get_AffectBoneIndices(
		m_AnimLayers[LayerIndex].AffectedBonesIndices,
		m_AnimLayers[LayerIndex].iStartBoneIndex);
}

void CAnimator3D::Reset_StartBone(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return;

	m_AnimLayers[LayerIndex].iStartBoneIndex = -1;
	m_AnimLayers[LayerIndex].AffectedBonesIndices.clear();
}

void CAnimator3D::Set_TPose()
{
	for (_uint i = 0; i < m_AnimLayers.size(); i++)
	{
		Reset_Layer(i);
		if (0 < i)
			m_AnimLayers[i].eLayerType = ANIM_LAYER_STATE::NONE;
	}

	m_CombinedMatrices = m_TPose;
}

void CAnimator3D::Set_LayerType(ANIM_LAYER_STATE eLayerType, _uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return;
	if (m_AnimLayers[LayerIndex].BaseLayer) return;

	m_AnimLayers[LayerIndex].eLayerType = eLayerType;
}

void CAnimator3D::Chagne_Speed(_float fSpeed, _uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return;

	m_AnimLayers[LayerIndex].fAnimSpeed = fSpeed;
}

void CAnimator3D::Change_TransitionSpeed(_float fTargetSpeed, _float fDuration, EaseType eEaseType, _uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return;

	m_AnimLayers[LayerIndex].fTargetSpeed = fTargetSpeed;
	m_AnimLayers[LayerIndex].fEaseDuration = fDuration;
	m_AnimLayers[LayerIndex].ePlayEaseType = eEaseType;
}

_quaternion CAnimator3D::Calc_TransformFromEndAnim(const _vector4& vTransformQuat)
{
	_quaternion endAnimQ = Get_RootBoneEndQuat();
	_quaternion transQ = vTransformQuat;

	endAnimQ.Normalize();
	transQ.Normalize();

	return endAnimQ * transQ;
}

void CAnimator3D::Control_Bone(const string& boneName, _fmatrix BoneMatrix)
{
	_int Index = m_pData->Find_BoneIndexByName(boneName);
	if (Index == -1) return;

	else {
		XMStoreFloat4x4(&m_ManipulateMatrices[Index], BoneMatrix);
	}
}

void CAnimator3D::Control_BoneByIndex(_uint Index, _fmatrix BoneMatrix)
{
	if (Index >= m_ManipulateMatrices.size()) return;
	else {
		XMStoreFloat4x4(&m_ManipulateMatrices[Index], BoneMatrix);
	}
}

void CAnimator3D::Dettach_BoneRelation(_uint Index)
{
	m_DettachedBone.insert(Index);
}

void CAnimator3D::Add_Event(CLIP_EVENT_TYPE EventType, string EventTag)
{
	m_EventBus.push_back(EVENT_INST{ EventType, EventTag });
}

void CAnimator3D::Clear_Events()
{
	m_EventBus.clear();
}

#pragma endregion

vector<_float4x4> CAnimator3D::Get_BoneMatrices(_uint meshIndex)
{
	vector<_float4x4> result;
	result.reserve(m_CombinedMatrices.size());

	for (size_t i = 0; i < m_CombinedMatrices.size(); ++i)
	{
		_smatrix final = m_CombinedMatrices[i];
		_smatrix offset = m_pData->Get_Offset(meshIndex, i);

		result.push_back(offset * final);
	}
	return result;
}

#pragma region GetTransformationBone

_float4x4 CAnimator3D::Get_BoneTransformationMatrix(AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (Index == -1)  return _float4x4{};
	else {
		return m_TransformationMatrices[Index];
	}
}

_float4x4* CAnimator3D::Get_BoneTransformationMatrixPtr(AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (Index == -1)  return nullptr;
	else {
		return &m_TransformationMatrices[Index];
	}
}

_vector3 CAnimator3D::Get_BoneTransformationPosition(AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (Index == -1)  return _vector3{};
	else {
		Matrix matrix = m_TransformationMatrices[Index];
		return _vector3(matrix._41, matrix._42, matrix._43);
	}
}

_vector4 CAnimator3D::Get_BoneTransformationQuaternion(AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (Index == -1)  return _quaternion::Identity;
	else {
		Matrix matrix = m_TransformationMatrices[Index];
		_vector3 S, T;
		_quaternion R;
		matrix.Decompose(S, R, T);
		return R;
	}
}

void CAnimator3D::Set_BoneTransformationMatrix(const _float4x4& Matrix, AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (Index == -1)  return _float4x4{};
	else {
		return m_TransformationMatrices[Index];
	}
}

void CAnimator3D::Get_BoneTransformationPosition(_vector3 Position, AnimArg BoneArg)
{
	return _vector3();
}

void CAnimator3D::Get_BoneTransformationQuaternion(_vector4 Quaternion, AnimArg BoneArg)
{
	return _vector4();
}

#pragma endregion

#pragma region GetCombinedBone
_float4x4 CAnimator3D::Get_BoneCombinedMatrix(AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (Index == -1)  return _float4x4{};
	else {
		return m_CombinedMatrices[Index];
	}
}

_float4x4* CAnimator3D::Get_BoneCombinedMatrixPtr(AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (Index == -1) return nullptr;
	else {
		return &m_CombinedMatrices[Index];
	}
}

_vector3 CAnimator3D::Get_BoneCombinedPosition(AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (Index == -1)  return _vector3{};
	else {
		Matrix matrix = m_CombinedMatrices[Index];
		return _vector3(matrix._41, matrix._42, matrix._43);
	}
}

_vector4 CAnimator3D::Get_BoneCombinedQuaternion(AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (Index == -1)  return _quaternion::Identity;
	else {
		Matrix matrix = m_CombinedMatrices[Index];
		_vector3 S, T;
		_quaternion R;
		matrix.Decompose(S, R, T);
		return R;
	}
}

#pragma endregion

_int CAnimator3D::Resolve_ClipIndex(AnimArg ClipArg)
{
	if (holds_alternative<_int>(ClipArg))
		return get<_int>(ClipArg);

	return Find_Clip(get<string>(ClipArg));
}

_int CAnimator3D::Resolve_BoneIndex(AnimArg BoneArg)
{
	if (holds_alternative<_int>(BoneArg))
		if (get<_int>(BoneArg) < m_pData->Get_BoneCount())
			return get<_int>(BoneArg);
		else
			return -1;

	return m_pData->Find_BoneIndexByName(get<string>(BoneArg));
}

_int CAnimator3D::Find_Clip(const string& ClipTag)
{
	_int iIndex = 0;
	for (auto& Clip : m_pAnimClips) {
		if (ClipTag == Clip->Get_Name())
			return iIndex;

		++iIndex;
	}

	return -1;
}

_bool CAnimator3D::isExistLayer(_int LayerIndex)
{
	return !m_AnimLayers.empty() && LayerIndex < m_AnimLayers.size();
}

_bool CAnimator3D::isExistClip(_int ClipIndex)
{
	return !m_pAnimClips.empty() && ClipIndex < m_pAnimClips.size();
}

_bool CAnimator3D::hasAxis(AXIS eExtractAxis, AXIS Axis)
{
	return (eExtractAxis & Axis) != AXIS::NONE;
}

Matrix CAnimator3D::Calc_MatrixBlend(const _float4x4& base, const _float4x4& target, _float weight)
{
	Matrix baseMat = base;
	Matrix targetMat = target;

	_vector3 baseS, targetS;
	_quaternion baseR, targetR;
	_vector3 baseT, targetT;

	baseMat.Decompose(baseS, baseR, baseT);
	targetMat.Decompose(targetS, targetR, targetT);
	_vector3	S = _vector3::Lerp(baseS, targetS, weight);
	_quaternion R = _quaternion::Slerp(baseR, targetR, weight);
	R.Normalize();
	_vector3	T = _vector3::Lerp(baseT, targetT, weight);

	return XMMatrixAffineTransformation(S, XMVectorZero(), R, T);
}

Matrix CAnimator3D::Calc_MatrixAdditive(const _float4x4& base, const _float4x4& target, const _float4x4& TPose, _float weight)
{
	_vector3 baseS, targetS, refS;
	_quaternion baseR, targetR, refR;
	_vector3 baseT, targetT, refT;

	Matrix(base).Decompose(baseS, baseR, baseT);
	Matrix(target).Decompose(targetS, targetR, targetT);
	Matrix(TPose).Decompose(refS, refR, refT);

	_vector3 deltaT = targetT - refT;
	refR.Inverse(refR);
	_quaternion deltaR = refR * targetR;
	_vector3 deltaS = targetS - refS;


	_vector3 outS = baseS;// +deltaS * weight;
	_quaternion outR =
		baseR * _quaternion::Slerp(_quaternion::Identity, deltaR, weight);
	outR.Normalize();
	_vector3 outT = baseT;// +deltaT * weight;

	return XMMatrixAffineTransformation(outS, XMVectorZero(), outR, outT);
}

/*-----------------*/
/*  Run Animation  */
/*-----------------*/
void CAnimator3D::Animation_Run(ANIM_LAYER& Layer, _float dt)
{
	//Empty Layer
	if (m_AnimLayers.empty()) return;

	//Empty Clip
	if (-1 == Layer.iClipIndex) return;

	//Update Animation
	auto& nowClip = m_pAnimClips[Layer.iClipIndex];

	//Calc Animation Speed
	_float AnimSpeed = Layer.fAnimSpeed;
	if (EaseType::None != Layer.ePlayEaseType) {
		Layer.fEaseElapsed += dt;

		_float t = min(Layer.fEaseElapsed / Layer.fEaseDuration, 1.f);
		_float Ease = Math::ApplyEase(Layer.ePlayEaseType, t);
		AnimSpeed = Math::Lerp(Layer.fAnimSpeed, Layer.fTargetSpeed, Ease);

		if (1.f <= t) {
			Layer.fAnimSpeed = AnimSpeed;
			Layer.ePlayEaseType = EaseType::None;
		}
	}

	_float playSpeed = dt * AnimSpeed;

	//Update TrackPos
	Layer.fCurrentTrackPosition = nowClip->TranslateAnimateMatrix(
		Layer.LocalMatrices, Layer.fCurrentTrackPosition,
		playSpeed, Layer.bLoop, &Layer.bWrapped, &Layer.bisFinished, m_EventBus);

	//Bone Extracter
	if (Layer.BaseLayer) {
		//Extract RootBone
		if (-1 != Layer.iRootBoneIndex && -1 != Layer.iMotionBoneIndex) {
			Matrix RootMat = Layer.LocalMatrices[Layer.iRootBoneIndex];
			Matrix MotionMat = Layer.LocalMatrices[Layer.iMotionBoneIndex];

			_vector S, R, T;
			XMMatrixDecompose(&S, &R, &T, RootMat);

			_vector3 vCurRootPos = T;
			_vector4 vCurRootQuat = R;

			if (Layer.bWrapped) { //Roop 
				_vector3 vStartPos = m_pAnimClips[Layer.iClipIndex]
					->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;

				Layer.vRootMoveDelta = (Layer.vRootEndPos - Layer.vPrevRootPos) + (vCurRootPos - vStartPos);
				Layer.bWrapped = false;
			}
			else {
				Layer.vRootMoveDelta = vCurRootPos - Layer.vPrevRootPos;
				//Extract Movebone
				// 이동 상쇄
				MotionMat.Translation(MotionMat.Translation() - vCurRootPos);

				// 회전 상쇄: 모션본에서 루트의 "현재 회전" 제거
				_vector invCurRootQuat = XMQuaternionInverse(vCurRootQuat);
				_matrix invCurRootRot = XMMatrixRotationQuaternion(invCurRootQuat);

				// 곱 순서는 엔진 규약에 따라 둘 중 하나가 맞음
				MotionMat = MotionMat * invCurRootRot;
				// 상쇄한 매트릭스 저장
				Layer.LocalMatrices[Layer.iMotionBoneIndex] = MotionMat;
			}

			_vector quatDeltaLocal =
				XMQuaternionNormalize(XMQuaternionMultiply(
					vCurRootQuat, XMQuaternionInverse(Layer.vPrevRootQuat)));

			// --- 위치(점) 변환: w = 1 ---
			_vector pos = XMVectorSet(Layer.vRootMoveDelta.x, Layer.vRootMoveDelta.y, Layer.vRootMoveDelta.z, 1.0f);
			_vector posT = XMVector4Transform(pos, m_PreTransform);

			Vector3 vPosOut;
			vPosOut.x = XMVectorGetX(posT);
			vPosOut.y = XMVectorGetY(posT);
			vPosOut.z = XMVectorGetZ(posT);
			Layer.vRootMoveDelta = vPosOut;

			// --- 회전(컨주게이션) ---
			_vector qCur = quatDeltaLocal;
			//프리트랜스폼 이동값 제거
			_matrix pRot = m_PreTransform;
			pRot.r[3] = XMVectorSet(0, 0, 0, 1);
			//프리트랜스폼 회전 매트릭스 생성
			_vector qP = XMQuaternionRotationMatrix(pRot);
			//실질적으로 사용할 수 있는 회전델타로 변환 
			_vector quatDeltaOut =
				XMQuaternionNormalize(XMQuaternionMultiply(
						qP,
						XMQuaternionMultiply(quatDeltaLocal, XMQuaternionInverse(qP))
					)
				);
			XMStoreFloat4(&Layer.vRootQuatDelta, quatDeltaOut);


			//다음 프레임 대비
			Layer.vPrevRootPos = vCurRootPos;
			Layer.vPrevRootQuat = vCurRootQuat;
		}

		//Extract MoveBone
		if (-1 != Layer.iMotionBoneIndex) {
			_float4x4& mat = Layer.LocalMatrices[Layer.iMotionBoneIndex];

			Layer.vPrevMotionBonePos = _vector3(mat._41, mat._42, mat._43);
			if (hasAxis(Layer.eExtractMoveAxis, AXIS::X)) mat._41 = 0.f;
			if (hasAxis(Layer.eExtractMoveAxis, AXIS::Y)) mat._42 = 0.f;
			if (hasAxis(Layer.eExtractMoveAxis, AXIS::Z)) mat._43 = 0.f;
		}
	}
}	
/*-----------------*/
/*Convert Animation*/
/*-----------------*/
void CAnimator3D::Animation_Convert(ANIM_LAYER& Layer, _float dt)
{
	if (-1 == Layer.iClipIndex) return;

	auto& nowClip = m_pAnimClips[Layer.iClipIndex];
	auto& nextClip = m_pAnimClips[Layer.iNextClipIndex];

	//Calc Animation Speed;
	_float AnimSpeed = Layer.fAnimSpeed;
	if (EaseType::None != Layer.ePlayEaseType) {
		Layer.fEaseElapsed += dt;

		_float t = min(Layer.fEaseElapsed / Layer.fEaseDuration, 1.f);
		_float Ease = Math::ApplyEase(Layer.ePlayEaseType, t);
		AnimSpeed = Math::Lerp(Layer.fAnimSpeed, Layer.fTargetSpeed, Ease);

		if (1.f <= t) {
			Layer.fAnimSpeed = AnimSpeed;
			Layer.ePlayEaseType = EaseType::None;
		}
	}

	_float playSpeed = dt * AnimSpeed;

	//Update TrackPos
	if (Layer.bUpdate_PrevClip) {
		Layer.fCurrentTrackPosition = nowClip->TranslateAnimateMatrix(
			Layer.LocalMatrices, Layer.fCurrentTrackPosition,
			playSpeed, Layer.bLoop, &Layer.bWrapped, &Layer.bisFinished, m_EventBus);
	}

	if (Layer.bUseFinalLocal)
		Layer.LocalMatrices = m_TransformationMatrices;

	if (Layer.bUpdate_NewClip) {
		Layer.fBlendTrackPosition = nextClip->TranslateAnimateMatrix(
			Layer.BlendMatrices, Layer.fBlendTrackPosition,
			playSpeed, Layer.bLoop, &Layer.bWrapped, &Layer.bisFinished, m_EventBus);
	}

	//Bone Extracter
	if (Layer.BaseLayer) {
		//Extract RootBone
		if (-1 != Layer.iRootBoneIndex && -1 != Layer.iMotionBoneIndex) {
			Matrix RootMat = Layer.BlendMatrices[Layer.iRootBoneIndex];
			Matrix MotionMat = Layer.BlendMatrices[Layer.iMotionBoneIndex];

			_vector S, R, T;
			XMMatrixDecompose(&S, &R, &T, RootMat);

			Vector3 vCurRootPos = T;
			Vector4 vCurRootQuat = R;

			if (Layer.bWrapped) { //Roop 
				_vector3 vStartPos = m_pAnimClips[Layer.iClipIndex]
					->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;

				Layer.vRootMoveDelta = (Layer.vRootEndPos - Layer.vPrevRootPos) + (vCurRootPos - vStartPos);
				Layer.bWrapped = false;
			}
			else {
				Layer.vRootMoveDelta = vCurRootPos - Layer.vPrevRootPos;
				// 이동 상쇄
				MotionMat.Translation(MotionMat.Translation() - vCurRootPos);

				// 회전 상쇄: 모션본에서 루트의 "현재 회전" 제거
				_vector invCurRootQuat = XMQuaternionInverse(vCurRootQuat);
				_matrix invCurRootRot = XMMatrixRotationQuaternion(invCurRootQuat);

				// 곱 순서는 엔진 규약에 따라 둘 중 하나가 맞음
				MotionMat = MotionMat * invCurRootRot;
				// 상쇄한 매트릭스 저장
				Layer.BlendMatrices[Layer.iMotionBoneIndex] = MotionMat;
			}


			_vector quatDeltaLocal =
				XMQuaternionNormalize(XMQuaternionMultiply(
					vCurRootQuat, XMQuaternionInverse(Layer.vPrevRootQuat)));

			// --- 위치(점) 변환: w = 1 ---
			_vector pos = XMVectorSet(Layer.vRootMoveDelta.x, Layer.vRootMoveDelta.y, Layer.vRootMoveDelta.z, 1.0f);
			_vector posT = XMVector4Transform(pos, m_PreTransform);

			Vector3 vPosOut;
			vPosOut.x = XMVectorGetX(posT);
			vPosOut.y = XMVectorGetY(posT);
			vPosOut.z = XMVectorGetZ(posT);
			Layer.vRootMoveDelta = vPosOut;

			// --- 회전(컨주게이션) ---
			_vector qCur = quatDeltaLocal;
			//프리트랜스폼 이동값 제거
			_matrix pRot = m_PreTransform;
			pRot.r[3] = XMVectorSet(0, 0, 0, 1);
			//프리트랜스폼 회전 매트릭스 생성
			_vector qP = XMQuaternionRotationMatrix(pRot);
			//실질적으로 사용할 수 있는 회전델타로 변환 
			_vector quatDeltaOut =
				XMQuaternionNormalize(XMQuaternionMultiply(
					qP,
					XMQuaternionMultiply(quatDeltaLocal, XMQuaternionInverse(qP))
				)
				);
			XMStoreFloat4(&Layer.vRootQuatDelta, quatDeltaOut);

			//다음 프레임 대비
			Layer.vPrevRootPos = vCurRootPos;
			Layer.vPrevRootQuat = vCurRootQuat;
		}

		//Extract MoveBone
		if (-1 != Layer.iMotionBoneIndex) {
			_float4x4& mat = Layer.BlendMatrices[Layer.iMotionBoneIndex];

			Layer.vPrevMotionBonePos = _float3(mat._41, mat._42, mat._43);
			if (hasAxis(Layer.eExtractMoveAxis, AXIS::X)) mat._41 = 0.f;
			if (hasAxis(Layer.eExtractMoveAxis, AXIS::Y)) mat._42 = 0.f;
			if (hasAxis(Layer.eExtractMoveAxis, AXIS::Z)) mat._43 = 0.f;
		}
	}

	//Animation Blend --- d여기 차이
	Layer.fBlendElapsed += dt;
	_float fBlendWeight = Math::ApplyEase(Layer.eBlendEaseType,
		Layer.bUseFinalLocal ?dt:Layer.fBlendElapsed / Layer.fBlendDuration);

	for (_uint i = 0; i < m_pData->Get_BoneCount(); ++i)
		Layer.FinalLocalMatrices[i] = Calc_MatrixBlend(Layer.LocalMatrices[i], Layer.BlendMatrices[i], fBlendWeight);
	
	if (Layer.bUseFinalLocal)
		m_TransformationMatrices = Layer.FinalLocalMatrices;

	//Convert End
	if (Layer.fBlendDuration < Layer.fBlendElapsed) {
		Layer.bBlending = false;
		Layer.bKeepTrackPos = false;
		Layer.bIgnoreRotation = false;

		Layer.iClipIndex = Layer.iNextClipIndex;
		Layer.iNextClipIndex = -1;
		Layer.fCurrentTrackPosition = Layer.fBlendTrackPosition;
		Layer.fBlendElapsed = 0.f;
		Layer.fBlendDuration = 0.f;

		Layer.LocalMatrices = Layer.FinalLocalMatrices;			
	}
}

void CAnimator3D::Layer_Base(const ANIM_LAYER& Layer)
{
	if (Layer.bBlending)
		m_TransformationMatrices = Layer.FinalLocalMatrices;
	else
		m_TransformationMatrices = Layer.LocalMatrices;
}

void CAnimator3D::Layer_Override(const ANIM_LAYER& Layer)
{
	if (-1 == Layer.iStartBoneIndex) {
		if (Layer.bBlending)
			m_TransformationMatrices = Layer.FinalLocalMatrices;
		else
			m_TransformationMatrices = Layer.LocalMatrices;
	}
	else {
		for (_int BoneIndex : Layer.AffectedBonesIndices) {
			if (BoneIndex == Layer.iMotionBoneIndex || BoneIndex == Layer.iRootBoneIndex) continue;

			if (Layer.bBlending)
				m_TransformationMatrices[BoneIndex] = Layer.FinalLocalMatrices[BoneIndex];
			else
				m_TransformationMatrices[BoneIndex] = Layer.LocalMatrices[BoneIndex];
		}
	}
}

void CAnimator3D::Layer_Blend(const ANIM_LAYER& Layer)
{
	if (-1 == Layer.iStartBoneIndex) {
		for (int i = 0; i < m_pData->Get_BoneCount(); i++) {
			if (i == Layer.iMotionBoneIndex || i == Layer.iRootBoneIndex) continue;

			if (Layer.bBlending)
				m_TransformationMatrices[i] = Calc_MatrixBlend(m_TransformationMatrices[i], Layer.FinalLocalMatrices[i], Layer.fLayerWeight);
			else
				m_TransformationMatrices[i] = Calc_MatrixBlend(m_TransformationMatrices[i], Layer.LocalMatrices[i], Layer.fLayerWeight);
		}
	}
	else {
		for (_int BoneIndex : Layer.AffectedBonesIndices) {
			if (BoneIndex == Layer.iMotionBoneIndex || BoneIndex == Layer.iRootBoneIndex) continue;

			if (Layer.bBlending)
				m_TransformationMatrices[BoneIndex] = Calc_MatrixBlend(m_TransformationMatrices[BoneIndex], Layer.FinalLocalMatrices[BoneIndex], Layer.fLayerWeight);
			else
				m_TransformationMatrices[BoneIndex] = Calc_MatrixBlend(m_TransformationMatrices[BoneIndex], Layer.LocalMatrices[BoneIndex], Layer.fLayerWeight);
		}
	}
}

void CAnimator3D::Layer_Additive(const ANIM_LAYER& Layer)
{
	if (-1 == Layer.iStartBoneIndex) {
		for (int i = 0; i < m_pData->Get_BoneCount(); i++) {
			if (i == Layer.iMotionBoneIndex || i == Layer.iRootBoneIndex) continue;
			
			if (Layer.bBlending)
				m_TransformationMatrices[i] = Calc_MatrixAdditive(m_TransformationMatrices[i], Layer.FinalLocalMatrices[i], m_BasePose[i], Layer.fLayerWeight);
			else
				m_TransformationMatrices[i] = Calc_MatrixAdditive(m_TransformationMatrices[i], Layer.LocalMatrices[i], m_BasePose[i], Layer.fLayerWeight);
		}
	}
	else {
		for (_int BoneIndex : Layer.AffectedBonesIndices) {
			if (BoneIndex == Layer.iMotionBoneIndex || BoneIndex == Layer.iRootBoneIndex) continue;

			if (Layer.bBlending)
				m_TransformationMatrices[BoneIndex] = Calc_MatrixAdditive(m_TransformationMatrices[BoneIndex], Layer.FinalLocalMatrices[BoneIndex], m_BasePose[BoneIndex], Layer.fLayerWeight);
			else
				m_TransformationMatrices[BoneIndex] = Calc_MatrixAdditive(m_TransformationMatrices[BoneIndex], Layer.LocalMatrices[BoneIndex], m_BasePose[BoneIndex], Layer.fLayerWeight);
		}
	}
}

void CAnimator3D::BuildBone(_float dt)
{
	for (auto& Layer : m_AnimLayers) {
		if (Layer.fLayerWeight <= 0) continue;

		if (EaseType::None != Layer.eLayerEaseType) {
			_float Ease = 0.f;
			Layer.fLayerWeightElapsed += dt;

			_float t = min(Layer.fLayerWeightElapsed / Layer.fLayerWeightDuration, 1.f);
			Ease = Math::ApplyEase(Layer.eLayerEaseType, t);
			Layer.fLayerWeight = Math::Lerp(Layer.fLayerWeight, Layer.fTargetLayerWeight, Ease);
		}
	
		switch (Layer.eLayerType)
		{
		case Engine::ANIM_LAYER_STATE::NONE:
			break;
		case Engine::ANIM_LAYER_STATE::BASE:
			Layer_Base(Layer);
			break;
		case Engine::ANIM_LAYER_STATE::OVERRIDE:
			Layer_Override(Layer);
			break;
		case Engine::ANIM_LAYER_STATE::BLEND:
			Layer_Blend(Layer);
			break;
		case Engine::ANIM_LAYER_STATE::ADDITIVE:
			Layer_Additive(Layer);
			break;
		default:
			break;
		}
	}

	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		int parent = m_pData->Get_BoneParentIndex(i);

		if (parent == -1) {
			_matrix MyTransformation =
				XMLoadFloat4x4(&m_ManipulateMatrices[i]) *
				XMLoadFloat4x4(&m_TransformationMatrices[i]) *
				XMLoadFloat4x4(&m_PreTransform);

			XMStoreFloat4x4(&m_CombinedMatrices[i], MyTransformation);
		}
		else if (m_DettachedBone.count(i)) {
			_matrix MyTransformation =
				XMLoadFloat4x4(&m_ManipulateMatrices[i]) *
				XMLoadFloat4x4(&m_TransformationMatrices[i]);

			XMStoreFloat4x4(&m_CombinedMatrices[i], MyTransformation);
		}
		else {
			_matrix ParentCombine = XMLoadFloat4x4(&m_CombinedMatrices[parent]);
			_matrix MyTransformation =
				XMLoadFloat4x4(&m_ManipulateMatrices[i])
				* XMLoadFloat4x4(&m_TransformationMatrices[i]);

			XMStoreFloat4x4(&m_CombinedMatrices[i], MyTransformation * ParentCombine);
		}
	}
}

#pragma region GUI
void CAnimator3D::Render_GUI()
{
	ImGui::SeparatorText("Animator 3D");
	GUI_ShowLayerInfo();
	GUI_SelectAnim();
}

void CAnimator3D::GUI_ShowLayerInfo()
{
	ImGui::BeginChild("##Animator Layer", ImVec2{ 0, 100.f }, true);
	// ───────── Layer / Loop
	ImGui::Text("Layer");
	ImGui::SameLine();

	int layerCount = m_AnimLayers.size();   // 현재 레이어 수

	static int curLayerIndex = 0;
	ImGui::SetNextItemWidth(80);
	if (ImGui::BeginCombo("##Layer", std::to_string(curLayerIndex).c_str()))
	{
		for (int i = 0; i < layerCount; ++i)
		{
			bool selected = (curLayerIndex == i);
			if (ImGui::Selectable(std::to_string(i).c_str(), selected))
				curLayerIndex = i;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	auto& curLayer = m_AnimLayers[curLayerIndex];

	ImGui::SameLine();
	static bool bLoop = false;
	ImGui::Checkbox("Loop", &curLayer.bLoop);

	ImGui::Separator();

	// ───────── Clip

	string AnimName{};

	if (isExistClip(curLayer.iClipIndex))
		AnimName = m_pAnimClips[curLayer.iClipIndex]->Get_Name();

	string AnimInfo = "Clip : " + to_string(curLayer.iClipIndex) + " | Name : " + AnimName;
	ImGui::Text(AnimInfo.c_str());
	ImGui::Separator();

	// ───────── Play bar
	if (ImGui::Button(curLayer.bPause ? "Play" : "Pause", ImVec2(60.f, 0.f))) {
		curLayer.bPause = !curLayer.bPause;
	}
	ImGui::SameLine();


	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

	int iDuration{};
	if (isExistClip(curLayer.iClipIndex))
		iDuration = m_pAnimClips[curLayer.iClipIndex]->Get_Duration();

	ImGui::SliderFloat("##PlayBar", &curLayer.fCurrentTrackPosition, 0.f, iDuration);
	ImGui::EndChild();
}

void CAnimator3D::GUI_SelectAnim()
{
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);
	ImVec2 padding = ImGui::GetStyle().FramePadding;

	ImGui::BeginChild("##Animator Animation", ImVec2{ 0, childHeight }, true);

	for (int i = 0; i < m_pAnimClips.size(); i++)
	{
		bool isSelected = (m_iCurrentClipIndex == i);
		string animName = m_pAnimClips[i]->Get_Name();

		ImGui::PushID((int)i);

		if (ImGui::Selectable(("##" + animName).c_str(), isSelected, 0, ImVec2{ 0, textLineHeight }))
		{
			Change_Animation(i)
				.Loop(true)
				.Apply();
		}

		ImVec2 itemMin = ImGui::GetItemRectMin();
		ImVec2 itemMax = ImGui::GetItemRectMax();

		ImVec2 textSize = ImGui::CalcTextSize(animName.c_str());
		ImVec2 textPos = ImVec2(itemMax.x - textSize.x - padding.x, itemMin.y + padding.y);

		// 텍스트 그리기
		ImGui::GetWindowDrawList()->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), animName.c_str());

		ImGui::PopID();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", animName.c_str());
		}

		if (isSelected) {
			ImGui::SetItemDefaultFocus();
		}
	}

	ImGui::EndChild();
}

#pragma endregion

void CAnimator3D::Reset_Anim()
{
	unordered_map<string, _uint> m_pAnimNames;
	m_pAnimNames.clear();
	for (auto& Clip : m_pAnimClips) {
		Safe_Release(Clip);
	}

	m_pAnimClips.clear();
	Safe_Release(m_pData);
}

CAnimator3D* CAnimator3D::Create()
{
	CAnimator3D* instance = new CAnimator3D();
	if (FAILED(instance->Initialize_Prototype())) {
		Safe_Release(instance);
	}
	return instance;
}

CComponent* CAnimator3D::Clone()
{
	return new CAnimator3D(*this);
}

void CAnimator3D::Free()
{
	__super::Free();
	Safe_Release(m_pData);
	for (auto& Clip : m_pAnimClips) {
		Safe_Release(Clip);
	}
	m_pAnimClips.clear();
	m_AnimLayers.clear();
}

#pragma region Builder

//----------  SetAnim Options
HRESULT SetAnimBuild::Apply()
{
	if (!m_pOwner || !m_pOwner->isExistLayer(m_iLayerIndex) || !m_pOwner->isExistClip(m_iClipIndex))
		return E_FAIL;
	
	//레이어, 클립 적용
	CAnimator3D::ANIM_LAYER& Layer = m_pOwner->m_AnimLayers[m_iLayerIndex];
	if (Layer.bApplied)
		return S_OK;
	
	Layer.iClipIndex = m_iClipIndex;

	//베이스 레이어일 경우 마지막 키프레임 위치, 회전을 갖고옴
	if (Layer.BaseLayer) {
		Layer.vPrevRootPos = m_pOwner->m_pAnimClips[m_iClipIndex]
			->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;
		Layer.vPrevRootQuat = m_pOwner->m_pAnimClips[m_iClipIndex]
			->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vRotation;
		Layer.vRootEndPos = m_pOwner->m_pAnimClips[m_iClipIndex]
			->Get_EndKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;
		Layer.vRootEndQuat = m_pOwner->m_pAnimClips[m_iClipIndex]
			->Get_EndKeyFrameByBoneIndex(Layer.iRootBoneIndex).vRotation;

		Layer.vMotionEndPos = m_pOwner->m_pAnimClips[m_iClipIndex]
			->Get_EndKeyFrameByBoneIndex(Layer.iMotionBoneIndex).vTranslation;
	}
	//베이스 레이어가 아닐경우 레이어블랜드의 값을 이용함
	else { 
		Layer.fLayerWeight = m_fLayerWeight;
		Layer.fTargetLayerWeight = m_fTargetWeight;
		Layer.fLayerWeightElapsed = 0.f;
		Layer.fLayerWeightDuration = m_fWeightDuration;
		Layer.eLayerEaseType = m_eLayerEaseType;

		if (Layer.eLayerType == ANIM_LAYER_STATE::ADDITIVE)
			m_pOwner->m_pAnimClips[m_iClipIndex]->TranslateAnimateMatrixFromDurationNoEvent(m_pOwner->m_BasePose, 0.f);
	}

	//애니매이션 기본
	Layer.bLoop = m_bLoop;
	Layer.fCurrentTrackPosition = 0.f;
	Layer.fAnimSpeed = m_fSpeed;
	Layer.bPause = m_bPause;

	//애니매이션 재생속도
	Layer.ePlayEaseType = m_ePlayEaseType;
	Layer.fTargetSpeed = m_fTargetSpeed;
	Layer.fEaseElapsed = 0.f;
	Layer.fEaseDuration = m_fEaseDuration;

	//애니매이션이 새로 시작됌
	Layer.bisFinished = false;
	Layer.bApplied = true;
	return S_OK;
}

//---------- ++ChangeAnim Options
HRESULT ChangeAnimBuild::Apply()
{
	if (!m_pOwner || !m_pOwner->isExistLayer(m_iLayerIndex) || !m_pOwner->isExistClip(m_iClipIndex))
		return E_FAIL;
	auto& Layer = m_pOwner->m_AnimLayers[m_iLayerIndex];

	if (Layer.bApplied)
		return S_OK;

	//베이스 레이어일 경우 마지막 키프레임 위치, 회전을 갖고옴
	if (Layer.BaseLayer) {
		Layer.vPrevRootPos = m_pOwner->m_pAnimClips[m_iClipIndex]
			->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;
		Layer.vPrevRootQuat = m_pOwner->m_pAnimClips[m_iClipIndex]
			->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vRotation;
		Layer.vRootEndPos = m_pOwner->m_pAnimClips[m_iClipIndex]
			->Get_EndKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;
		Layer.vRootEndQuat = m_pOwner->m_pAnimClips[m_iClipIndex]
			->Get_EndKeyFrameByBoneIndex(Layer.iRootBoneIndex).vRotation;

		Layer.vMotionEndPos = m_pOwner->m_pAnimClips[m_iClipIndex]
			->Get_EndKeyFrameByBoneIndex(Layer.iMotionBoneIndex).vTranslation;
	}
	//베이스 레이어가 아닐경우 레이어블랜드의 값을 이용함
	else {
		Layer.fLayerWeight = m_fLayerWeight;
		Layer.fTargetLayerWeight = m_fTargetWeight;
		Layer.fLayerWeightElapsed = 0.f;
		Layer.fLayerWeightDuration = m_fWeightDuration;
		Layer.eLayerEaseType = m_eLayerEaseType;
		if (Layer.eLayerType == ANIM_LAYER_STATE::ADDITIVE)
			m_pOwner->m_pAnimClips[m_iClipIndex]->TranslateAnimateMatrixFromDurationNoEvent(m_pOwner->m_BasePose, 0.f);
	}

	//애니매이션 기본
	Layer.bLoop = m_bLoop;
	Layer.fAnimSpeed = m_fSpeed;
	Layer.bPause = m_bPause;

	//애니매이션 재생속도
	Layer.ePlayEaseType = m_ePlayEaseType;
	Layer.fTargetSpeed = m_fTargetSpeed;
	Layer.fEaseElapsed = 0.f;
	Layer.fEaseDuration = m_fEaseDuration;

	//먄약 호출시 클립이 없으면 새로시작
	if (-1 == Layer.iClipIndex) {
		Layer.iClipIndex = m_iClipIndex;
		Layer.fCurrentTrackPosition = 0.f;
		//애니매이션이 새로 시작됌
		Layer.bisFinished = false;
		return S_OK;
	}
	else {
		//만약 블랜드 상태이면 바로 다음으로 블랜드될 수 있도록 얘내를 로컬로
		if (Layer.bBlending) {
			Layer.iClipIndex = Layer.iNextClipIndex;
			Layer.fCurrentTrackPosition = Layer.fBlendTrackPosition;
			Layer.LocalMatrices = Layer.BlendMatrices;
		}

		if (Layer.bKeepTrackPos) {
			Layer.fBlendTrackPosition = Layer.fCurrentTrackPosition;
		}
	}

	//클립끼리의 블랜드 상태
	Layer.bBlending = true;
	Layer.bKeepTrackPos = m_bKeepTrackPos;
	Layer.bUpdate_PrevClip = m_bUpdate_PrevClip;
	Layer.bUpdate_NewClip = m_bUpdate_NewClip;
	Layer.bIgnoreRotation = m_bIgnoreRotation;
	Layer.iNextClipIndex = m_iClipIndex;
	Layer.fBlendTrackPosition = 0.f;
	Layer.fBlendElapsed = 0.f;
	Layer.fBlendDuration = m_fBlendDuration;
	Layer.eBlendEaseType = m_eBlendEaseType;

	Layer.bUseFinalLocal = m_bUseFinalLocal;

	//클립을 업데이트 하지 않겠다면 다음 클립의 0초로 세팅
	if (false == m_bUpdate_NewClip)
		m_pOwner->m_pAnimClips[Layer.iNextClipIndex]->TranslateAnimateMatrixFromDurationNoEvent(Layer.BlendMatrices, 0);

	//애니매이션이 새로 시작됌
	Layer.bisFinished = false;
	Layer.bApplied = true;
	return S_OK;
}

#pragma endregion