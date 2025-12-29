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
        //---------- ���̾� �Ӽ� (���̾� ��������)
        _bool               BaseLayer = { false };

        _bool               bPause = { true };
        ANIM_LAYER_STATE    eLayerType = { ANIM_LAYER_STATE::OVERRIDE };
        _int                iStartBoneIndex = { -1 };
        vector<_int>        AffectedBonesIndices;

        //��Ʈ�� ��Ÿ�� (���̽� ���̾, �������� �������� ����ϴ� ��)
        _bool               bWrapped = { false };
        _int                iRootBoneIndex = { -1 }; //��Ʈ �� 
        _float3             vRootEndPos{};  //�� Ŭ���� ���ϸ����� ��Ʈ��ġ
        _float4             vRootEndQuat{}; //�� Ŭ���� ���ϸ����� ��Ʈȸ����
        _float3             vPrevRootPos{}; //���� ������ ��ġ
        _float4             vPrevRootQuat{}; //���� ������ ȸ��
        _float3             vRootMoveDelta{};   //�̵���

        //��Ǻ� (�ִϸ��̼��� �������� ����ϴ� ��)
        _int    iMotionBoneIndex = { -1 };
        AXIS    eExtractAxis = { AXIS::NONE };
        _float3 vMotionEndPos{};        //�� Ŭ���� ���� ������ �����ġ
        _float3 vPrevMotionBonePos{};   //���� ������ ��ġ

        //---------- �ִϸ��̼� ������ (����� �ʱ�ȭ)
        _int    iClipIndex = { -1 };
        _float  fPrevTrackPosition = {};
        _float  fCurrentTrackPosition = {};
        _float  fAnimSpeed = { 1.f };
        _bool   bLoop = { false };
        _bool   bisFinished = { true };
        //��� �ӵ�����
        EaseType ePlayEaseType = { EaseType::None };
        _float  fTargetSpeed;
        _float  fEaseDuration = {};
        _float  fEaseElapsed = {};

        //���� ��Ʈ����
        vector<_float4x4> LocalMatrices = {};
 
        //---------- ������ ���� (����� �ʱ�ȭ)
        _bool   bBlending = { false };
        _bool   bKeepTrackPos = { false };
        _int    iNextClipIndex = { -1 };
        _float  fBlendTrackPosition = {};
        _float  fBlendElapsed = {};
        _float  fBlendDuration = {};
        EaseType eBlendEaseType = { EaseType::None };
             
        //���� ��Ʈ����
        vector<_float4x4> BlendMatrices = {};

        //������ ���� ���� ��Ʈ����
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
    HRESULT Resize_Layer(_uint iLayerCount); //���̾� ũ��(����) ���� //����resize�� ������ ��� 
    virtual void Update_Animation(_float dt);

public: 
    //��� �ִϸ��̼� ����
    class SetAnimBuild Set_Animation(AnimArg ClipArg);
    class SetAnimBuild Set_Animation(_uint LayerIndex, AnimArg ClipArg);
    //�ִϸ��̼� ���� ����
    class ChangeAnimBuild Change_Animation(AnimArg ClipArg);
    class ChangeAnimBuild Change_Animation(_uint LayerIndex, AnimArg ClipArg);
    //���̾� �ʱ�ȭ
    virtual void Reset_Layer(_uint LayerIndex);
    //���̾� �ִϸ��̼��� ���� (�ʱ�ȭ x)
    virtual HRESULT Stop_Animation(_uint LayerIndex);
    //��ü ���̾� �ִϸ��̼��� ���ߵ� �ٽ� �ѷ��� ���� ��� Ǯ���־����.
    virtual HRESULT StopAll_Animation(); 

public://�ִϸ����� ������
    /*----- is -----*/

    //���� ���̾��� �ִϸ��̼��� ��������
    _bool isCurrentAnimEnd(_uint LayerIndex = 0);
    //���� ���̾��� Ŭ���� 0~1������ ������ �ް�, �� ���� ������ �Ѿ����
    _bool isOverClipTiming(_float percent, _uint LayerIndex = 0);
    //���� ���̾��� �ִϸ��̼��� ������ ������
    _bool isBlending(_uint LayerIndex = 0);

    /*----- Getter -----*/

    //���� ���̾� Ŭ���� ����� 0~1 ��ȯ
    _float Get_CurAnimDuration(_uint LayerIndex = 0);
    //���� ���̾��� �ִϸ��̼� �̸�
    string Get_CurAnimName(_uint LayerIndex = 0);
    //���� ���̾��� �ִϸ��̼� �ε���
    _int Get_CurAnimIndex(_uint LayerIndex = 0);
    //���� ���̾� ����
    _int Get_NumLayer();
    //�̺�Ʈ�� �ҷ����� �Լ� (�� ������ ���� ���빰�� ������)
    const vector<EVENT_INST>& Get_EventBus() const;
    //"Root"��� �̸��� ���� ���� ������ ��Ÿ��
    _float3 Get_RootBoneMoveDelta() const;
    _float3 Get_RootBoneQuatDelta() const;
    //��Ǻ� �ִϸ��̼� ��Ÿ��
    _vector Get_MotionBoneDelta(_uint LayerIndex = 0);
    //���� ���̾��� Ease���̸� �� ����������
    _float Get_EaseDuration(_uint LayerIndex = 0);
    //���� ���̾� ����ӵ�
    _float Get_AnimSpeed(_uint LayerIndex = 0);
    //���� ��������
    _bool Get_isPause(_uint LayerIndex = 0);

    /*----- Setter -----*/
    
    //��Ǻ� ��������
    void Set_MotionBone(_int MoveBoneIndex);
    //�ִϸ��̼� �࿡ ������ ���� (ex : (AXIS::X | AXIS::Z) = XZ������)
    void Set_RemoveAxisFromMotionBone(AXIS eAxis);
    //�� ���� ����
    void Reset_ExtractBoneMovement();
    //�ִϸ��̼� ����
    void Set_Pause(_bool bPause, _uint LayerIndex = 0);
    //�ִϸ��̼��� ���� �� ����
    void Set_StartBone(_int StartBoneIndex, _uint LayerIndex = 0);
    void Reset_StartBone(_uint LayerIndex = 0);
    //�ִϸ��̼� Tpose�� ���� (�� �ִϸ��̼� ���̾� ���°� ���� ���ư�)
    void Set_TPose();

