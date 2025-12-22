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
	m_CombinedMatrices{ rhs.m_CombinedMatrices },
	m_FinalMatices{ rhs.m_FinalMatices }
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
	m_CombinedMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_FinalMatices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_ManipulateMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);

	/*뼈 개수만큼 뼈의 로컬상태를 가져옴*/
	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		m_TransformationMatrices[i] = m_pData->Get_TransformMatrix(i);
	}
	/*부모 뼈를 받을 수 있게 기본값으로 초기화*/
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
	/*최종 뼈 행렬에 대입*/
	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		XMStoreFloat4x4(&m_FinalMatices[i], m_pData->Get_OffsetMatrix(i) * XMLoadFloat4x4(&m_CombinedMatrices[i]));
	}
}

HRESULT CAnimator3D::Link_MetaData(const string& LevelKey, const string& MetaClipKey)
{
	m_pAnimClips = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_MetaClip(LevelKey, MetaClipKey);

	if (m_pAnimClips.empty()) {
		string msg = "Anim Add Failed: " + MetaClipKey + "\n";
		OutputDebugStringA(msg.c_str());
		return E_FAIL;
	}

	for(auto& Clip : m_pAnimClips)
		Safe_AddRef(Clip);

	Resize_Layer(1);

	return S_OK;
}

HRESULT CAnimator3D::Resize_Layer(_uint iLayerCount)
{
	if(isExistLayer(iLayerCount))
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

	for (auto& Layer : m_AnimLayers) {
		Animation_Run(dt);
	}

	BuildBone();
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

	Layer.iClipIndex = -1;
	Layer.fPrevTrackPosition = 0.f;
	Layer.fCurrentTrackPosition = 0.f;
	Layer.bLoop = false;
	Layer.bisFinished = true;

	Layer.bBlending = { false };
	Layer.iNextClipIndex = { -1 };
	Layer.fBlendElapsed = 0.f;
	Layer.fBlendDuration = 0.f;

	Layer.eBlendState = { BLEND_STATE::NONE };
	Layer.AffectedBonesIndices.clear();

	Matrix identityMat = XMMatrixIdentity();

	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());
	Layer.LocalMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	Layer.BlendMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	Layer.FinalLocalMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
}

HRESULT CAnimator3D::Stop_Animation(_uint LayerIndex)
{
	return E_NOTIMPL;
}

HRESULT CAnimator3D::StopAll_Animation()
{
	return E_NOTIMPL;
}

_bool CAnimator3D::isCurrentAnimEnd(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex))
		return true;
	if (m_iCurrentClipIndex == -1)
		return true;

	ANIM_LAYER& Layer = m_AnimLayers[LayerIndex];

	if (Layer.eBlendState != BLEND_STATE::NONE)
		return false;
	else if (Layer.bLoop)
		return false;
	else
		return Layer.bisFinished;
}

_bool CAnimator3D::isOverClipTiming(_float percent, _uint LayerIndex)
{
	if (!isExistLayer(LayerIndex))
		return true;

	ANIM_LAYER& Layer = m_AnimLayers[LayerIndex];

	if (!isExistClip(Layer.iClipIndex))
		return 0.f;

	auto& nowClip = m_pAnimClips[Layer.iClipIndex];

	_float threshold = nowClip->Get_Duration() * percent;

	// 이전 프레임 트랙 위치 < 기준 <= 현재 트랙 위치일 때 true
	return Layer.fCurrentTrackPosition >= threshold;
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
	
	if (!isExistClip(Layer.iClipIndex))
		return 0.f;

	auto& nowClip = m_pAnimClips[Layer.iClipIndex];

	return Layer.fCurrentTrackPosition / nowClip->Get_Duration();
}

