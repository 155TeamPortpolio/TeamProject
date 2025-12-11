#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CAnimator3D :
    public CComponent
{
protected:
    enum class ANIMATOR_STATE{IDLE, RUNNING, CONVERTING};

    enum class BLENDER_STATE{NONE, BLEND_IN, RUNNING, BLEND_OUT, BLEND_PAUSE};
    struct AnimConvert {
        _uint SrcClip;
        _uint DstClip;
        _float ConversionElapsedTime = {};
        _float ConversionDuration = {};
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
    HRESULT Add_AnimClips(const string& LevelKey, const string& AnimKey, const string& Subject,_bool Loop = false);

public:
    virtual void Update_Animation(_float dt);

    const vector<_float4x4>& Get_BoneMatrices() { return m_FinalMatices; };
    const vector<_float4x4>& Get_CombinedBoneMatrices() { return m_CombinedMatrices; };
    virtual void Change_Animation(_uint index, _float convertDuration = 0.2f);
    virtual HRESULT Change_Animation(string animName, _bool overrideSame = false,_float convertDuration = 0.2f);
    virtual HRESULT ForceChange_Animation(string animName, _bool overrideSame = false,_float convertDuration = 0.2f);
    virtual HRESULT Stop_Animation();

    virtual HRESULT Set_AnimationBlend(string animName, vector<_uint> blendIndex);
    virtual HRESULT Release_AnimationBlend(vector<_uint> blendIndex);
    virtual HRESULT Release_AnimationBlend();
    virtual HRESULT Stop_AnimationBlend();
    virtual HRESULT Restart_AnimationBlend();

    _bool isCurrentAnimEnd();
    _bool isOverAnimTiming(_float percent);
    string Get_CurrentAnimName();

public:
    void Control_Bone(const string& boneName, _fmatrix BoneMatrix);
    void Control_BoneByIndex(_uint Index, _fmatrix BoneMatrix);
    void Dettach_BoneRelation(_uint Index);

public:
    _float4x4 Get_BoneMatrix(const string& boneName);
    _float4x4 Get_BoneMatrix(_uint Index);
    _float4x4* Get_BoneMatrixPtr(const string& boneName);
    _float4x4* Get_BoneTransformMatrixPtr(const string& boneName);

protected:
    void Animation_Run(_float dt);
    void Animation_Convert(_float dt);


    void Blend_In(_float dt);
    void Blend_Run(_float dt);
    void Blend_Out(_float dt);
    void Blend_Pause(_float dt);
    void Blend_Convert(_float dt);
    void Override_BlendAnim();
    void BuildBone();

public:
    virtual void Render_GUI();

private:
    void Reset_Anim();
protected:
    class CModelData* m_pData = {};
    ANIMATOR_STATE m_eState = {};

    _int m_iNextClipIndex = { -1 }; //다음 애니메이션 전환 용
    _float m_fConvertDuration = {}; 
    _float m_fPrevTrackPosition = {}; //다음 애니메이션 전환 용

    _int m_iCurrentClipIndex = { -1 };
    _float m_fCurrentTrackPosition = {};
    _bool isAnimEnd = { false };

    vector<_float4x4> m_TransfromationMatrices = {};
    vector<_float4x4> m_ManipulateMatrices = {};
    vector<_float4x4> m_CombinedMatrices = {};
    vector<_float4x4> m_FinalMatices = {};
    unordered_set<_uint> m_DettachedBone = {};

    vector<class CAnimationClip*> m_pAnimClips;
    
    /*Blend*/
    _int m_iBlendAnimation = {-1};
    _float m_fBlendTrackPosition = {};
    _float m_fBlendConversionTrackPosition = {};
    _float m_fBlendDuration = {};
    _float m_fBlendWeight = {1.5f};
    vector<_uint> m_BlendIndex = {};
    vector<_float4x4> m_BlendTransfomationMatices = {};
    _bool isBlendAnimEnd = { false };
    BLENDER_STATE m_eBlendState;

    /*Managing*/
    vector<_bool> m_pAnimLoops;
    unordered_map<string, _uint> m_pAnimNames;

public:
    static CAnimator3D* Create();
    virtual CComponent* Clone();
    void Free() override;
};
NS_END
