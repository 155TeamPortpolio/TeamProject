#pragma once
#include "Helper_Func.h"

NS_BEGIN(Engine)

typedef struct ReserveSpeed {
    _float Start, End, TargetSpeed;
    EaseType Ease = { EaseType::None };
}RESERVE_SPEED;

enum class ANIM_LAYER_STATE { NONE, BASE, OVERRIDE, BLEND, ADDITIVE };

typedef struct AnimationLayer {
    //---------- 레이어 속성 (레이어 영구변경)
    _bool               BaseLayer = { false };
    ANIM_LAYER_STATE    eLayerType = { ANIM_LAYER_STATE::OVERRIDE };

    _bool               bPause = { true };
    _float              fLayerWeight = {};
    _float              fTargetLayerWeight = {};
    _float              fLayerWeightElapsed = {};
    _float              fLayerWeightDuration = {};
    EaseType            eLayerEaseType = { EaseType::None };
    _int                iStartBoneIndex = { -1 };
    vector<_int>        AffectedBonesIndices;

    _bool               bUseFinalLocal = false; //마지막 로컬 기준으로 블랜드할건지

    //---------- 베이스 레이어 속성
    //루트본 델타값 (베이스 레이어만, 실질적인 움직임을 담당하는 본)
    _bool               bWrapped = { false };
    _bool               bIgnoreCalcRootDelta = { false };
    _int                iRootBoneIndex = { -1 }; //루트 본 
    _vector3            vRootEndPos{};                          //그 클립의 제일마지막 루트위치
    _quaternion         vRootEndQuat{ _quaternion::Identity };  //그 클립의 제일마지막 루트회전값
    _vector3            vPrevRootPos{};                         //이전 프레임 위치
    _quaternion         vPrevRootQuat{ _quaternion::Identity }; //이전 프레임 회전
    _vector3            vRootMoveDelta{};                       //이동값
    _quaternion         vRootQuatDelta{ _quaternion::Identity };//회전값

    //모션본 (애니매이션의 움직임을 담당하는 본)
    _int    iMotionBoneIndex = { -1 };
    AXIS    eExtractMoveAxis = { AXIS::NONE }; //움직임을 뺄 축
    AXIS    eExtractRotAxis = { AXIS::NONE }; //회전을 뺄 축
    _vector3 vMotionEndPos{};        //그 클립의 제일 마지막 모션위치
    _vector3 vPrevMotionBonePos{};   //이전 프레임 위치

    //---------- 애니매이션 데이터 (변경시 초기화)
    _int    iClipIndex = { -1 };
    _float  fPrevTrackPosition = {};
    _float  fCurrentTrackPosition = {};
    _float  fProgress = { 0.f };
    _float  fAnimSpeed = { 1.f };
    _float  fAppliedAnimSpeed = { 1.f }; //속도 변환하는 변수용
    _bool   bLoop = { false };
    _float  fEndAt = { 1.f };   //애니매이션  끝날 위치 
    _float  fStartAt = { 1.f }; //애니매이션 시작위치
    _bool   bisFinished = { true };
    //재생 속도관련
    EaseType ePlayEaseType = { EaseType::None };
    _float  fTargetSpeed{};
    _float  fEaseDuration{};
    _float  fEaseElapsed{};
    _bool   isUpdateByTime = { true }; //시간으로 애니매이션 속도를 조절하는지
    _float  fStartProgress{};
    _float  fEndProgress{};
    queue<RESERVE_SPEED> ReservedSpeeds;

    //로컬 매트릭스
    vector<_float4x4> LocalMatrices = {};

    //---------- 블렌드 상태 (변경시 초기화)
    _bool   bBlending = { false };
    _bool   bKeepTrackPos = { false };
    _bool   bUpdate_PrevClip = { false };
    _bool   bUpdate_NewClip = { false };
    _bool   bIgnoreRotation = { false };
    _int    iNextClipIndex = { -1 };
    _float  fBlendTrackPosition = {};
    _float  fBlendElapsed = {};
    _float  fBlendDuration = {};
    EaseType eBlendEaseType = { EaseType::None };

    //다음 매트릭스
    vector<_float4x4> BlendMatrices = {};

    //보간을 다한 최종 매트릭스
    vector<_float4x4> FinalLocalMatrices = {};
}ANIM_LAYER;

NS_END