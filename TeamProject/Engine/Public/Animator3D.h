#pragma once
#include "Component.h"
#include "Engine_Math.h"

NS_BEGIN(Engine)
using AnimArg = variant<_int, string>;
enum class ANIM_LAYER_STATE { OVERRIDE, BLEND, ADDITIVE };
enum class BLEND_STATE { NONE, CROSSFADE, IMMEDIATE };

class ENGINE_DLL CAnimator3D :
    public CComponent
{
public:
    struct AnimConvert {
        _uint SrcClip;
        _uint DstClip;
        _float ConversionElapsedTime = {};
        _float ConversionDuration = {};
    };

    friend class SetAnimBuild;
    friend class ChangeAnimBuild;

    typedef struct AnimationLayer {
        //---------- 레이어 속성 (레이어 영구변경)
        ANIM_LAYER_STATE    eLayerType = { ANIM_LAYER_STATE::OVERRIDE };
        _int                iStartBoneIndex = { -1 };
        vector<_int>        AffectedBonesIndices;
        //루트본 관련
        _bool   bUseTransform = { true };
        _int    iMoveBoneIndex = { -1 };
        _float3 fPrevAnimPos{};

        //---------- 애니매이션 데이터 (변경시 초기화)
        _int    iClipIndex = { -1 };
        _float  fPrevTrackPosition = {};
        _float  fCurrentTrackPosition = {};
        _float  fAnimSpeed = { 1.f };
        _bool   bLoop = { false };
        _bool   bisFinished = { true };

        //로컬 매트릭스
        vector<_float4x4> LocalMatrices = {};
 
        //---------- 블렌드 상태 (변경시 초기화)
        _bool   bBlending = { false };
        _int    iNextClipIndex = { -1 };
        _float  fBlendElapsed = {};
        _float  fBlendDuration = {};
        BLEND_STATE eBlendState = { BLEND_STATE::NONE };
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

public: //클라이언트 사용 전용 함수 .Apply() 로 적용할것
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

    /*----- Setter -----*/
    void Set_NoTransform(_int MoveBoneIndex = -1, _uint LayerIndex = 0);

public:
    void Control_Bone(const string& boneName, _fmatrix BoneMatrix);
    void Control_BoneByIndex(_uint Index, _fmatrix BoneMatrix);
    void Dettach_BoneRelation(_uint Index);

public:
    //월드상의 최종 뼈 위치를 가져옴
    _float4x4 Get_BoneMatrix(const string& boneName);
    _float4x4 Get_BoneMatrix(_uint Index);
    _float4x4* Get_BoneMatrixPtr(const string& boneName);
    _float4x4* Get_BoneTransformMatrixPtr(const string& boneName);
    const vector<_float4x4>& Get_BoneMatrices() { return m_FinalMatices; };
    //로컬 뼈 최종위치를 가져옴
    const vector<_float4x4>& Get_CombinedBoneMatrices() { return m_CombinedMatrices; };

protected://애니매이션 체크
    //문자열 및 숫자를 인덱스로 잘 바꿔주는 함수
    _int Resolve_ClipIndex(AnimArg ClipArg);
    //레이어 인덱스를 찾음
    _int Find_Clip(const string& ClipTag);
    //존재하는지 여부
    _bool isExistLayer(_int LayerIndex);
    _bool isExistClip(_int ClipIndex);

protected://애니매이션 연산
    void Animation_Run(_float dt);
    void Animation_Convert(_float dt);

    void Override_BlendAnim();

    void Layer_Override(const ANIM_LAYER& Layer);
    void Layer_Blend(const ANIM_LAYER& Layer);
    void Layer_Additive(const ANIM_LAYER& Layer);

    void BuildBone();

public:
    virtual void Render_GUI();

private:
    void Reset_Anim();
protected:
    class CModelData* m_pData = {};
    vector<class CAnimationClip*> m_pAnimClips;         //불러온 애니매이션클립들
    vector<ANIM_LAYER> m_AnimLayers;
    vector<string> m_EventBus;

    /* 아래 4개의 값만 제대로 들어오면 애니매이션이 돌아감  */
    vector<_float4x4> m_TransformationMatrices = {};    //애니매이션 클립을 업데이트한 로컬 매트릭스
    vector<_float4x4> m_ManipulateMatrices = {};        //강제로 추가할 매트릭스
    vector<_float4x4> m_CombinedMatrices = {};          //부모로부터 업데이트됀 매트릭스
    vector<_float4x4> m_FinalMatices = {};              //월드행렬까지 곱해진 최종 매트릭스
    unordered_set<_uint> m_DettachedBone = {};

    _int m_iNextClipIndex = { -1 }; //다음 애니메이션 전환 용
    _float m_fConvertDuration = {}; 
    _float m_fPrevTrackPosition = {}; //다음 애니메이션 전환 용

    _int m_iCurrentClipIndex = { -1 };
    _float m_fCurrentTrackPosition = {};
    _bool isAnimEnd = { false };

    /*Blend*/
    _int m_iBlendAnimation = {-1};
    _float m_fBlendTrackPosition = {};
    _float m_fBlendConversionTrackPosition = {};
    _float m_fBlendDuration = {};
    _float m_fBlendWeight = {1.5f};
    vector<_uint> m_BlendIndex = {};
    vector<_float4x4> m_BlendTransfomationMatices = {};
    _bool isBlendAnimEnd = { false };
    

    /*Managing*/
    vector<_bool> m_pAnimLoops;
    unordered_map<string, _uint> m_pAnimNames;

public:
    static CAnimator3D* Create();
    virtual CComponent* Clone();
    virtual void Free() override;
};

class ENGINE_DLL SetAnimBuild {
public:
    SetAnimBuild(_int LayerIndex, _int ClipIndex, CAnimator3D* Owner)
        :m_iLayerIndex{ LayerIndex }, m_iClipIndex{ ClipIndex }, m_pOwner{ Owner } {}
    ~SetAnimBuild() { if (!bApplied) Apply(); };
    
    SetAnimBuild(const SetAnimBuild&) = delete;
    SetAnimBuild& operator=(const SetAnimBuild&) = delete;

public:
    virtual HRESULT Apply();

    //---------- 기본 속성
    
    //애니매이션을 반복재생할건지
    SetAnimBuild& Loop(_bool bLoop = false);
    //애니매이션을 재생속도
    SetAnimBuild& Speed(_float fSpeed = 1.f);

protected:
    CAnimator3D* m_pOwner = nullptr;
    _int m_iLayerIndex = -1;
    _int m_iClipIndex = -1;
    _bool bApplied = false;

    //---------- 기본 속성
    _bool m_bLoop = false;
    _float m_fSpeed = 1.f;

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
    ChangeAnimBuild& BlendState(_float fDuration = 0.2f, BLEND_STATE eBlendState = BLEND_STATE::CROSSFADE);

protected:
    _float      m_fBlendDuration = 0.2f;
    BLEND_STATE m_eBlendState = { BLEND_STATE::NONE };
};

NS_END