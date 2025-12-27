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

	//0번 레이어는 베이스레이어, Layer.BaseLayer는 절대 건들지 말것
	m_AnimLayers[0].BaseLayer = true;
	m_AnimLayers[0].iRootBoneIndex = m_pData->Find_BoneIndexByName("Root");

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

	Clear_Events();

	for (auto& Layer : m_AnimLayers) {
		if (Layer.bPause) continue;

		if (Layer.bBlending)
			Animation_Convert(Layer, dt);
		else	
			Animation_Run(Layer, dt);
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

_bool CAnimator3D::isCurrentAnimEnd(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex))
		return true;

	ANIM_LAYER& Layer = m_AnimLayers[LayerIndex];

	if (!isExistClip(Layer.iClipIndex))
		return 0.f;

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

	if (!isExistClip(Layer.iClipIndex))
		return 0.f;

	auto& nowClip = m_pAnimClips[Layer.iClipIndex];

	_float threshold = nowClip->Get_Duration() * percent;

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

const vector<EVENT_INST>& CAnimator3D::Get_EventBus() const
{
	return m_EventBus;
}

_vector CAnimator3D::Get_RootMotionDelta(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return XMVectorZero();

	return XMVectorZero();
}

_vector CAnimator3D::Get_MoveBoneMotionDelta(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return XMVectorZero(); 

	if (-1 == m_AnimLayers[LayerIndex].iMoveBoneIndex)
		return XMVectorZero();

	_float4x4 MoveBoneMat =
		m_AnimLayers[LayerIndex].LocalMatrices[m_AnimLayers[LayerIndex].iMoveBoneIndex];

	_float3 fCurAnimPos = { MoveBoneMat._41, MoveBoneMat._42, MoveBoneMat._43 };

	return (XMLoadFloat3(&fCurAnimPos) - XMLoadFloat3(&m_AnimLayers[LayerIndex].vPrevMoveBonePos));
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

void CAnimator3D::Set_ExtractBoneMovement(_int MoveBoneIndex, _bool UseX, _bool UseY, _bool UseZ, _uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return;

	auto& Layer = m_AnimLayers[LayerIndex];

	Layer.iMoveBoneIndex = MoveBoneIndex;
	Layer.bUseBoneX = UseX;
	Layer.bUseBoneY = UseY;
	Layer.bUseBoneZ = UseZ;
}

void CAnimator3D::Reset_ExtractBoneMovement(_uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return;

	auto& Layer = m_AnimLayers[LayerIndex];

	Layer.iMoveBoneIndex = -1;
	Layer.bUseBoneX = true;
	Layer.bUseBoneY = true;
	Layer.bUseBoneZ = true;
}

void CAnimator3D::Set_Pause(_bool bPause, _uint LayerIndex)
{
	if (!isExistLayer(LayerIndex)) return;
	m_AnimLayers[LayerIndex].bPause = true;
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
	m_EventBus.push_back(EVENT_INST{EventType, EventTag});
}

void CAnimator3D::Clear_Events()
{
	m_EventBus.clear();
}

_float4x4 CAnimator3D::Get_BoneMatrix(const string& boneName)
{
	_int Index = m_pData->Find_BoneIndexByName(boneName);
	if (Index == -1)  return _float4x4{};
	else {
		return m_CombinedMatrices[Index];
	}
}

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

_float4x4 CAnimator3D::Get_BoneMatrix(_uint Index)
{
	if (Index >= m_ManipulateMatrices.size()) return _float4x4{};
	else {
		return m_CombinedMatrices[Index];
	}
}

_float4x4* CAnimator3D::Get_BoneMatrixPtr(const string& boneName)
{
	_int Index = m_pData->Find_BoneIndexByName(boneName);
	if (Index == -1)  return nullptr;
	else {
		return &m_CombinedMatrices[Index];
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

void CAnimator3D::Animation_Run(ANIM_LAYER& Layer, _float dt)
{
	//Empty Layer
	if (m_AnimLayers.empty()) return;
	
	//Empty Clip
	if (-1 == Layer.iClipIndex) return;

	//Update Animation
	auto& nowClip = m_pAnimClips[Layer.iClipIndex];

	//Calc Animation Speed;
	_float AnimSpeed = Layer.fAnimSpeed;
	if (EaseType::None != Layer.ePlayEaseType) {
		Layer.fEaseElapsed += dt;

		float t = min(Layer.fEaseElapsed / Layer.fEaseDuration, 1.f);
		float Ease = Math::ApplyEase(Layer.ePlayEaseType, t);
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

	if (Layer.BaseLayer) {
		if (Layer.bWrapped) {

		}
		else {
			auto RootMat = Layer.LocalMatrices[Layer.iRootBoneIndex];
			_float3 vCurRootPos = { RootMat._41, RootMat._42 ,RootMat._43 };
			XMStoreFloat3(&Layer.vRootDelta,
				(XMLoadFloat3(&vCurRootPos) - XMLoadFloat3(&Layer.vPrevRootPos)));
		}
	}
}

void CAnimator3D::Animation_Convert(ANIM_LAYER& Layer, _float dt)
{
	if (-1 == Layer.iClipIndex) return;

	auto& nowClip = m_pAnimClips[Layer.iClipIndex];
	auto& nextClip = m_pAnimClips[Layer.iNextClipIndex];

	//Calc Animation Speed;
	_float AnimSpeed = Layer.fAnimSpeed;
	if (EaseType::None != Layer.ePlayEaseType) {
		Layer.fEaseElapsed += dt;

		float t = min(Layer.fEaseElapsed / Layer.fEaseDuration, 1.f);
		float Ease = Math::ApplyEase(Layer.ePlayEaseType, t);
		AnimSpeed = Math::Lerp(Layer.fAnimSpeed, Layer.fTargetSpeed, Ease);

		if (1.f <= t) {
			Layer.fAnimSpeed = AnimSpeed;
			Layer.ePlayEaseType = EaseType::None;
		}
	}

	_float playSpeed = dt * AnimSpeed;

	//Update TrackPos
	if (Layer.bKeepTrackPos) {
		Layer.fCurrentTrackPosition = nowClip->TranslateAnimateMatrix(
			Layer.LocalMatrices, Layer.fCurrentTrackPosition,
			playSpeed, Layer.bLoop, &Layer.bWrapped, &Layer.bisFinished, m_EventBus);
	}

	Layer.fBlendTrackPosition = nextClip->TranslateAnimateMatrix(
		Layer.BlendMatrices, Layer.fBlendTrackPosition,
		playSpeed, Layer.bLoop, &Layer.bWrapped, &Layer.bisFinished, m_EventBus);

	//Animation Blend
	Layer.fBlendElapsed += dt;
	_float fBlendRate = Math::ApplyEase(Layer.eBlendEaseType,
		Layer.fBlendElapsed / Layer.fBlendDuration);

	for (_uint i = 0; i < m_pData->Get_BoneCount(); ++i)
	{
		_matrix base = XMLoadFloat4x4(&Layer.LocalMatrices[i]);
		_matrix blend = XMLoadFloat4x4(&Layer.BlendMatrices[i]);
		_vector baseS, baseR, baseT;
		_vector blendS, blendR, blendT;

		XMMatrixDecompose(&baseS, &baseR, &baseT, base);
		XMMatrixDecompose(&blendS, &blendR, &blendT, blend);

		_vector blendedS = XMVectorLerp(baseS, blendS, fBlendRate);
		_vector blendedT = XMVectorLerp(baseT, blendT, fBlendRate);
		_vector blendedR = XMQuaternionSlerp(baseR, blendR, fBlendRate);

		_matrix blendedM = XMMatrixAffineTransformation(
			blendedS, XMVectorSet(0.f, 0.f, 0.f, 1.f), blendedR, blendedT);

		XMStoreFloat4x4(&Layer.FinalLocalMatrices[i], blendedM);
	}

	//Eliminate Transform
	if (-1 != Layer.iMoveBoneIndex) {
		_float4x4& mat = Layer.LocalMatrices[Layer.iMoveBoneIndex];
		//지금은 Transform만 가져오고 있지만 혹시 회전이나 크기가 필요하면 매트릭스 자체를 저장해도 무관
		Layer.vPrevMoveBonePos = _float3(mat._41, mat._42, mat._43);
		if (!Layer.bUseBoneX) mat._41 = 0.f;
		if (!Layer.bUseBoneY) mat._42 = 0.f;
		if (!Layer.bUseBoneZ) mat._43 = 0.f;
	}

	//Convert End
	if (Layer.fBlendDuration < Layer.fBlendElapsed) {
		Layer.bBlending = false;

		Layer.iClipIndex = Layer.iNextClipIndex;
		Layer.fCurrentTrackPosition = Layer.fBlendTrackPosition;
		Layer.fBlendElapsed = 0.f;
		Layer.fBlendDuration = 0.f;

		Layer.LocalMatrices = Layer.FinalLocalMatrices;
	}
}

void CAnimator3D::Layer_Override(const ANIM_LAYER& Layer)
{
	if (-1 == Layer.iStartBoneIndex) {
		if(Layer.bBlending)
			m_TransformationMatrices = Layer.FinalLocalMatrices;
		else
			m_TransformationMatrices = Layer.LocalMatrices;
	}
	else {
		for (_int BoneIndex : Layer.AffectedBonesIndices) {
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
		case Engine::ANIM_LAYER_STATE::NONE:
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
}

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

	if(isExistClip(curLayer.iClipIndex))
		AnimName = m_pAnimClips[curLayer.iClipIndex]->Get_Name();

	string AnimInfo = "Clip : " + to_string(curLayer.iClipIndex) + " | Name : " + AnimName;
	ImGui::Text(AnimInfo.c_str());
	ImGui::Separator();

	// ───────── Play bar
	if(ImGui::Button(curLayer.bPause ? "Play" : "Pause", ImVec2(60.f, 0.f))) {
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

	ImGui::BeginChild("##Animator Animation", ImVec2{ 0, childHeight }, true);
	for (int i = 0; i < m_pAnimClips.size(); i++)
	{
		bool isSelected = (m_iCurrentClipIndex == i);
		ImGui::PushID((int)i);

		if (ImGui::Selectable(m_pAnimClips[i]->Get_Name().c_str(), isSelected, 0, ImVec2{ childWidth * 0.50f, textLineHeight }))
		{
			Change_Animation(i).Loop(true);
		}
		ImGui::PopID();

		ImGui::PushID(("##" + m_pAnimClips[i]->Get_Name() + "Loop").c_str());
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0));
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", m_pAnimClips[i]->Get_Name().c_str());
		}
		ImGui::PopStyleVar();
		ImGui::PopID();

		if (isSelected) {
			ImGui::SetItemDefaultFocus();
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

//BUILDER------------------------------------------------------------------------------------------

//----------  SetAnim Options
HRESULT SetAnimBuild::Apply()
{
	if (!m_pOwner || !m_pOwner->isExistLayer(m_iLayerIndex) || !m_pOwner->isExistClip(m_iClipIndex))
		return E_FAIL;

	//m_pOwner->Reset_Layer(m_iLayerIndex);
	CAnimator3D::ANIM_LAYER& Layer = m_pOwner->m_AnimLayers[m_iLayerIndex];

	Layer.iClipIndex = m_iClipIndex;

	Layer.bLoop = m_bLoop;
	Layer.fCurrentTrackPosition = 0.f;
	Layer.fAnimSpeed = m_fSpeed;
	Layer.bPause = m_bPause;

	Layer.ePlayEaseType = m_ePlayEaseType;
	Layer.fTargetSpeed = m_fTargetSpeed;
	Layer.fEaseElapsed = 0.f;
	Layer.fEaseDuration = m_fEaseDuration;

	Layer.bisFinished = false;

	m_bApplied = true;
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

SetAnimBuild& SetAnimBuild::TransitionSpeed(_float fStartSpeed, _float fTargetSpeed, _float fDuration, EaseType eEaseType)
{
	m_fSpeed = fStartSpeed;
	m_fTargetSpeed = fTargetSpeed;
	m_fEaseDuration = fDuration;
	m_ePlayEaseType = eEaseType;

	return *this;
}

SetAnimBuild& SetAnimBuild::Pause(_bool bPause)
{
	m_bPause = bPause;
	return *this;
}

//---------- ++ChangeAnim Options
HRESULT ChangeAnimBuild::Apply()
{
	if (!m_pOwner || !m_pOwner->isExistLayer(m_iLayerIndex) || !m_pOwner->isExistClip(m_iClipIndex))
		return E_FAIL;

	auto& Layer = m_pOwner->m_AnimLayers[m_iLayerIndex];

	Layer.bLoop = m_bLoop;
	Layer.fAnimSpeed = m_fSpeed;
	Layer.bPause = m_bPause;

	Layer.ePlayEaseType = m_ePlayEaseType;
	Layer.fTargetSpeed = m_fTargetSpeed;
	Layer.fEaseElapsed = 0.f;
	Layer.fEaseDuration = m_fEaseDuration;

	Layer.bBlending = true;
	Layer.bKeepTrackPos = m_bKeepTrackPos;
	Layer.iNextClipIndex = m_iClipIndex;
	Layer.fBlendTrackPosition = 0.f;
	Layer.fBlendElapsed = 0.f;
	Layer.fBlendDuration = m_fBlendDuration;
	Layer.eBlendEaseType = m_eBlendEaseType;

	Layer.bisFinished = false;

	m_bApplied = true;
	return S_OK;
}

ChangeAnimBuild& ChangeAnimBuild::BlendDuration(_float fDuration)
{
	m_fBlendDuration = fDuration;
	return *this;
}

ChangeAnimBuild& ChangeAnimBuild::BlendWeightEaseType(EaseType eEaseType)
{
	m_eBlendEaseType = eEaseType;
	return *this;
}

ChangeAnimBuild& ChangeAnimBuild::KeepTrackPos(_bool bKeepTrackPos)
{
	m_bKeepTrackPos = bKeepTrackPos;
	return *this;
}