public:
    void Control_Bone(const string& boneName, _fmatrix BoneMatrix);
    void Control_BoneByIndex(_uint Index, _fmatrix BoneMatrix);
    void Dettach_BoneRelation(_uint Index);
    //�̺�Ʈ �߰�
    void Add_Event(CLIP_EVENT_TYPE EventType, string EventTag);
    void Clear_Events();

public:
    //������� ���� �� ��ġ�� ������
    _float4x4 Get_BoneMatrix(const string& boneName);
    _float4x4 Get_BoneMatrix(_uint Index);
    _float4x4* Get_BoneMatrixPtr(const string& boneName);
    _float4x4* Get_BoneTransformMatrixPtr(const string& boneName);
    const vector<_float4x4>& Get_BoneMatrices() { return m_CombinedMatrices; };
    vector<_float4x4> Get_BoneMatrices(_uint meshIndex);
    //���� �� ������ġ�� ������
    const vector<_float4x4>& Get_CombinedBoneMatrices() { return m_CombinedMatrices; };
    vector<_float4x4>* Get_CombinedBoneMatrices_Ptr() { return &m_CombinedMatrices; };

protected://�ִϸ��̼� üũ
    //���ڿ� �� ���ڸ� �ε����� �� �ٲ��ִ� �Լ�
    _int Resolve_ClipIndex(AnimArg ClipArg);
    //���̾� �ε����� ã��
    _int Find_Clip(const string& ClipTag);
    //�����ϴ��� ����
    _bool isExistLayer(_int LayerIndex);
    _bool isExistClip(_int ClipIndex);
    //���� �ִ��� ���
    _bool hasAxis(AXIS eExtractAxis, AXIS Axis);

protected:
    //�ִϸ��̼� ����
    void Animation_Run(ANIM_LAYER& Layer, _float dt);
    void Animation_Convert(ANIM_LAYER& Layer, _float dt);
    //���̾� ����
    void Layer_Override(const ANIM_LAYER& Layer);
    void Layer_Blend(const ANIM_LAYER& Layer);
    void Layer_Additive(const ANIM_LAYER& Layer);
    //Combined ����

    //���� �� ���
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
 
    vector<ANIM_LAYER>              m_AnimLayers;   //�ִϸ��̼� ���̾�
    vector<class CAnimationClip*>   m_pAnimClips;   //�ִϸ��̼� Ŭ��
    vector<EVENT_INST>              m_EventBus;     //�̺�Ʈ ����

    /* �Ʒ� 4���� ���� ����� ������ �ִϸ��̼��� ���ư�  */
    vector<_float4x4> m_TPose = {};                     //T-Pose Matrices
    vector<_float4x4> m_TransformationMatrices = {};    //�ִϸ��̼� Ŭ���� ������Ʈ�� ���� ��Ʈ����
    vector<_float4x4> m_ManipulateMatrices = {};        //������ �߰��� ��Ʈ����
    vector<_float4x4> m_CombinedMatrices = {};          //�θ�κ��� ������Ʈ�� ���� ��Ʈ����
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

// ������������������ Builder

template <typename T>
class AnimBuild {
public:
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

    T& Pause(_bool bPause) {
        m_bPause = bPause;
        return static_cast<T&>(*this);
    }
 
protected:
    _bool    m_bLoop = false;
    _bool    m_bPause = false;
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
    //---------- �ִϸ��̼� ������ �Ӽ�

    //�ִϸ��̼� ��ȯ�ð�
    ChangeAnimBuild& BlendDuration(_float fDuration) {
        m_fBlendDuration = fDuration;
        return *this;
    }
    //�ִϸ��̼� ��ȯ ����ġ ��¡
    ChangeAnimBuild& BlendWeightEaseType(EaseType eEaseType) {
        m_eBlendEaseType = eEaseType;
        return *this;
    }
    //�ִϸ��̼��� �����ϸ鼭 ���� Ŭ���� Ʈ���������� ���� ����ؼ� ��������
    ChangeAnimBuild& KeepTrackPos(_bool bKeepTrackPos) {
        m_bKeepTrackPos = bKeepTrackPos;
        return *this;
    }

protected:
    CAnimator3D* m_pOwner = nullptr;
    _int m_iLayerIndex = -1;
    _int m_iClipIndex = -1;
    _bool m_bApplied = false;

    _float      m_fBlendDuration = 0.2f;
    _bool       m_bKeepTrackPos = false;
    EaseType    m_eBlendEaseType = { EaseType::Linear };
};

NS_END