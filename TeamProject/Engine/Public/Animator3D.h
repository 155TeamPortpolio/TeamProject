#pragma once
#include "AnimatorStruct.h"
#include "Helper_Func.h"
#include "AnimBuilder.h"
#include "Component.h"

NS_BEGIN(Engine)
using AnimArg = variant<_int, string>;
class SetAnimBuild;
class ChangeAnimBuild;

class ENGINE_DLL CAnimator3D :
    public CComponent
{
public:
    friend class SetAnimBuild;
    friend class ChangeAnimBuild;

    struct IK_CHAIN
    {
        class IIKSolver* pSolver;
        vector<_int>     BoneIndices;
        _vector3         vPoleVector;
        _float           fWeight;
        _bool            bEnabled;

        IK_CHAIN()
            : pSolver(nullptr)
            , vPoleVector(0.f, 0.f, 1.f)
            , fWeight(1.f)
            , bEnabled(true)
        {
        }
    };

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

    // ------------------------------------------
    _int   Get_AnimClipCount() const;
    string Get_AnimClipName(_uint clipIndex) const;
    _float Get_TimeSec();
    _float Get_DurationSec();
    void   Set_TimeSec(_float timeSec);


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
    void Change_Speed(_float fSpeed, _uint LayerIndex = 0);
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

    enum class BoneSpace { TRANSFORMATION, MANIPULATE, COMBINED , WORLD };//, FINAL };

    _float4x4 Get_BoneMatrix(BoneSpace eBoneSpace, AnimArg BoneArg);
    _float4x4* Get_BoneMatrixPtr(BoneSpace eBoneSpace, AnimArg BoneArg);
    _vector3 Get_BonePosition(BoneSpace eBoneSpace, AnimArg BoneArg);
    _quaternion Get_BoneQuaternion(BoneSpace eBoneSpace, AnimArg BoneArg);
    const vector<_float4x4>& Get_BoneMatrices(BoneSpace eBoneSpace);
    vector<_float4x4>* Get_BoneMatrices_Ptr(BoneSpace eBoneSpace);

    void Set_BoneMatrix(BoneSpace eBoneSpace, const _float4x4& Matrix, AnimArg BoneArg);
    void Set_BonePosition(BoneSpace eBoneSpace, _vector3 Position, AnimArg BoneArg);
    void Set_BoneQuaternion(BoneSpace eBoneSpace, _quaternion Quaternion, AnimArg BoneArg);

    //Exception
    const vector<_float4x4>& Get_TPose() { return m_TPose; };
    Matrix Get_OwnerWorldMatrix();

public: /* DynamicBone */
    class CDynamicBone* Get_DynamicBone_Ptr() { 
        if (nullptr == m_pDynamicBone) Link_DynamicBone();
        return m_pDynamicBone;
    };

    vector<_float4x4>& Get_DynamicBoneMatricesPtr() { return m_DynamicBoneMatrices; };
    void Reset_DynamicBoneMatrices();
    _bool isUsingDynamicBone() { return (m_pDynamicBone != nullptr); }
    void Delete_DB();

public: /* IKSolver */
    HRESULT Initialize_HumanoidRig();
    HRESULT Initialize_FootIK(void* pFootIKDesc = nullptr);
    HRESULT Add_IKChain(IIKSolver* pSolver, const vector<_int>& BoneIndices,
        _vector3 vPoleVector = _vector3(0.f, 0.f, 1.f));
    void    Set_IKChainEnabled(_uint iChainIndex, _bool bEnabled);
    void    Set_IKChainWeight(_uint iChainIndex, _float fWeight);
    void    Clear_IKChains();
    const HumanoidRigData& Get_HumanoidRig() const { return m_HumanoidRig; }

protected://애니매이션 체크
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
    Matrix Calc_MatrixAdditive(const _float4x4& base, const _float4x4& target, const _float4x4& ref,  _float weight);


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
    void BuildBone();
    void BuildDynamicBone();

public:
    virtual void Render_GUI();

protected:
    void GUI_ShowLayerInfo();
    void GUI_SelectAnim();

public:
    void Render_BonePoint(_vector3 vPos, _float dotSize, ImU32 color);

private: /* IKSolver */
    void Update_IK(_float dt);
    void Apply_IK(IK_CONTEXT& context);

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
    vector<_float4x4> m_DynamicBoneMatrices = {};       //다이나믹본 업데이트용 매트릭스
    vector<_float4x4> m_CombinedMatrices = {};          //부모로부터 업데이트됀 최종 매트릭스
    vector<_float4x4> m_FinalMatrices = {};          //부모로부터 업데이트됀 최종 매트릭스
    unordered_set<_uint> m_DettachedBone = {};


    /*Managing*/
    vector<_bool> m_pAnimLoops;
    unordered_map<string, _uint> m_pAnimNames;
    _int m_iCurrentClipIndex{};

    /* IKSolver */
    HumanoidRigData  m_HumanoidRig;
    vector<IK_CHAIN> m_IKChains;

    string m_animFilter;
public:
    static CAnimator3D* Create();
    virtual CComponent* Clone();
    virtual void Free() override;
};

NS_END