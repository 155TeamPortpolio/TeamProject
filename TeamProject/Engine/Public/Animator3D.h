#pragma once
#include "Component.h"
#include "Engine_Math.h"
#include "AnimationLayout.h"

NS_BEGIN(Engine)
using AnimArg = variant<_int, string>;
enum class ANIM_LAYER_STATE { NONE, OVERRIDE, BLEND, ADDITIVE };

class ENGINE_DLL CAnimator3D :
    public CComponent
{
public:
    friend class SetAnimBuild;
    friend class ChangeAnimBuild;

    typedef struct AnimationLayer {
        //---------- 레이어 속성 (레이어 영구변경)
        ANIM_LAYER_STATE    eLayerType = { ANIM_LAYER_STATE::OVERRIDE };
        _int                iStartBoneIndex = { -1 };
        vector<_int>        AffectedBonesIndices;
        _bool               bPause = { true };
        //루트본 관련
        _bool   bUseTransform = { true };
        _int    iMoveBoneIndex = { -1 };
        _float3 vPrevAnimPos{};

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
    HRESULT Resize_Layer(_uint iLayerCount); //레이어 크기(개수) 지정 //벡터resize와 동일한 기능 
    virtual void Update_Animation(_float dt);

public: 
    //즉시 애니매이션 변경
    class SetAnimBuild Set_Animation(AnimArg ClipArg);
    class SetAnimBuild Set_Animation(_uint LayerIndex, AnimArg ClipArg);
    //애니매이션 보간 변경 (아직 보간처리가 작동하지 않음)
    class ChangeAnimBuild Change_Animation(AnimArg ClipArg);
    class ChangeAnimBuild Change_Animation(_uint LayerIndex, AnimArg ClipArg);
    //레이어 초기화
    virtual void Reset_Layer(_uint LayerIndex);
    //레이어 애니매이션을 멈춤 (초기화 x)
    virtual HRESULT Stop_Animation(_uint LayerIndex); //(구현안됌)
    virtual HRESULT StopAll_Animation(); //(구현안됌)

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
    //이벤트들 불러오는 함수
    const vector<EVENT_INST>& Get_EventBus() const;
    //루트애니매이션 델타값
    _vector Get_RootMotionDelta(_uint LayerIndex = 0);
    //현재 레이어의 Ease중이면 그 비율가져옴
    _float Get_EaseDuration(_uint LayerIndex = 0);
    //현재 레이어 재생속도
    _float Get_AnimSpeed(_uint LayerIndex = 0);
    //퍼즈 상태인지
    _bool Get_isPause(_uint LayerIndex = 0);

    /*----- Setter -----*/
    
    //애니매이션 트랜스폼을 제거
    void Set_NoTransform(_int MoveBoneIndex = -1, _uint LayerIndex = 0);
    //애니매이션 트랜스폼 사용
    void Set_UseTransform(_uint LayerIndex = 0);
    //애니매이션 퍼즈
    void Set_Pause(_bool bPause, _uint LayerIndex = 0);
    //애니매이션을 돌릴 본 설정
    void Set_StartBone(_int StartBoneIndex, _uint LayerIndex = 0);
    void Reset_StartBone(_uint LayerIndex = 0);
    //애니매이션 Tpose로 설정 (※ 애니매이션 레이어 상태가 전부 날아감)
    void Set_TPose();

public:
    void Control_Bone(const string& boneName, _fmatrix BoneMatrix);
    void Control_BoneByIndex(_uint Index, _fmatrix BoneMatrix);
    void Dettach_BoneRelation(_uint Index);
    //이벤트 추가
    void Add_Event(CLIP_EVENT_TYPE EventType, string EventTag);
    void Clear_Events();

public:
    //월드상의 최종 뼈 위치를 가져옴
    _float4x4 Get_BoneMatrix(const string& boneName);
    _float4x4 Get_BoneMatrix(_uint Index);
    _float4x4* Get_BoneMatrixPtr(const string& boneName);
    _float4x4* Get_BoneTransformMatrixPtr(const string& boneName);
    const vector<_float4x4>& Get_BoneMatrices() { return m_CombinedMatrices; };
    vector<_float4x4> Get_BoneMatrices(_uint meshIndex);
    //로컬 뼈 최종위치를 가져옴
    const vector<_float4x4>& Get_CombinedBoneMatrices() { return m_CombinedMatrices; };
    vector<_float4x4>* Get_CombinedBoneMatrices_Ptr() { return &m_CombinedMatrices; };

protected://애니매이션 체크
    //문자열 및 숫자를 인덱스로 잘 바꿔주는 함수
    _int Resolve_ClipIndex(AnimArg ClipArg);
    //레이어 인덱스를 찾음
    _int Find_Clip(const string& ClipTag);
    //존재하는지 여부
    _bool isExistLayer(_int LayerIndex);
    _bool isExistClip(_int ClipIndex);

protected:
    //애니매이션 연산
    void Animation_Run(ANIM_LAYER& Layer, _float dt);
    void Animation_Convert(ANIM_LAYER& Layer, _float dt);
    //레이어 연산
    void Layer_Override(const ANIM_LAYER& Layer);
    void Layer_Blend(const ANIM_LAYER& Layer);
    void Layer_Additive(const ANIM_LAYER& Layer);
    //최종 뼈 계산
    void BuildBone();

public:
    virtual void Render_GUI();

protected:
    void GUI_ShowLayerInfo();
    void GUI_SelectAnim();

private:
    void Reset_Anim();

protected:
    class CModelData* m_pData = {};
 
    vector<ANIM_LAYER>              m_AnimLayers;   //애니매이션 레이어
    vector<class CAnimationClip*>   m_pAnimClips;   //애니매이션 클립
    vector<EVENT_INST>              m_EventBus;     //이벤트 버스

    /* 아래 4개의 값만 제대로 들어오면 애니매이션이 돌아감  */
    vector<_float4x4> m_TPose = {}; //T-Pose Matrices
    vector<_float4x4> m_TransformationMatrices = {};    //애니매이션 클립을 업데이트한 로컬 매트릭스
    vector<_float4x4> m_ManipulateMatrices = {};        //강제로 추가할 매트릭스
    vector<_float4x4> m_CombinedMatrices = {};          //부모로부터 업데이트됀 최종 매트릭스
    unordered_set<_uint> m_DettachedBone = {};

    _int m_iCurrentClipIndex = { -1 };

    /*Managing*/
    vector<_bool> m_pAnimLoops;
    unordered_map<string, _uint> m_pAnimNames;

public:
    static CAnimator3D* Create();
    virtual CComponent* Clone();
    virtual void Free() override;
};

// ───────── Builder

class ENGINE_DLL SetAnimBuild {
public:
    SetAnimBuild(_int LayerIndex, _int ClipIndex, CAnimator3D* Owner)
        :m_iLayerIndex{ LayerIndex }, m_iClipIndex{ ClipIndex }, m_pOwner{ Owner } {}
    ~SetAnimBuild() { if (!m_bApplied) Apply(); };
    
    SetAnimBuild(const SetAnimBuild&) = delete;
    SetAnimBuild& operator=(const SetAnimBuild&) = delete;

public:
    virtual HRESULT Apply();

    //---------- 기본 속성
    
    //애니매이션을 반복재생할건지
    SetAnimBuild& Loop(_bool bLoop);
    //애니매이션 재생속도 (TransitionSpeed랑 마지막에 부른거로 덮어씌임)
    SetAnimBuild& Speed(_float fSpeed);
    //애니매이션 속도를 보간변경 (무조건 변경시점부터 진행, Speed랑 겹침)
    SetAnimBuild& TransitionSpeed(_float fStartSpeed, _float fTargetSpeed, _float fDuration, EaseType eEaseType = EaseType::Linear);
    //애니매이션 속도를 보간변경 (무조건 변경시점부터 진행, Speed랑 겹침)
    SetAnimBuild& Pause(_bool bPause);
protected:
    CAnimator3D* m_pOwner = nullptr;
    _int m_iLayerIndex = -1;
    _int m_iClipIndex = -1;
    _bool m_bApplied = false;

    //---------- 기본 속성
    _bool    m_bLoop = false;
    _bool    m_bPause = false;
    _float   m_fSpeed = 1.f;
    EaseType m_ePlayEaseType = { EaseType::None };
    _float   m_fTargetSpeed = { 1.f };
    _float   m_fEaseDuration = { 0.f };
};

class ENGINE_DLL ChangeAnimBuild : public SetAnimBuild {
public:
    ChangeAnimBuild(_int LayerIndex, _int ClipIndex, CAnimator3D* Owner)
        :SetAnimBuild(LayerIndex, ClipIndex, Owner) {}
    ~ChangeAnimBuild() DEFAULT;

    ChangeAnimBuild(const ChangeAnimBuild&) = delete;
    ChangeAnimBuild& operator=(const ChangeAnimBuild&) = delete;

public:
    virtual HRESULT Apply() override;
    //---------- 애니매이션 블랜드 속성

    //애니매이션 전환시간
    ChangeAnimBuild& BlendDuration(_float fDuration);
    //애니매이션 전환 가중치 이징
    ChangeAnimBuild& BlendWeightEaseType(EaseType eEaseType);
    //애니매이션을 변경하면서 이전 클립의 트랙포지션을 같이 사용해서 섞을건지
    ChangeAnimBuild& KeepTrackPos(_bool bKeepTrackPos);

protected:
    _float      m_fBlendDuration = 0.2f;
    _bool       m_bKeepTrackPos = false;
    EaseType    m_eBlendEaseType = { EaseType::Linear };
};

NS_END