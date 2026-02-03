#include "Engine_Defines.h"
#include "AnimBuilder.h"
#include "Animator3D.h"
#include "AnimationClip.h"
#include "AnimatorStruct.h"
#include "Helper_Func.h"

//----------  SetAnim Options
HRESULT SetAnimBuild::Apply()
{
	if (!m_pOwner || !m_pOwner->isExistLayer(m_iLayerIndex) || !m_pOwner->isExistClip(m_iClipIndex))
		return E_FAIL;

	//레이어, 클립 적용
	ANIM_LAYER& Layer = m_pOwner->m_AnimLayers[m_iLayerIndex];

	Layer.iClipIndex = m_iClipIndex;

	//베이스 레이어일 경우 마지막 키프레임 위치, 회전을 갖고옴
	if (Layer.BaseLayer) {
		////애니매이션 시작 포지션이 1인지 구분
		//if (m_fStartAt == 0.f) {
		//	Layer.vPrevRootPos = m_pOwner->m_pAnimClips[m_iClipIndex]
		//		->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;
		//	Layer.vPrevRootQuat = m_pOwner->m_pAnimClips[m_iClipIndex]
		//		->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vRotation;
		//}
		//else {
		//	m_pOwner->m_pAnimClips[Layer.iClipIndex]->Sample_KeyFrameByBoneIndex(Layer.iRootBoneIndex, m_fStartAt,
		//		nullptr, &Layer.vPrevRootQuat, &Layer.vPrevRootPos);
		//}
				
		//애니매이션 루프시 마지막 엔드프레임이 1인지 구분
		if (fabs(m_fEndAt - 1.f) < 0.0001f) {
			Layer.vRootEndPos = m_pOwner->m_pAnimClips[m_iClipIndex]
				->Get_EndKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;
			Layer.vRootEndQuat = m_pOwner->m_pAnimClips[m_iClipIndex]
				->Get_EndKeyFrameByBoneIndex(Layer.iRootBoneIndex).vRotation;
		}
		else {
			m_pOwner->m_pAnimClips[m_iClipIndex]->Sample_KeyFrameByBoneIndex(Layer.iRootBoneIndex, m_fEndAt,
				nullptr, &Layer.vRootEndQuat, &Layer.vRootEndPos);
		}

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

	Layer.fCurrentTrackPosition = m_fStartAt * m_pOwner->m_pAnimClips[m_iClipIndex]->Get_Duration();

	//애니매이션 기본
	Layer.bLoop = m_bLoop;
	Layer.bWrapped = false;
	Layer.fEndAt = m_fEndAt;
	Layer.fStartAt = m_fStartAt;
	Layer.bJumpedAnim = true;
	Layer.fAnimSpeed = m_fSpeed;
	Layer.bPause = m_bPause;

	//애니매이션 재생속도
	Layer.ePlayEaseType = m_ePlayEaseType;
	Layer.fTargetSpeed = m_fTargetSpeed;
	Layer.fEaseElapsed = 0.f;
	Layer.fEaseDuration = m_fEaseDuration;
	Layer.isUpdateByTime = true;
	Layer.ReservedSpeeds = m_Reserves;

	//애니매이션이 새로 시작됌
	Layer.bisFinished = false;
	return S_OK;
}

//---------- ++ChangeAnim Options
HRESULT ChangeAnimBuild::Apply()
{
	if (!m_pOwner || !m_pOwner->isExistLayer(m_iLayerIndex) || !m_pOwner->isExistClip(m_iClipIndex))
		return E_FAIL;
	auto& Layer = m_pOwner->m_AnimLayers[m_iLayerIndex];

	//베이스 레이어일 경우 마지막 키프레임 위치, 회전을 갖고옴
	if (Layer.BaseLayer) {
		////애니매이션 시작 포지션이 1인지 구분
		//if (m_fStartAt == 0.f) {
		//	Layer.vPrevRootPos = m_pOwner->m_pAnimClips[m_iClipIndex]
		//		->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;
		//	Layer.vPrevRootQuat = m_pOwner->m_pAnimClips[m_iClipIndex]
		//		->Get_StartKeyFrameByBoneIndex(Layer.iRootBoneIndex).vRotation;
		//}
		//else {
		//	m_pOwner->m_pAnimClips[m_iClipIndex]->Sample_KeyFrameByBoneIndex(Layer.iRootBoneIndex, m_fStartAt,
		//		nullptr, &Layer.vPrevRootQuat, &Layer.vPrevRootPos);
		//}
		//
		//애니매이션 루프시 마지막 엔드프레임이 1인지 구분
		if (fabs(m_fEndAt - 1.f) < 0.0001f) {
			Layer.vRootEndPos = m_pOwner->m_pAnimClips[m_iClipIndex]
				->Get_EndKeyFrameByBoneIndex(Layer.iRootBoneIndex).vTranslation;
			Layer.vRootEndQuat = m_pOwner->m_pAnimClips[m_iClipIndex]
				->Get_EndKeyFrameByBoneIndex(Layer.iRootBoneIndex).vRotation;
		}
		else {
			m_pOwner->m_pAnimClips[m_iClipIndex]->Sample_KeyFrameByBoneIndex(Layer.iRootBoneIndex, m_fEndAt,
				nullptr, &Layer.vRootEndQuat, &Layer.vRootEndPos);
		}

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
	Layer.bWrapped = false;
	Layer.bNoRootMoveDelta = m_bNoRootMoveDelta;
	Layer.fEndAt = m_fEndAt;
	Layer.fStartAt = m_fStartAt;
	Layer.bJumpedAnim = true;
	Layer.fAnimSpeed = m_fSpeed;
	Layer.bPause = m_bPause;

	//애니매이션 재생속도
	Layer.ePlayEaseType = m_ePlayEaseType;
	Layer.fTargetSpeed = m_fTargetSpeed;
	Layer.fEaseElapsed = 0.f;
	Layer.fEaseDuration = m_fEaseDuration;
	Layer.isUpdateByTime = true;
	Layer.ReservedSpeeds = m_Reserves;

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
			Layer.LocalMatrices = Layer.FinalLocalMatrices;
		}

		//이전 애니매이션의 진행도를 가져옴
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
	Layer.fBlendTrackPosition = m_fStartAt * m_pOwner->m_pAnimClips[m_iClipIndex]->Get_Duration();
	Layer.fBlendElapsed = 0.f;
	Layer.fBlendDuration = m_fBlendDuration;
	Layer.eBlendEaseType = m_eBlendEaseType;

	//
	if (m_bUseFinalLocal)
		Layer.LocalMatrices = Layer.FinalLocalMatrices;

	//클립을 업데이트 하지 않겠다면 다음 클립의 0초로 세팅
	if (false == m_bUpdate_NewClip)
		m_pOwner->m_pAnimClips[Layer.iNextClipIndex]->TranslateAnimateMatrixFromDurationNoEvent(Layer.BlendMatrices, 0);

	//애니매이션이 새로 시작됌
	Layer.bisFinished = false;
	return S_OK;
}
