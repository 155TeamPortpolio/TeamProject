#pragma once
#include "Component.h"
#include "Engine_Math.h"

NS_BEGIN(Engine)
using AnimArg = variant<_int, string>;
enum class ANIM_LAYER_STATE { NONE, BASE, OVERRIDE, BLEND, ADDITIVE };

class ENGINE_DLL CAnimator3D :
    public CComponent
{
public:
    friend class SetAnimBuild;
    friend class ChangeAnimBuild;

    typedef struct AnimationLayer {
        //---------- 레이어 속성 (레이어 영구변경)
        _bool               BaseLayer = { false };
        ANIM_LAYER_STATE    eLayerType = { ANIM_LAYER_STATE::OVERRIDE };
        
        _bool               bPause = { true };
        _bool               bApplied = { false };
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
        _int                iRootBoneIndex = { -1 }; //루트 본 
        _vector3            vRootEndPos{};                          //그 클립의 제일마지막 루트위치
        _vector4            vRootEndQuat{ _quaternion::Identity };  //그 클립의 제일마지막 루트회전값
        _vector3            vPrevRootPos{};                         //이전 프레임 위치
        _vector4            vPrevRootQuat{ _quaternion::Identity }; //이전 프레임 회전
        _vector3            vRootMoveDelta{};                       //이동값
        _vector4            vRootQuatDelta{ _quaternion::Identity };//회전값

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
        _float  fAnimSpeed = { 1.f };
        _bool   bLoop = { false };
        _bool   bisFinished = { true };
        //재생 속도관련
        EaseType ePlayEaseType = { EaseType::None };
        _float  fTargetSpeed;
        _float  fEaseDuration = {};
        _float  fEaseElapsed = {};

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

protected:
    CAnimator3D();
    CAnimator3D(const CAnimator3D& rhs);
    ~CAnimator3D() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    void LinkAnimate_Model(const string& LevelKey, const string& ModelKey);
    HRESULT Link_MetaData(const string& LevelKey, const string& MetaClipKey);
    HRESULT Link_DynamicBone();
    HRESULT Resize_Layer(_uint iLayerCount); //레이어 크기(개수) 지정 //벡터resize와 동일한 기능 
    virtual void Update_Animation(_float dt);

public: 
    //즉시 애니매이션 변경
    class SetAnimBuild Set_Animation(AnimArg ClipArg);
    class SetAnimBuild Set_Animation(_uint LayerIndex, AnimArg ClipArg);
    //애니매이션 보간 변경
    class ChangeAnimBuild Change_Animation(AnimArg ClipArg);
    class ChangeAnimBuild Change_Animation(_uint LayerIndex, AnimArg ClipArg);
    //레이어 초기화
    virtual void Reset_Layer(_uint LayerIndex);
    //레이어 애니매이션을 멈춤 (초기화 x)
    virtual HRESULT Stop_Animation(_uint LayerIndex);
    //전체 레이어 애니매이션을 멈추되 다시 켜려면 각각 퍼즈를 풀어주어야함.
    virtual HRESULT StopAll_Animation(); 

public://애니매이터 데이터
    /*----- is -----*/

    //현재 레이어의 애니매이션이 끝났는지
    _bool isCurrentAnimEnd(_uint LayerIndex = 0);
    //현재 레이어의 클립이 0~1사이의 비율을 받고, 그 값의 비율을 넘어섰는지
    _bool isOverClipTiming(_float percent, _uint LayerIndex = 0);
    //현재 레이어의 애니매이션이 블랜드 중인지
    _bool isBlending(_uint LayerIndex = 0);

    /*----- Getter -----*/

    //현재 레이어 클립의 진행률 0~1 반환
    _float Get_CurAnimDuration(_uint LayerIndex = 0);
    //현재 레이어의 애니매이션 이름
    string Get_CurAnimName(_uint LayerIndex = 0);
    //현재 레이어의 애니매이션 인덱스
    _int Get_CurAnimIndex(_uint LayerIndex = 0);
    //현재 레이어 개수
    _int Get_NumLayer();
    //이벤트들 불러오는 함수 (매 프레임 벡터 내용물이 지워짐)
    const vector<EVENT_INST>& Get_EventBus() const;
    //"Root"라는 이름을 가진 본의 움직임 델타값
    _float3 Get_RootBoneMoveDelta() const;
    _float4 Get_RootBoneQuatDelta() const;
    //Root의 마지막 회전 쿼터니언값
    _float4 Get_RootBoneEndQuat() const;
    //모션본 애니매이션 델타값
    _vector Get_MotionBoneDelta(_uint LayerIndex = 0);
    //현재 레이어의 Ease중이면 그 비율가져옴
    _float Get_EaseDuration(_uint LayerIndex = 0);
    //현재 레이어 재생속도
    _float Get_AnimSpeed(_uint LayerIndex = 0);
    //퍼즈 상태인지
    _bool Get_isPause(_uint LayerIndex = 0);
    //
    class CModelData* Get_ModelData() { return m_pData; }
    /*----- Setter -----*/
    
    //모션본 직접설정
    void Set_MotionBone(_int MoveBoneIndex);
    //애니매이션 축에 움직임 제거 (ex : (AXIS::X | AXIS::Z) = XZ축제거)
    void Set_ExtractMotionboneMovement(AXIS eAxis);
    //움직임 축 고정 리셋
    void Reset_ExtractBoneMovement();
    //애니매이션 퍼즈
    void Set_Pause(_bool bPause, _uint LayerIndex = 0);
    //애니매이션을 돌릴 본 설정
    void Set_StartBone(_int StartBoneIndex, _uint LayerIndex = 0);
    void Reset_StartBone(_uint LayerIndex = 0);
    //애니매이션 Tpose로 설정 (※ 애니매이션 레이어 상태가 전부 날아감)
    void Set_TPose();
    //애니매이션 레이어 타입
    void Set_LayerType(ANIM_LAYER_STATE eLayerType, _uint LayerIndex = 0);

    /*----- Change -----*/

    //현재 레이어 애니매이션 속도만 변경
    void Chagne_Speed(_float fSpeed, _uint LayerIndex = 0);
    //현재 레이어 애니매이션 현재 속도에서부터 보간 변경
    void Change_TransitionSpeed(_float fTargetSpeed, _float fDuration, EaseType eEaseType = EaseType::Linear, _uint LayerIndex = 0);

    /*----- Calculator -----*/
    _quaternion Calc_TransformFromEndAnim(const _vector4& vTransformQuat);

public:
    void Control_Bone(const string& boneName, _fmatrix BoneMatrix);
    void Control_BoneByIndex(_uint Index, _fmatrix BoneMatrix);
    void Dettach_BoneRelation(_uint Index);
    //이벤트 추가
    void Add_Event(CLIP_EVENT_TYPE EventType, string EventTag);
    void Clear_Events();

    
public: //뼈 관련
    vector<_float4x4> Get_BoneMatrices(_uint meshIndex);

    //Transformation
    _float4x4 Get_BoneTransformationMatrix(AnimArg BoneArg);
    _float4x4* Get_BoneTransformationMatrixPtr(AnimArg BoneArg);
    _vector3 Get_BoneTransformationPosition(AnimArg BoneArg);
    _vector4 Get_BoneTransformationQuaternion(AnimArg BoneArg);
    const vector<_float4x4>& Get_TransformationMatrices() { return m_TransformationMatrices; };
    vector<_float4x4>* Get_TransformationBoneMatrices_Ptr() { return &m_TransformationMatrices; };

    void Set_BoneTransformationMatrix(const _float4x4& Matrix, AnimArg BoneArg);
    void Set_BoneTransformationPosition(_vector3 Position, AnimArg BoneArg);
    void Set_BoneTransformationQuaternion(_vector4 Quaternion, AnimArg BoneArg);
    
    //Manipulate
    const vector<_float4x4>& Get_ManipulateMatrices() { return m_TransformationMatrices; };
    vector<_float4x4>* Get_ManipulateBoneMatrices_Ptr() { return &m_TransformationMatrices; };

    void Set_BoneManipulateMatrix(const _float4x4& Matrix, AnimArg BoneArg);
    void Set_BoneManipulatePosition(_vector3 Position, AnimArg BoneArg);
    void Set_BoneManipulateQuaternion(_vector4 Quaternion, AnimArg BoneArg);

    //Combined
    _float4x4 Get_BoneCombinedMatrix(AnimArg BoneArg);
    _float4x4* Get_BoneCombinedMatrixPtr(AnimArg BoneArg);
    _vector3 Get_BoneCombinedPosition(AnimArg BoneArg);
    _vector4 Get_BoneCombinedQuaternion(AnimArg BoneArg);
    const vector<_float4x4>& Get_CombinedBoneMatrices() { return m_CombinedMatrices; };
    vector<_float4x4>* Get_CombinedBoneMatrices_Ptr() { return &m_CombinedMatrices; };    

    void Set_BoneCombinedMatrix(const _float4x4& Matrix, AnimArg BoneArg);
    void Set_BoneCombinedPosition(_vector3 Position, AnimArg BoneArg);
    void Set_BoneCombinedQuaternion(_vector4 Quaternion, AnimArg BoneArg);

    //TPose
    const vector<_float4x4>& Get_TPose() { return m_TPose; };

public:
    class CDynamicBone* Get_DynamicBone_Ptr() { 
        if (nullptr == m_pDynamicBone) Link_DynamicBone();
        return m_pDynamicBone;
    };

public://애니매이션 체크
    //문자열 및 숫자를 인덱스로 잘 바꿔주는 함수
    _int Resolve_ClipIndex(AnimArg ClipArg);
    _int Resolve_BoneIndex(AnimArg BoneArg);

    //레이어 인덱스를 찾음
    _int Find_Clip(const string& ClipTag);
    //존재하는지 여부
    _bool isExistLayer(_int LayerIndex);
    _bool isExistClip(_int ClipIndex);
    //축이 있는지 계산
    _bool hasAxis(AXIS eExtractAxis, AXIS Axis);
    
    //매트릭스 보간
    Matrix Calc_MatrixBlend(const _float4x4& base, const _float4x4& target, _float weight);
    Matrix Calc_MatrixAdditive(const _float4x4& base, const _float4x4& target, const _float4x4& TPose,  _float weight);


protected:
    //애니매이션 연산
    void Animation_Run(ANIM_LAYER& Layer, _float dt);
    void Animation_Convert(ANIM_LAYER& Layer, _float dt);
    //레이어 연산
    void Layer_Base(const ANIM_LAYER& Layer);
    void Layer_Override(const ANIM_LAYER& Layer);
    void Layer_Blend(const ANIM_LAYER& Layer);
    void Layer_Additive(const ANIM_LAYER& Layer);

    //최종 뼈 계산
    void Update_Layers(_float dt);
    void BuildLocal(_float dt);
    void BuildIKMatrices(_float dt);
    void Update_DynamicBone(_float dt);
    void BuildBone(_float dt);
    

public:
    virtual void Render_GUI();

protected:
    void GUI_ShowLayerInfo();
    void GUI_SelectAnim();

private:
    void Reset_Anim();

protected:
    class CModelData*   m_pData = { nullptr };
    class CDynamicBone* m_pDynamicBone = { nullptr };

    Matrix m_PreTransform = { Matrix::Identity };
    _bool                           m_bUpdatedClip = { false };
    vector<ANIM_LAYER>              m_AnimLayers;   //애니매이션 레이어
    vector<class CAnimationClip*>   m_pAnimClips;   //애니매이션 클립
    vector<EVENT_INST>              m_EventBus;     //이벤트 버스

    /* 아래 4개의 값만 제대로 들어오면 애니매이션이 돌아감  */
    vector<_float4x4> m_TPose = {};                     //T-Pose Matrices
    vector<_float4x4> m_BasePose = {};                  //Additive용 BasePose << 만약 애니매이션을 여러개 덧붙여야하면 이벡터 자체가 여러개필요
    vector<_float4x4> m_TransformationMatrices = {};    //애니매이션 클립을 업데이트한 로컬 매트릭스
    vector<_float4x4> m_ManipulateMatrices = {};        //강제로 추가할 매트릭스
    vector<_float4x4> m_CombinedMatrices = {};          //부모로부터 업데이트됀 최종 매트릭스
    unordered_set<_uint> m_DettachedBone = {};


    /*Managing*/
    vector<_bool> m_pAnimLoops;
    unordered_map<string, _uint> m_pAnimNames;
    _int m_iCurrentClipIndex{};

public:
    static CAnimator3D* Create();
    virtual CComponent* Clone();
    virtual void Free() override;
};

// ───────── Builder
#pragma region Builder
template <typename T>
class AnimBuild {
public:

    //Layer
    T& LayerBlend(_float fBlendWeight,
        _float fTargetWeight,
        _float fDuration,
        EaseType eEaseType) {

        m_fLayerWeight  = fBlendWeight;
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

};

class ENGINE_DLL SetAnimBuild
    : public AnimBuild<SetAnimBuild> {
public:
    SetAnimBuild(_int LayerIndex, _int ClipIndex, CAnimator3D* Owner)
        :m_iLayerIndex{ LayerIndex }, m_iClipIndex{ ClipIndex }, m_pOwner{ Owner } {}
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
#pragma endregion

NS_END