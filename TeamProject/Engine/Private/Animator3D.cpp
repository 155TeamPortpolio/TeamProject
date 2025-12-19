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
	m_TransfromationMatrices{ rhs.m_TransfromationMatrices },
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

	m_TransfromationMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_CombinedMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_FinalMatices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_ManipulateMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);

	/*뼈 개수만큼 뼈의 로컬상태를 가져옴*/
	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		m_TransfromationMatrices[i] = m_pData->Get_TransformMatrix(i);
	}
	/*부모 뼈를 받을 수 있게 기본값으로 초기화*/
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

HRESULT CAnimator3D::Set_Animation(_uint LayerIndex, string ClipTag)
{
	return E_NOTIMPL;
}

HRESULT CAnimator3D::Set_Animation(_uint LayerIndex, _uint Clipindex)
{
	return E_NOTIMPL;
}

HRESULT CAnimator3D::Change_Animation(_uint LayerIndex, string ClipTag, _float convertDuration)
{
	return E_NOTIMPL;
}

HRESULT CAnimator3D::Change_Animation(_uint LayerIndex, _uint Clipindex, _float convertDuration)
{
	return E_NOTIMPL;
}

HRESULT CAnimator3D::ForceChange_Animation(_uint LayerIndex, string ClipTag, _bool overrideSame, _float convertDuration)
{
	return E_NOTIMPL;
}

HRESULT CAnimator3D::ForceChange_Animation(_uint LayerIndex, _uint Clipindex, _bool overrideSame, _float convertDuration)
{
	return E_NOTIMPL;
}

HRESULT CAnimator3D::Stop_Animation(_uint LayerIndex)
{
	return E_NOTIMPL;
}

HRESULT CAnimator3D::StopAll_Animation(_uint LayerIndex)
{
	return E_NOTIMPL;
}


_bool CAnimator3D::isCurrentAnimEnd()
{
	if (m_iCurrentClipIndex == -1)
		return true;
	if (m_eState == ANIMATOR_STATE::CONVERTING)
		return false;
	else if (m_pAnimLoops[m_iCurrentClipIndex])
		return false;
	else
		return isAnimEnd;
}

_bool CAnimator3D::isOverAnimTiming(_float percent)
{
	auto& nowClip = m_pAnimClips[m_iCurrentClipIndex];
	_float threshold = nowClip->Get_Duration() * percent;

	// 이전 프레임 트랙 위치 < 기준 <= 현재 트랙 위치일 때 true
	return  m_fCurrentTrackPosition >= threshold;
}

string CAnimator3D::Get_CurrentAnimName(_uint LayerIndex)
{
	if (m_eState == ANIMATOR_STATE::CONVERTING)
		return m_pAnimClips[m_iNextClipIndex]->Get_Name();
	else
		return m_pAnimClips[m_iCurrentClipIndex]->Get_Name();
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
		return &m_TransfromationMatrices[Index];
	}
}

void CAnimator3D::Animation_Run(_float dt)
{
	if (m_iCurrentClipIndex == -1)  return;

	auto& nowClip = m_pAnimClips[m_iCurrentClipIndex];
	//m_fCurrentTrackPosition = nowClip->TranslateAnimateMatrix(
	//	m_TransfromationMatrices, m_fCurrentTrackPosition,
	//	dt, m_pAnimLoops[m_iCurrentClipIndex], &isAnimEnd);

	m_fCurrentTrackPosition = nowClip->TranslateAnimateMatrix(
		m_TransfromationMatrices, m_fCurrentTrackPosition,
		dt, true, &isAnimEnd);

	//if (m_pAnimLoops[m_iCurrentClipIndex] == false && isAnimEnd) {
	//	m_eState = ANIMATOR_STATE::IDLE;
	//}
}

void CAnimator3D::Animation_Convert(_float dt)
{

}

