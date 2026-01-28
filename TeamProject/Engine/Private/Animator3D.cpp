#include "Engine_Defines.h"
#include "Animator3D.h"
#include "AnimationClip.h"
#include "AnimBuilder.h"
#include "ModelData.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "DynamicBone.h"
#include "FootIK.h"


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

	m_BasePose.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_TransformationMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_ManipulateMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_CombinedMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	//m_FinalMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);

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

	BuildBone();
	m_TPose = m_CombinedMatrices;

	return S_OK;
}

HRESULT CAnimator3D::Link_DynamicBone()
{
	CDynamicBone* pDynamicBone = CDynamicBone::Create(this);
	if (nullptr == pDynamicBone)
		return E_FAIL;

	m_pDynamicBone = pDynamicBone;
	m_DynamicBoneMatrices.resize(m_pData->Get_BoneCount(), Matrix::Identity);

	//m_pDynamicBone->Link_ChainData(m_pData->Get_ChaingGroups());

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
	if (!m_pAnimClips.empty()) {

		/* Clear Clip Events */
		Clear_Events();

		/* Update Animation Clips*/
		Update_Layers(dt);

		/* Has Been Updated Even Once */
		if (m_bUpdatedClip) {

			/* Create TransformationMatrices */
			BuildLocal(dt);

			/* Create CombinedMatrices */
			BuildBone();
		}
	}

	/* Update IK Bone */
	//Update_IK(dt);

	/* Rebuild Combined */
	//BuildBone();
	
	/* If Linked DynamicBone */
	if (m_pDynamicBone) {

		/* Update DynamicBone */
 		m_pDynamicBone->Update(dt);

		/* Additive Combined */
		if(m_pDynamicBone->isInitUpdated())
			BuildDynamicBone();
		else
			m_FinalMatrices = m_CombinedMatrices;
	}
	else /* If Not Exist DynamicBone */
		m_FinalMatrices = m_CombinedMatrices;

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
			return Layer.vOutRootMoveDelta;

	return _float3();
}