string CAnimator3D::Get_CurAnimName(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return "";
	
	ANIM_LAYER& Layer = m_AnimLayers[LayerIndex];

	if (Layer.bBlending)
		if(isExistClip(Layer.iNextClipIndex))
			return m_pAnimClips[Layer.iNextClipIndex]->Get_Name();
	else
		if (isExistClip(Layer.iClipIndex))
			return m_pAnimClips[Layer.iClipIndex]->Get_Name();

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

void CAnimator3D::Set_NoTransform(_int MoveBoneIndex, _uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return;

	m_AnimLayers[LayerIndex].bUseTransform = false;
	m_AnimLayers[LayerIndex].iMoveBoneIndex = MoveBoneIndex;
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

_float4x4 CAnimator3D::Get_BoneMatrix(const string& boneName)
{
	_int Index = m_pData->Find_BoneIndexByName(boneName);
	if (Index == -1)  return _float4x4{};
	else {
		return m_FinalMatices[Index];
	}
}

_float4x4 CAnimator3D::Get_BoneMatrix(_uint Index)
{
	if (Index >= m_ManipulateMatrices.size()) return _float4x4{};
	else {
		return m_FinalMatices[Index];
	}
}

_float4x4* CAnimator3D::Get_BoneMatrixPtr(const string& boneName)
{
	_int Index = m_pData->Find_BoneIndexByName(boneName);
	if (Index == -1)  return nullptr;
	else {
		return &m_FinalMatices[Index];
	}
}

_float4x4* CAnimator3D::Get_BoneTransformMatrixPtr(const string& boneName)
{
	_int Index = m_pData->Find_BoneIndexByName(boneName);
	if (Index == -1)  return nullptr;
	else {
		return &m_TransformationMatrices[Index];
	}
}

_int CAnimator3D::Resolve_ClipIndex(AnimArg ClipArg)
{
	if (holds_alternative<_int>(ClipArg))
		return get<_int>(ClipArg);

	return Find_Clip(get<string>(ClipArg));
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

void CAnimator3D::Animation_Run(_float dt)
{
	if (m_AnimLayers.empty()) return;

	//애니매이션 업데이트
	for (auto& Layer : m_AnimLayers) {
		if (-1 == Layer.iClipIndex) continue;

		auto& nowClip = m_pAnimClips[Layer.iClipIndex];

		Layer.fCurrentTrackPosition = nowClip->TranslateAnimateMatrix(
			Layer.LocalMatrices, Layer.fCurrentTrackPosition,
			(dt*Layer.fAnimSpeed) , Layer.bLoop, &Layer.bisFinished);
	}

	//이동값 제거
	for (auto& Layer : m_AnimLayers) {
		if (false == Layer.bUseTransform) {
			if (isExistClip(Layer.iMoveBoneIndex)) {
				_float4x4& mat = Layer.LocalMatrices[Layer.iMoveBoneIndex];
				Layer.fPrevAnimPos = _float3(mat._41, mat._42, mat._43);
				mat._41 = mat._42 = mat._43 = 0;
			}
		}
	}
}

void CAnimator3D::Animation_Convert(_float dt)
{
	for (auto& Layer : m_AnimLayers) {
		if (-1 == Layer.iClipIndex) continue;

		auto& nowClip = m_pAnimClips[Layer.iClipIndex];

		Layer.fCurrentTrackPosition = nowClip->TranslateAnimateMatrix(
			Layer.LocalMatrices, Layer.fCurrentTrackPosition,
			dt, Layer.bLoop, &Layer.bisFinished);
	}
}

void CAnimator3D::Override_BlendAnim()
{
	// base와 blend 보간
	for (size_t i = 0; i < m_BlendIndex.size(); ++i)
	{
		_uint idx = m_BlendIndex[i];
		_matrix base = XMLoadFloat4x4(&m_TransformationMatrices[idx]);
		_matrix blend = XMLoadFloat4x4(&m_BlendTransfomationMatices[idx]);
		_vector baseS, baseR, baseT;
		_vector blendS, blendR, blendT;

		XMMatrixDecompose(&baseS, &baseR, &baseT, base);
		XMMatrixDecompose(&blendS, &blendR, &blendT, blend);

		_vector blendedS = XMVectorLerp(baseS, blendS, m_fBlendDuration);
		_vector blendedT = XMVectorLerp(baseT, blendT, m_fBlendDuration);
		_vector blendedR = XMQuaternionSlerp(baseR, blendR, m_fBlendDuration);

		_matrix blendedM = XMMatrixAffineTransformation(
			blendedS, XMVectorSet(0.f, 0.f, 0.f, 1.f), blendedR, blendedT);

		XMStoreFloat4x4(&m_TransformationMatrices[idx], blendedM);
	}
}

void CAnimator3D::Layer_Override(const ANIM_LAYER& Layer)
{
	if (-1 == Layer.iStartBoneIndex) {
		m_TransformationMatrices = Layer.LocalMatrices;
	}
	else {
		for (_int BoneIndex : Layer.AffectedBonesIndices)
			m_TransformationMatrices[BoneIndex] = Layer.LocalMatrices[BoneIndex];
	}
}

void CAnimator3D::Layer_Blend(const ANIM_LAYER& Layer)
{
	if (-1 == Layer.iStartBoneIndex) {

	}
}

void CAnimator3D::Layer_Additive(const ANIM_LAYER& Layer)
{
	if (-1 == Layer.iStartBoneIndex) {

	}
}

void CAnimator3D::BuildBone()
{
	for (auto& Layer : m_AnimLayers) {
		switch (Layer.eLayerType)
		{
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
				XMLoadFloat4x4(&m_TransformationMatrices[i]);

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

	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		XMStoreFloat4x4(&m_FinalMatices[i], m_pData->Get_OffsetMatrix(i) * XMLoadFloat4x4(&m_CombinedMatrices[i]));
	}
}

void CAnimator3D::Render_GUI()
{
	ImGui::SeparatorText("Animator 3D");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##Animator 3DChild", ImVec2{ 0, childHeight }, true);
	for (int i = 0; i < m_pAnimClips.size(); i++)
	{
		bool isSelected = (m_iCurrentClipIndex == i);
		ImGui::PushID((int)i);

		if (ImGui::Selectable(m_pAnimClips[i]->Get_Name().c_str(), isSelected, 0, ImVec2{ childWidth * 0.50f, textLineHeight }))
		{
			Set_Animation(0, i);
		}
		ImGui::PopID();

		ImGui::PushID(("##" + m_pAnimClips[i]->Get_Name() + "Loop").c_str());
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", m_pAnimClips[i]->Get_Name().c_str());
		}
		//if (ImGui::Button(string(m_pAnimLoops[i] ? "Do Once" : "Do Loop").c_str(), ImVec2{ childWidth * 0.35f, textLineHeight + 4 }))
		//	(m_pAnimLoops[i]) = !(m_pAnimLoops[i]);
		ImGui::PopStyleVar();
		ImGui::PopID();

		if (isSelected) {
			ImGui::SetItemDefaultFocus(); // 선택된 항목에 포커스
		}
	}

	ImGui::EndChild();
}

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

//BUILDERㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ

HRESULT SetAnimBuild::Apply()
{
	if (!m_pOwner || !m_pOwner->isExistLayer(m_iLayerIndex) || !m_pOwner->isExistClip(m_iClipIndex))
		return E_FAIL;

	m_pOwner->Reset_Layer(m_iLayerIndex);
	CAnimator3D::ANIM_LAYER& Layer =
		m_pOwner->m_AnimLayers[m_iLayerIndex];

	Layer.iClipIndex = m_iClipIndex;

	Layer.bLoop = m_bLoop;
	Layer.fAnimSpeed = m_fSpeed;

	return S_OK;
}

SetAnimBuild& SetAnimBuild::Loop(_bool bLoop)
{
	m_bLoop = bLoop;

	return *this;
}

SetAnimBuild& SetAnimBuild::Speed(_float fSpeed)
{
	m_fSpeed = fSpeed;

	return *this;
}

HRESULT ChangeAnimBuild::Apply()
{
	if (!m_pOwner || !m_pOwner->isExistLayer(m_iLayerIndex) || !m_pOwner->isExistClip(m_iClipIndex))
		return E_FAIL;

	m_pOwner->Reset_Layer(m_iLayerIndex);
	auto& Layer = m_pOwner->m_AnimLayers[m_iLayerIndex];

	Layer.iClipIndex = m_iClipIndex;

	Layer.bLoop = m_bLoop;
	Layer.fAnimSpeed = m_fSpeed;

	return S_OK;
}

ChangeAnimBuild& ChangeAnimBuild::BlendState(_float fDuration, BLEND_STATE eBlendState)
{
	m_fBlendDuration = fDuration;
	m_eBlendState = eBlendState;
	return *this;
}