void CAnimator3D::Override_BlendAnim()
{
	// base와 blend 보간
	for (size_t i = 0; i < m_BlendIndex.size(); ++i)
	{
		_uint idx = m_BlendIndex[i];
		_matrix base = XMLoadFloat4x4(&m_TransfromationMatrices[idx]);
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

		XMStoreFloat4x4(&m_TransfromationMatrices[idx], blendedM);
	}
}


void CAnimator3D::BuildBone()
{
	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		int parent = m_pData->Get_BoneParentIndex(i);

		if (parent == -1) {
			_matrix MyTransformation =
				XMLoadFloat4x4(&m_ManipulateMatrices[i]) *
				XMLoadFloat4x4(&m_TransfromationMatrices[i]);

			XMStoreFloat4x4(&m_CombinedMatrices[i], MyTransformation);
		}
		else if (m_DettachedBone.count(i)) {
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
}

void CAnimator3D::Render_GUI()
{
	ImGui::SeparatorText("Animator 3D");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##Animator 3DChild", ImVec2{ 0, childHeight }, true);
	for (size_t i = 0; i < m_pAnimClips.size(); i++)
	{
		bool isSelected = (m_iCurrentClipIndex == i);
		ImGui::PushID((int)i);

		if (ImGui::Selectable(m_pAnimClips[i]->Get_Name().c_str(), isSelected, 0, ImVec2{ childWidth * 0.50f, textLineHeight }))
		{
			Change_Animation(i);
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
	ImGui::Text("m_fBlendDuration : %.4f", m_fBlendDuration);
	switch (m_eBlendState)
	{
	case Engine::CAnimator3D::BLENDER_STATE::NONE:
		ImGui::Text("NONE BLEND");
		break;
	case Engine::CAnimator3D::BLENDER_STATE::BLEND_IN:
		ImGui::Text(" BLEND_IN");
		break;
	case Engine::CAnimator3D::BLENDER_STATE::RUNNING:
		ImGui::Text(" BLEND RUNNING");
		break;
	case Engine::CAnimator3D::BLENDER_STATE::BLEND_OUT:
		ImGui::Text(" BLEND_OUT");
		break;
	case Engine::CAnimator3D::BLENDER_STATE::BLEND_PAUSE:
		ImGui::Text(" BLEND_PAUSE");
		break;
	default:
		break;
	}

	ImGui::EndChild();

}

void CAnimator3D::Reset_Anim()
{
	vector<_float4x4> dum1;
	m_TransfromationMatrices.swap(dum1);
	vector<_float4x4> dum2;
	m_CombinedMatrices.swap(dum2);
	vector<_float4x4> dum3;
	m_FinalMatices.swap(dum3);
	vector<_float4x4> dum4;
	m_ManipulateMatrices.swap(dum4);

	m_iNextClipIndex = { -1 }; //다음 애니메이션 전환 용
	m_fConvertDuration = {};
	m_fPrevTrackPosition = {}; //다음 애니메이션 전환 용

	 m_iCurrentClipIndex = { -1 };
	 m_fCurrentTrackPosition = {};
	 isAnimEnd = { false };
	 m_DettachedBone = {};


	  m_iBlendAnimation = { -1 };
	  m_fBlendTrackPosition = {};
	  m_fBlendConversionTrackPosition = {};
	  m_fBlendWeight = { 1.5f };
	  vector<_uint> dum5;
	  m_BlendIndex.swap(dum5);
	  vector<_float4x4> dum6;
	  m_BlendTransfomationMatices.swap(dum6);
	  isBlendAnimEnd = { false };
	  m_eBlendState = {BLENDER_STATE::NONE};
	  m_eState = { ANIMATOR_STATE::IDLE };

	 /*Managing*/
	  vector<_bool> dum7;
	m_pAnimLoops.swap(dum7);
	 unordered_map<string, _uint> m_pAnimNames;

	for (auto& Clip : m_pAnimClips) {
		Safe_Release(Clip);
	}
	m_pAnimNames.clear();
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
}