_float4 CAnimator3D::Get_RootBoneQuatDelta() const
{
	for (auto& Layer : m_AnimLayers)
		if (Layer.BaseLayer)
			return Layer.vOutRootQuatDelta;

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

_int CAnimator3D::Get_AnimClipCount() const
{
	return (_int)m_pAnimClips.size();
}

string CAnimator3D::Get_AnimClipName(_uint clipIndex) const
{
	if (clipIndex >= m_pAnimClips.size()) return "";
	return m_pAnimClips[clipIndex]->Get_Name();
}

_float CAnimator3D::Get_TimeSec()
{
	if (m_AnimLayers.empty()) return 0.f;

	auto& layer = m_AnimLayers[0];
	return layer.bBlending ? layer.fBlendTrackPosition : layer.fCurrentTrackPosition;
}

_float CAnimator3D::Get_DurationSec()
{
	const _int cur = Get_CurAnimIndex(0);
	if (cur < 0) return 0.f;

	return m_pAnimClips[(size_t)cur]->Get_Duration();
}

void CAnimator3D::Set_TimeSec(_float timeSec)
{
	if (m_AnimLayers.empty()) return;

	const _int cur = Get_CurAnimIndex(0);
	if (cur < 0) return;

	const _float dur = m_pAnimClips[(size_t)cur]->Get_Duration();

	if (timeSec < 0.f) timeSec = 0.f;
	if (dur > 0.f && timeSec > dur) timeSec = dur;

	auto& layer = m_AnimLayers[0];

	layer.fCurrentTrackPosition = timeSec;
	layer.fBlendTrackPosition = timeSec;
	layer.fPrevTrackPosition = timeSec;

	const _bool wasPause = layer.bPause;

	const _vector3    prevRootPos = layer.vPrevRootPos;
	const _float4     prevRootQuat = layer.vPrevRootQuat;
	const _vector3    prevMovePos = layer.vPrevMotionBonePos;
	const _float3     prevMoveDelta = layer.vRootMoveDelta;
	const _float4     prevQuatDelta = layer.vRootQuatDelta;
	const _bool       prevWrapped = layer.bWrapped;

	layer.bPause = false;
	Update_Animation(0.f);
	layer.bPause = wasPause;

	layer.vPrevRootPos = prevRootPos;
	layer.vPrevRootQuat = prevRootQuat;
	layer.vPrevMotionBonePos = prevMovePos;
	layer.vRootMoveDelta = prevMoveDelta;
	layer.vRootQuatDelta = prevQuatDelta;
	layer.bWrapped = prevWrapped;

	Clear_Events();
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

void CAnimator3D::Change_Speed(_float fSpeed, _uint LayerIndex)
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

	m_AnimLayers[LayerIndex].fAnimSpeed = m_AnimLayers[LayerIndex].fAppliedAnimSpeed;
	m_AnimLayers[LayerIndex].isUpdateByTime = true;
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

_float4x4 CAnimator3D::Get_BoneMatrix(BoneSpace eBoneSpace, AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (-1 == Index) return _float4x4();

	switch (eBoneSpace)
	{
	case Engine::CAnimator3D::BoneSpace::TRANSFORMATION:	return m_TransformationMatrices[Index];
	case Engine::CAnimator3D::BoneSpace::MANIPULATE:		return m_ManipulateMatrices[Index];
	case Engine::CAnimator3D::BoneSpace::COMBINED:			return m_CombinedMatrices[Index];
	case Engine::CAnimator3D::BoneSpace::WORLD:				return m_CombinedMatrices[Index] * m_pOwner->Get_WorldMatrix();
	//case Engine::CAnimator3D::BoneSpace::FINAL:				return m_FinalMatrices[Index];
	default:												return _float4x4();
	}
}

_float4x4* CAnimator3D::Get_BoneMatrixPtr(BoneSpace eBoneSpace, AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (-1 == Index) return nullptr;

	switch (eBoneSpace)
	{
	case Engine::CAnimator3D::BoneSpace::TRANSFORMATION:	return &m_TransformationMatrices[Index];
	case Engine::CAnimator3D::BoneSpace::MANIPULATE:		return &m_ManipulateMatrices[Index];
	case Engine::CAnimator3D::BoneSpace::COMBINED:			return &m_CombinedMatrices[Index];
	//case Engine::CAnimator3D::BoneSpace::FINAL:				return &m_FinalMatrices[Index];
	default:												return nullptr;
	}
}

_vector3 CAnimator3D::Get_BonePosition(BoneSpace eBoneSpace, AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (-1 == Index) return _vector3();
	Matrix mat;

	switch (eBoneSpace)
	{
	case Engine::CAnimator3D::BoneSpace::TRANSFORMATION:	mat = m_TransformationMatrices[Index];	break;
	case Engine::CAnimator3D::BoneSpace::MANIPULATE:		mat = m_ManipulateMatrices[Index];		break;
	case Engine::CAnimator3D::BoneSpace::COMBINED:			mat = m_CombinedMatrices[Index];		break;
	case Engine::CAnimator3D::BoneSpace::WORLD:				mat = m_CombinedMatrices[Index] * m_pOwner->Get_WorldMatrix(); break;
	//case Engine::CAnimator3D::BoneSpace::FINAL:				mat = m_FinalMatrices[Index];			break;
	default:												return _vector3();
	}

	return _vector3(mat._41, mat._42, mat._43);
}

_quaternion CAnimator3D::Get_BoneQuaternion(BoneSpace eBoneSpace, AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (-1 == Index) return _quaternion::Identity;

	Matrix mat;
	switch (eBoneSpace)
	{
	case Engine::CAnimator3D::BoneSpace::TRANSFORMATION:	mat = m_TransformationMatrices[Index];	break;
	case Engine::CAnimator3D::BoneSpace::MANIPULATE:		mat = m_ManipulateMatrices[Index];		break;
	case Engine::CAnimator3D::BoneSpace::COMBINED:			mat = m_CombinedMatrices[Index];		break;
	case Engine::CAnimator3D::BoneSpace::WORLD:				mat = m_CombinedMatrices[Index] * m_pOwner->Get_WorldMatrix(); break;
	//case Engine::CAnimator3D::BoneSpace::FINAL:			mat = m_FinalMatrices[Index];			break;
	default:												return _quaternion::Identity;
	}

	_vector3 S, T;
	_quaternion R;
	mat.Decompose(S, R, T);

	return R;
}

const vector<_float4x4>& CAnimator3D::Get_BoneMatrices(BoneSpace eBoneSpace)
{
	switch (eBoneSpace)
	{
	case Engine::CAnimator3D::BoneSpace::TRANSFORMATION:	return m_TransformationMatrices;
	case Engine::CAnimator3D::BoneSpace::MANIPULATE:		return m_ManipulateMatrices;
	case Engine::CAnimator3D::BoneSpace::COMBINED:			return m_CombinedMatrices;
	//case Engine::CAnimator3D::BoneSpace::FINAL:				return m_FinalMatrices;
	default:												return vector<_float4x4>();
	}
}

vector<_float4x4>* CAnimator3D::Get_BoneMatrices_Ptr(BoneSpace eBoneSpace)
{
	switch (eBoneSpace)
	{
	case Engine::CAnimator3D::BoneSpace::TRANSFORMATION:	return &m_TransformationMatrices;
	case Engine::CAnimator3D::BoneSpace::MANIPULATE:		return &m_ManipulateMatrices;
	case Engine::CAnimator3D::BoneSpace::COMBINED:			return &m_CombinedMatrices;
	//case Engine::CAnimator3D::BoneSpace::FINAL:				return &m_FinalMatrices;
	default:												return nullptr;
	}
}

void CAnimator3D::Set_BoneMatrix(BoneSpace eBoneSpace, const _float4x4& Matrix, AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (-1 == Index) return;

	switch (eBoneSpace)
	{
	case Engine::CAnimator3D::BoneSpace::TRANSFORMATION:	m_TransformationMatrices[Index] = Matrix;	return;
	case Engine::CAnimator3D::BoneSpace::MANIPULATE:		m_ManipulateMatrices[Index] = Matrix;		return;
	case Engine::CAnimator3D::BoneSpace::COMBINED:			m_CombinedMatrices[Index] = Matrix;			return;
	//case Engine::CAnimator3D::BoneSpace::FINAL:				m_FinalMatrices[Index] = Matrix;			return;
	default:												return;
	}
}

void CAnimator3D::Set_BonePosition(BoneSpace eBoneSpace, _vector3 Position, AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (-1 == Index) return;

	switch (eBoneSpace)
	{
	case Engine::CAnimator3D::BoneSpace::TRANSFORMATION:
		m_TransformationMatrices[Index]._41 = Position.x;
		m_TransformationMatrices[Index]._42 = Position.y;
		m_TransformationMatrices[Index]._43 = Position.z;
		return;
	case Engine::CAnimator3D::BoneSpace::MANIPULATE:
		m_ManipulateMatrices[Index]._41 = Position.x;
		m_ManipulateMatrices[Index]._42 = Position.y;
		m_ManipulateMatrices[Index]._43 = Position.z;
		return;
	case Engine::CAnimator3D::BoneSpace::COMBINED:
		m_CombinedMatrices[Index]._41 = Position.x;
		m_CombinedMatrices[Index]._42 = Position.y;
		m_CombinedMatrices[Index]._43 = Position.z;
		return;
	//case Engine::CAnimator3D::BoneSpace::FINAL:
	//	m_FinalMatrices[Index]._41 = Position.x;
	//	m_FinalMatrices[Index]._42 = Position.y;
	//	m_FinalMatrices[Index]._43 = Position.z;
	//	return;
	default:												
		return;
	}
}

void CAnimator3D::Set_BoneQuaternion(BoneSpace eBoneSpace, _quaternion Quaternion, AnimArg BoneArg)
{
	_int Index = Resolve_BoneIndex(BoneArg);
	if (Index == -1)
		return;

	_float4x4* pMat = nullptr;

	switch (eBoneSpace)
	{
	case BoneSpace::TRANSFORMATION:	pMat = &m_TransformationMatrices[Index];	break;
	case BoneSpace::MANIPULATE:		pMat = &m_ManipulateMatrices[Index];		break;
	case BoneSpace::COMBINED:		pMat = &m_CombinedMatrices[Index];			break;
	//case BoneSpace::FINAL:			pMat = &m_FinalMatrices[Index];			break;
	default:						return;
	}

	if (!pMat)
		return;

	Matrix mat = *pMat;

	_vector3 S, T;
	_quaternion R;
	mat.Decompose(S, R, T);

	XMStoreFloat4x4(pMat, XMMatrixAffineTransformation(S, XMVectorZero(), Quaternion, T));
}

Matrix CAnimator3D::Get_OwnerWorldMatrix()
{
	return m_pOwner->Get_WorldMatrix();
}

void CAnimator3D::Reset_DynamicBoneMatrices()
{
	m_DynamicBoneMatrices.resize(m_pData->Get_BoneCount(), Matrix::Identity);
}

void CAnimator3D::Delete_DB()
{
	Safe_Release(m_pDynamicBone);
}

HRESULT CAnimator3D::Initialize_HumanoidRig()
{
	if (!m_pData) return E_FAIL;
	m_HumanoidRig.Reset();
	if (!m_pData->Get_RiggedData(m_HumanoidRig)) return E_FAIL;
	m_HumanoidRig.RebuildChainsFromMap();

	if (!m_HumanoidRig.IsRigComplete()) return E_FAIL;
	//OutputDebugStringA("HumanoidRig Setup Success\n");

	return S_OK;
}

HRESULT CAnimator3D::Initialize_FootIK(void* pFootIKDesc)
{
	if (!m_HumanoidRig.IsRigComplete()) return E_FAIL;
	if (!m_HumanoidRig.leftLeg.IsValid() || !m_HumanoidRig.rightLeg.IsValid()) return E_FAIL;
	if (!m_HumanoidRig.spine.HasPelvis()) return E_FAIL;

	CFootIK* pFootIK = CFootIK::Create(pFootIKDesc);
	if (!pFootIK) return E_FAIL;

	vector<_int> boneIndices = {
		m_HumanoidRig.leftLeg.upperIndex,
		m_HumanoidRig.leftLeg.lowerIndex,
		m_HumanoidRig.leftLeg.endIndex,
		m_HumanoidRig.rightLeg.upperIndex,
		m_HumanoidRig.rightLeg.lowerIndex,
		m_HumanoidRig.rightLeg.endIndex,
		m_HumanoidRig.spine.pelvisIndex
	};
	_vector3 vPoleVector = _vector3::Zero;

	if (FAILED(Add_IKChain(pFootIK, boneIndices, vPoleVector)))
	{
		Safe_Release(pFootIK);
		return E_FAIL;
	}

	OutputDebugStringA("FootIK Setup Success\n");
	return S_OK;
}

HRESULT CAnimator3D::Add_IKChain(IIKSolver* pSolver, const vector<_int>& BoneIndices, _vector3 vPoleVector)
{
	if (!pSolver) return E_FAIL;

	IK_CHAIN chain;
	chain.pSolver = pSolver;
	chain.BoneIndices = BoneIndices;
	chain.vPoleVector = vPoleVector;
	chain.fWeight = 1.f;
	chain.bEnabled = true;

	m_IKChains.push_back(chain);

	return S_OK;
}

void CAnimator3D::Set_IKChainEnabled(_uint iChainIndex, _bool bEnabled)
{
	if (iChainIndex >= m_IKChains.size()) return;
	m_IKChains[iChainIndex].bEnabled = bEnabled;
}

void CAnimator3D::Set_IKChainWeight(_uint iChainIndex, _float fWeight)
{
	if (iChainIndex >= m_IKChains.size()) return;
	m_IKChains[iChainIndex].fWeight = clamp(fWeight, 0.f, 1.f);
}

void CAnimator3D::Clear_IKChains()
{
	for (auto& chain : m_IKChains)
		Safe_Release(chain.pSolver);
	m_IKChains.clear();
}

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
	_quaternion R = _quaternion::Slerp(baseR, targetR, weight); R.Normalize();
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

	_float playSpeed = Compute_PlaySpeed(Layer, dt);

	//Update TrackPos
	Layer.fCurrentTrackPosition = nowClip->TranslateAnimateMatrix(
		Layer.LocalMatrices, Layer.fCurrentTrackPosition,
		playSpeed, Layer.bLoop, Layer.fEndAt, Layer.fStartAt,
		&Layer.bWrapped, &Layer.bJumpedAnim,
		&Layer.bisFinished,
		&Layer.fProgress,
		m_EventBus);

	//Reserved Animation Speed
	Check_ReservedSpeeds(Layer);

	//Bone Extracter
	if (Layer.BaseLayer) {
		//Extract RootBone
		if (-1 != Layer.iRootBoneIndex) {
			Matrix RootMat = Layer.LocalMatrices[Layer.iRootBoneIndex];

			_vector S, R, T;
			XMMatrixDecompose(&S, &R, &T, RootMat);

			_vector3 vCurRootPos = T;
			_vector4 vCurRootQuat = R;

			if (Layer.bJumpedAnim)
			{
				//Layer.vRootMoveDelta = _vector3::Zero;
				//Layer.vRootQuatDelta = _quaternion::Identity;
				Layer.bJumpedAnim = false;
				//Layer.bWrapped = false;
			}
			else {
				Compute_RootMoveDelta(Layer, vCurRootPos);
				Compute_RootQuatDelta(Layer, vCurRootQuat);
			}

			//다음 프레임 대비
			Layer.vPrevRootPos = vCurRootPos;
			Layer.vPrevRootQuat = vCurRootQuat;

			//Extract Movebone
			Extract_MotionBone(Layer);
		}

		//Extract MoveBone
		//if (-1 != Layer.iMotionBoneIndex) {
		//	_float4x4& mat = Layer.LocalMatrices[Layer.iMotionBoneIndex];
		//
		//	Layer.vPrevMotionBonePos = _vector3(mat._41, mat._42, mat._43);
		//	if (hasAxis(Layer.eExtractMoveAxis, AXIS::X)) mat._41 = 0.f;
		//	if (hasAxis(Layer.eExtractMoveAxis, AXIS::Y)) mat._42 = 0.f;
		//	if (hasAxis(Layer.eExtractMoveAxis, AXIS::Z)) mat._43 = 0.f;
		//}
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

	_float playSpeed = Compute_PlaySpeed(Layer, dt);

	//Update TrackPos
	if (Layer.bUpdate_PrevClip) {
		Layer.fCurrentTrackPosition = nowClip->TranslateAnimateMatrix(
			Layer.LocalMatrices, Layer.fCurrentTrackPosition,
			playSpeed, Layer.bLoop, Layer.fEndAt, Layer.fStartAt,
			&Layer.bWrapped, &Layer.bJumpedAnim,
			&Layer.bisFinished,
			nullptr,
			m_EventBus);
	}

	if (Layer.bUpdate_NewClip) {
		Layer.fBlendTrackPosition = nextClip->TranslateAnimateMatrix(
			Layer.BlendMatrices, Layer.fBlendTrackPosition,
			playSpeed, Layer.bLoop, Layer.fEndAt, Layer.fStartAt,
			&Layer.bWrapped, &Layer.bJumpedAnim,
			&Layer.bisFinished,
			&Layer.fProgress,
			m_EventBus);
	}

	//Reserved Speed
	Check_ReservedSpeeds(Layer);

	//Bone Extracter
	if (Layer.BaseLayer) {
		//Extract RootBone
		if (-1 != Layer.iRootBoneIndex) {
			Matrix RootMat = Layer.BlendMatrices[Layer.iRootBoneIndex];

			_vector S, R, T;
			XMMatrixDecompose(&S, &R, &T, RootMat);

			Vector3 vCurRootPos = T;
			Vector4 vCurRootQuat = R;

			if (Layer.bJumpedAnim)
			{
				//Layer.vRootMoveDelta = _vector3::Zero;
				//Layer.vRootQuatDelta = _quaternion::Identity;
				Layer.bJumpedAnim = false;
				//Layer.bWrapped = false;
			}
			else {
				Compute_RootMoveDelta(Layer, vCurRootPos);
				Compute_RootQuatDelta(Layer, vCurRootQuat);
			}

			//다음 프레임 대비
			Layer.vPrevRootPos = vCurRootPos;
			Layer.vPrevRootQuat = vCurRootQuat;

			Extract_MotionBone(Layer);
		}

		//Extract MoveBone
		//if (-1 != Layer.iMotionBoneIndex) {
		//	_float4x4& mat = Layer.BlendMatrices[Layer.iMotionBoneIndex];
		//
		//	Layer.vPrevMotionBonePos = _float3(mat._41, mat._42, mat._43);
		//	if (hasAxis(Layer.eExtractMoveAxis, AXIS::X)) mat._41 = 0.f;
		//	if (hasAxis(Layer.eExtractMoveAxis, AXIS::Y)) mat._42 = 0.f;
		//	if (hasAxis(Layer.eExtractMoveAxis, AXIS::Z)) mat._43 = 0.f;
		//}
	}

	//Animation Blend
	Compute_ClipConvert(Layer, dt);
}

_float CAnimator3D::Compute_PlaySpeed(ANIM_LAYER& Layer, _float dt)
{
	Layer.fAppliedAnimSpeed = Layer.fAnimSpeed;

	//Calc Animation Speed
	if (EaseType::None != Layer.ePlayEaseType) {
		if (Layer.isUpdateByTime) {
			//Change speed by time
			Layer.fEaseElapsed += dt;

			_float t = min(Layer.fEaseElapsed / Layer.fEaseDuration, 1.f);
			_float Ease = Math::ApplyEase(Layer.ePlayEaseType, t);
			Layer.fAppliedAnimSpeed = Math::Lerp(Layer.fAnimSpeed, Layer.fTargetSpeed, Ease);

			if (1.f <= t) {
				Layer.fAnimSpeed = Layer.fAppliedAnimSpeed;
				Layer.ePlayEaseType = EaseType::None;
			}
		}
		else {
			//Change speed by Progress
			_float percent = (Layer.fProgress - Layer.fStartProgress) / (Layer.fEndProgress - Layer.fStartProgress);
			_float t = min(percent, 1.f);
			_float Ease = Math::ApplyEase(Layer.ePlayEaseType, t);
			Layer.fAppliedAnimSpeed = Math::Lerp(Layer.fAnimSpeed, Layer.fTargetSpeed, Ease);

			if (1.f <= t) {
				Layer.fAnimSpeed = Layer.fAppliedAnimSpeed;
				Layer.ePlayEaseType = EaseType::None;
				Layer.isUpdateByTime = false;
			}
		}
	}

	return dt * Layer.fAppliedAnimSpeed;
}

void CAnimator3D::Check_ReservedSpeeds(ANIM_LAYER& Layer)
{
	if (!Layer.ReservedSpeeds.empty() && Layer.fProgress > Layer.ReservedSpeeds.front().Start) {
		auto Reserve = Layer.ReservedSpeeds.front();
		Layer.fStartProgress = Reserve.Start;
		Layer.fEndProgress = Reserve.End;
		Layer.fTargetSpeed = Reserve.TargetSpeed;
		Layer.ePlayEaseType = Reserve.Ease;

		Layer.fAnimSpeed = Layer.fAppliedAnimSpeed;

		Layer.ReservedSpeeds.pop();
		Layer.isUpdateByTime = true;
	}
}

void CAnimator3D::Compute_RootMoveDelta(ANIM_LAYER& Layer, _vector3& curPos)
{
	if (Layer.bWrapped) { //Roop 
		_vector3 vStartPos = m_pAnimClips[Layer.iClipIndex]->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;
		Layer.vRootMoveDelta = (Layer.vRootEndPos - Layer.vPrevRootPos) + (curPos - vStartPos);
		Layer.bWrapped = false;
	}
	else
		Layer.vRootMoveDelta = curPos - Layer.vPrevRootPos;

	//PreTransform
	Layer.vOutRootMoveDelta = XMVector4Transform(Layer.vRootMoveDelta, m_PreTransform);
}

void CAnimator3D::Compute_RootQuatDelta(ANIM_LAYER& Layer, _vector4& curQuat)
{
	_quaternion quatDeltaLocal = XMQuaternionNormalize(XMQuaternionMultiply(curQuat, XMQuaternionInverse(Layer.vPrevRootQuat)));

	//프리트랜스폼 이동값 제거
	Matrix pRot = m_PreTransform;
	pRot.Translation(_vector3::Zero);

	//프리트랜스폼 회전 매트릭스 생성
	_quaternion qP = _quaternion::CreateFromRotationMatrix(pRot);

	//실질적으로 사용할 수 있는 회전델타로 변환 
	_vector quatDeltaOut =
		XMQuaternionNormalize(XMQuaternionMultiply(
			qP,
			XMQuaternionMultiply(quatDeltaLocal, XMQuaternionInverse(qP))
		)
		);

	//PreTransform
	XMStoreFloat4(&Layer.vOutRootQuatDelta, quatDeltaOut);
}

void CAnimator3D::Compute_ClipConvert(ANIM_LAYER& Layer, _float dt)
{
	Layer.fBlendElapsed += dt;
	_float fBlendWeight = Math::ApplyEase(Layer.eBlendEaseType, Layer.fBlendElapsed / Layer.fBlendDuration);

	for (_uint i = 0; i < m_pData->Get_BoneCount(); ++i)
		Layer.FinalLocalMatrices[i] = Calc_MatrixBlend(Layer.LocalMatrices[i], Layer.BlendMatrices[i], fBlendWeight);

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

void CAnimator3D::Extract_MotionBone(ANIM_LAYER& Layer)
{
	if (-1 != Layer.iMotionBoneIndex) {
		Matrix MotionMat = Layer.LocalMatrices[Layer.iMotionBoneIndex];
		MotionMat.Translation(MotionMat.Translation() - Layer.vPrevRootPos);

		// 회전 상쇄: 모션본에서 루트의 "현재 회전" 제거
		_vector invCurRootQuat = XMQuaternionInverse(Layer.vPrevRootQuat);
		_matrix invCurRootRot = XMMatrixRotationQuaternion(invCurRootQuat);

		// 곱 순서는 엔진 규약에 따라 둘 중 하나가 맞음
		MotionMat = MotionMat * invCurRootRot;
		// 상쇄한 매트릭스 저장
		Layer.LocalMatrices[Layer.iMotionBoneIndex] = MotionMat;
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

void CAnimator3D::Update_Layers(_float dt)
{
	m_bUpdatedClip = false;

	for (auto& Layer : m_AnimLayers) {
		if (Layer.bPause) continue;
		if (Layer.fLayerWeight <= 0) continue;
		if (-1 == Layer.iClipIndex) continue;
	
		if (Layer.bBlending)
			Animation_Convert(Layer, dt);
		else
			Animation_Run(Layer, dt);
		
		m_bUpdatedClip = true;
	}
}

void CAnimator3D::BuildLocal(_float dt)
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
}

void CAnimator3D::BuildIKMatrices(_float dt)
{
}


void CAnimator3D::BuildBone()
{
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

void CAnimator3D::BuildDynamicBone()
{
	for (size_t i = 0; i < m_pData->Get_BoneCount(); ++i)
	{
		int parent = m_pData->Get_BoneParentIndex(i);

		Matrix local = XMLoadFloat4x4(&m_TransformationMatrices[i]);

		Matrix animLocal = XMLoadFloat4x4(&m_TransformationMatrices[i]);
		Matrix manipulate = XMLoadFloat4x4(&m_ManipulateMatrices[i]);
		Matrix dynamic = XMLoadFloat4x4(&m_DynamicBoneMatrices[i]);

		Matrix localFinal = manipulate * animLocal * dynamic;

		if (parent == -1)
		{
			XMStoreFloat4x4(
				&m_FinalMatrices[i],
				localFinal * m_PreTransform
			);
		}
		else
		{
			Matrix parentCombined = XMLoadFloat4x4(&m_FinalMatrices[parent]);

			XMStoreFloat4x4(
				&m_FinalMatrices[i],
				localFinal * parentCombined
			);
		}
	}
}

#pragma region GUI
void CAnimator3D::Render_GUI()
{
	ImGui::SeparatorText("Animator 3D");
	GUI_ShowLayerInfo();
	GUI_SelectAnim();
	if (!m_IKChains.empty())
	{
		ImGui::Separator();
		ImGui::TextColored(ImVec4(0, 1, 1, 1), "IK Chains");

		for (size_t i = 0; i < m_IKChains.size(); ++i)
		{
			auto& chain = m_IKChains[i];

			ImGui::PushID(i);

			string label = "Chain " + to_string(i);
			if (ImGui::TreeNode(label.c_str()))
			{
				ImGui::Checkbox("Enabled", &chain.bEnabled);
				ImGui::DragFloat("Weight", &chain.fWeight, 0.01f, 0.f, 1.f);

				ImGui::Text("Bone Indices:");
				for (auto idx : chain.BoneIndices)
				{
					string boneName = m_pData->Find_BoneNameByIndex(idx);
					ImGui::Text("  [%d] %s", idx, boneName.c_str());
				}

				// Solver GUI
				if (chain.pSolver)
				{
					dynamic_cast<CFootIK*>(chain.pSolver)->Render_GUI();
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}

	if (m_pDynamicBone)
		m_pDynamicBone->Render_GUI();
}

void CAnimator3D::GUI_ShowLayerInfo()
{
	if (m_AnimLayers.empty())
		return;

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

	_float fDuration{};
	if (isExistClip(curLayer.iClipIndex))
		fDuration = m_pAnimClips[Get_CurAnimIndex()]->Get_Duration();
	
	_float fTrackPos;
	(curLayer.bBlending)
		? fTrackPos = curLayer.fBlendTrackPosition
		: fTrackPos = curLayer.fCurrentTrackPosition;

	ImGui::SliderFloat("##PlayBar", &fTrackPos, 0.f, fDuration);

	ImGui::Text("Speed ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(40.f);
	ImGui::DragFloat("##Speed", &curLayer.fAnimSpeed, 0.01f, 0.f, 10.f, "%.2f");
	ImGui::SameLine();
	ImGui::Text("Progress : %.2f", curLayer.fProgress);

	ImGui::Text("PrevPos : X:%.2f Y:%.2f Z:%.2f", curLayer.vPrevRootPos.x,
		curLayer.vPrevRootPos.y,
		curLayer.vPrevRootPos.z);

	ImGui::EndChild();
}

void CAnimator3D::GUI_SelectAnim()
{
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 15) + (ImGui::GetStyle().WindowPadding.y * 2);
	ImVec2 padding = ImGui::GetStyle().FramePadding;
	ImGui::Text("Search");
	ImGui::InputTextWithHint("##AnimSearch", "Search...", &m_animFilter);
	ImGui::BeginChild("##Animator Animation", ImVec2{ 0, childHeight }, true);
	for (int index = 0; index < (int)m_pAnimClips.size(); ++index)
	{
		const string& animName = m_pAnimClips[index]->Get_Name();

		if (!m_animFilter.empty())
		{
			if (!Helper::ContainsCaseInsensitive(animName, m_animFilter))
				continue;
		}

		bool isSelected = (m_iCurrentClipIndex == index);
		ImGui::PushID(index);

		if (ImGui::Selectable(("##" + animName).c_str(), isSelected, 0, ImVec2{ 0, textLineHeight }))
		{
			Change_Animation(index).Loop(true).Apply();
		}

		ImVec2 itemMin = ImGui::GetItemRectMin();
		ImVec2 itemMax = ImGui::GetItemRectMax();
		ImVec2 textSize = ImGui::CalcTextSize(animName.c_str());
		ImVec2 textPos = ImVec2(itemMax.x - textSize.x - padding.x, itemMin.y + padding.y);
		ImGui::GetWindowDrawList()->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), animName.c_str());

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", animName.c_str());

		if (isSelected)
			ImGui::SetItemDefaultFocus();

		ImGui::PopID();
	}

	ImGui::EndChild();
}

#pragma endregion

#pragma region IKBone
void CAnimator3D::Update_IK(_float dt)
{
	for (auto& chain : m_IKChains)
	{
		if (!chain.bEnabled || chain.fWeight <= 0.f || !chain.pSolver)
			continue;

		IK_CONTEXT context;
		context.pAnimator = this;
		context.BoneIndices = chain.BoneIndices;
		context.vPoleVector = chain.vPoleVector;
		context.fWeight = chain.fWeight;

		chain.pSolver->Solve(context);

		if (!context.bSuccess)
			continue;

		Apply_IK(context);
	}
}

void CAnimator3D::Apply_IK(IK_CONTEXT& context)
{
	for (size_t i = 0; i < context.BoneIndices.size(); ++i)
	{
		_int iBone = context.BoneIndices[i];
		if (iBone < 0 || iBone >= m_TransformationMatrices.size())
			continue;

		_smatrix matCurrent = XMLoadFloat4x4(&m_TransformationMatrices[iBone]);
		_vector3 S, T;
		_quaternion R;
		matCurrent.Decompose(S, R, T);

		// 회전 적용
		if (i < context.OutRotations.size())
		{
			_quaternion R_IK = context.OutRotations[i];

			// Identity가 아닌 경우만 적용
			_float fDiff = abs(R_IK.x) + abs(R_IK.y) + abs(R_IK.z) + abs(R_IK.w - 1.f);

			if (fDiff > 0.01f)
			{
				R_IK.Normalize();

				// IK 회전으로 완전 대체 (Slerp)
				R = _quaternion::Slerp(R, R_IK, context.fWeight);
				R.Normalize();
			}
		}

		// 위치 적용
		if (i < context.OutPositions.size())
		{
			_vector3 T_Offset = context.OutPositions[i];
			if (T_Offset.LengthSquared() > 0.0001f)
			{
				T += T_Offset * context.fWeight;
			}
		}

		_smatrix matNew = XMMatrixAffineTransformation(S, XMVectorZero(), R, T);
		XMStoreFloat4x4(&m_TransformationMatrices[iBone], matNew);
	}
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
	Clear_IKChains();
	Safe_Release(m_pData);
	Safe_Release(m_pDynamicBone);
	for (auto& Clip : m_pAnimClips) {
		Safe_Release(Clip);
	}
	m_pAnimClips.clear();
	m_AnimLayers.clear();
}
