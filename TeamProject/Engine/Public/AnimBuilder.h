#pragma once
#include "AnimatorStruct.h"

NS_BEGIN(Engine)

template <typename T>
class AnimBuild {
public:
    //Layer
    T& LayerBlend(_float fBlendWeight,
        _float fTargetWeight,
        _float fDuration,
        EaseType eEaseType) {

        m_fLayerWeight = fBlendWeight;
        m_fTargetWeight = fTargetWeight;
        m_fWeightDuration = fDuration;
        m_eLayerEaseType = eEaseType;

        return static_cast<T&>(*this);
    }

    //레이어 애니매이션 업데이트가 멈춤
    T& Pause(_bool bPause) {
        m_bPause = bPause;
        return static_cast<T&>(*this);
    }

    //Clip
    T& Loop(_bool bLoop) {
        m_bLoop = bLoop;
        return static_cast<T&>(*this);
    }

    T& Speed(_float fSpeed) {
        m_fSpeed = fSpeed;
        return static_cast<T&>(*this);
    }

    T& TransitionSpeed(_float fStartSpeed,
        _float fTargetSpeed,
        _float fDuration,
        EaseType eEaseType = EaseType::Linear)
    {
        m_fSpeed = fStartSpeed;
        m_fTargetSpeed = fTargetSpeed;
        m_fEaseDuration = fDuration;
        m_ePlayEaseType = eEaseType;

        return static_cast<T&>(*this);
    }

    //시작 0~1, 끝 0~1, 목표속도 (주의 : 넣은순서대로 실행, 실행중이던 속도보간 무시)
    T& ReserveSpeed(_float fStartPercent, _float fEndPercent, _float fTargetSpeed, EaseType eEaseType) {
        RESERVE_SPEED Reserve{};

        Reserve.Start = clamp(fStartPercent, 0.f, 1.f);
        Reserve.End = clamp(fEndPercent, 0.f, 1.f);
        Reserve.TargetSpeed = fTargetSpeed;
        Reserve.Ease = eEaseType;

        if (Reserve.End <= Reserve.Start)
            return static_cast<T&>(*this);

        if (Reserve.Ease == EaseType::None)
            Reserve.Ease = EaseType::Linear;

        m_Reserves.push(Reserve);

        return static_cast<T&>(*this);
    }

    T& ResetRotation(_bool bResetRotation) {
        m_bPause = bResetRotation;
        return static_cast<T&>(*this);
    }

protected:
    //레이어 가중치, 가중치가 0이면 업데이트 자체를 하지 않음
    //레이어 가중치가 0이되지 않게 끝이나면 매 프레임마다 업데이트 하는거로 간주
    _float   m_fLayerWeight = 0.f;
    _float   m_fTargetWeight = 0.f;
    _float   m_fWeightDuration = 0.f;
    EaseType m_eLayerEaseType = { EaseType::None };
    //반복
    _bool    m_bLoop = false;
    //멈춤
    _bool    m_bPause = false;
    //회전보간 끄기
    _bool    m_bResetRotation = false;
    //애니매이션 속도
    _float   m_fSpeed = 1.f;
    EaseType m_ePlayEaseType = { EaseType::None };
    _float   m_fTargetSpeed = { 1.f };
    _float   m_fEaseDuration = { 0.f };
    queue<RESERVE_SPEED> m_Reserves;
};

class ENGINE_DLL SetAnimBuild
    : public AnimBuild<SetAnimBuild> {
public:
    SetAnimBuild(_int LayerIndex, _int ClipIndex, CAnimator3D* Owner)
        :m_iLayerIndex{ LayerIndex }, m_iClipIndex{ ClipIndex }, m_pOwner{ Owner } {
    }
    ~SetAnimBuild() DEFAULT;

    SetAnimBuild(const SetAnimBuild&) = delete;
    SetAnimBuild& operator=(const SetAnimBuild&) = delete;

public:
    HRESULT Apply();

protected:
    CAnimator3D* m_pOwner = nullptr;
    _int m_iLayerIndex = -1;
    _int m_iClipIndex = -1;
    _bool m_bApplied = false;
};

class ENGINE_DLL ChangeAnimBuild
    : public AnimBuild<ChangeAnimBuild> {
public:
    ChangeAnimBuild(_int LayerIndex, _int ClipIndex, CAnimator3D* Owner)
        : m_iLayerIndex(LayerIndex), m_iClipIndex(ClipIndex), m_pOwner(Owner) {
    }
    ~ChangeAnimBuild() DEFAULT;

    ChangeAnimBuild(const ChangeAnimBuild&) = delete;
    ChangeAnimBuild& operator=(const ChangeAnimBuild&) = delete;

public:
    HRESULT Apply();
    //---------- 애니매이션 블랜드 속성

    //애니매이션 전환시간
    ChangeAnimBuild& BlendDuration(_float fDuration) {
        m_fBlendDuration = fDuration;
        return *this;
    }
    //애니매이션 전환 가중치 이징
    ChangeAnimBuild& BlendWeightEaseType(EaseType eEaseType) {
        m_eBlendEaseType = eEaseType;
        return *this;
    }
    //이전 클립의 트랙포지션 갖고올건지
    ChangeAnimBuild& KeepTrackPos(_bool bKeepTrackPos) {
        m_bKeepTrackPos = bKeepTrackPos;
        return *this;
    }
    //보간하면서 이전 클립의 업데이트를 허용할건지
    ChangeAnimBuild& Update_PrevClip(_bool bUpdate_PrevClip) {
        m_bUpdate_PrevClip = bUpdate_PrevClip;
        return *this;
    }
    //보간하면서 현재 클립의 업데이트를 허용할건지
    ChangeAnimBuild& Update_NewClip(_bool bUpdate_NewClip) {
        m_bUpdate_NewClip = bUpdate_NewClip;
        return *this;
    }

    ChangeAnimBuild& UseFinalLocalPose(_bool b) {
        m_bUseFinalLocal = b;
        return *this;
    }

    //애니매이션 보간시 회전을 제외할것인지
    ChangeAnimBuild& IgnoreRotation(_bool bIgnoreRotation) {
        m_bIgnoreRotation = bIgnoreRotation;
        return *this;
    }

protected:
    CAnimator3D* m_pOwner = nullptr;
    _int m_iLayerIndex = -1;
    _int m_iClipIndex = -1;

    //클립 블랜드
    _float      m_fBlendDuration = { 0.2f };
    _bool       m_bKeepTrackPos = { false };
    _bool       m_bUpdate_PrevClip = { false };
    _bool       m_bConvertCurMatrix = { false };
    _bool       m_bUpdate_NewClip = { true };
    _bool       m_bIgnoreRotation = { false };
    EaseType    m_eBlendEaseType = { EaseType::Linear };

    _bool       m_bUseFinalLocal = false;
};

NS_END