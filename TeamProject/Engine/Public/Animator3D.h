#pragma once
#include "Component.h"
#include "Engine_Math.h"

NS_BEGIN(Engine)
class ENGINE_DLL CAnimator3D :
    public CComponent
{
protected:
    enum class ANIM_LAYER_STATE {OVERRIDE, BLEND, ADDITIVE};
    enum class BLEND_STATE{ NONE, CROSSFADE, IMMEDIATE };

    struct AnimConvert {
        _uint SrcClip;
        _uint DstClip;
        _float ConversionElapsedTime = {};
        _float ConversionDuration = {};
    };

    typedef struct AnimationLayer {
        //---------- 레이어 속성
        ANIM_LAYER_STATE    eLayerType = { ANIM_LAYER_STATE::OVERRIDE };
        _int                iStartBoneIndex = { -1 };

        //---------- 애니매이션 데이터
        string  iClipTag{};
        _int    iClipIndex = { 0 };
        _float  fPrevTrackPosition = {};
        _float  fCurrentTrackPosition = {};
        _bool   bLoop = { false };
        _bool   bisFinish = { false };
        vector<_float4x4> LocalMatrices = {};

        // ───────── 블렌드 상태
        _bool   bBlending = { false };
        _int    iNextNodeIndex = { -1 };
        _float  fBlendElapsed = {};
        _float  fBlendDuration = {};
        BLEND_STATE     eBlendState = { BLEND_STATE::NONE };
        vector<_uint>   BlendBonesIndex;
        vector<_float4x4> BlendMatrices = {};

        // ───────── 로컬매트릭스
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

public:
    //clip
    virtual void Update_Animation(_float dt);
    virtual HRESULT Set_Animation(_uint LayerIndex, string ClipTag);
    virtual HRESULT Set_Animation(_uint LayerIndex, _uint Clipindex);
    virtual HRESULT Change_Animation(_uint LayerIndex, string ClipTag, _float convertDuration = 0.2f);
    virtual HRESULT Change_Animation(_uint LayerIndex, _uint Clipindex, _float convertDuration = 0.2f);
    virtual HRESULT ForceChange_Animation(_uint LayerIndex, string ClipTag, _bool overrideSame = false, _float convertDuration = 0.2f);
    virtual HRESULT ForceChange_Animation(_uint LayerIndex, _uint Clipindex, _bool overrideSame = false,_float convertDuration = 0.2f);
    
    virtual HRESULT Stop_Animation(_uint LayerIndex);
    virtual HRESULT StopAll_Animation(_uint LayerIndex);


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
    const vector<_float4x4>& Get_BoneMatrices() { return m_FinalMatices; };
    const vector<_float4x4>& Get_CombinedBoneMatrices() { return m_CombinedMatrices; };

protected:
    void Animation_Run(_float dt);
    void Animation_Convert(_float dt);

    void Override_BlendAnim();
    void BuildBone();

public:
    virtual void Render_GUI();

private:
    void Reset_Anim();
protected:
    class CModelData* m_pData = {};
    vector<class CAnimationClip*> m_pAnimClips;         //불러온 애니매이션클립들
    vector<ANIM_LAYER> m_AnimLayers;


    /* 아래 4개의 값만 제대로 들어오면 애니매이션이 돌아감  */
    vector<_float4x4> m_TransfromationMatrices = {};    //애니매이션 클립을 업데이트한 로컬 매트릭스
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
NS_END
