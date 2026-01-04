#pragma once

#include "Component.h"
#include "Engine_Math.h"
#include "AnimationLayout.h"

NS_BEGIN(Engine)

using AnimArg = variant<_int, string>;

enum class ANIM_LAYER_STATE { NONE, BASE, OVERRIDE, BLEND, ADDITIVE };

class ENGINE_DLL CAnimator3D : public CComponent
{
public:
    friend class SetAnimBuild;
    friend class ChangeAnimBuild;

    _float timeScale = 1.f;

    struct ANIM_LAYER
    {
        _bool baseLayer = false;
        ANIM_LAYER_STATE layerType = ANIM_LAYER_STATE::OVERRIDE;

        _bool paused = true;

        _float weight{};
        _float targetWeight{};
        _float weightElapsed{};
        _float weightDur{};
        EaseType weightEase = EaseType::None;

        _int startBoneIdx = -1;
        vector<_int> affectedBoneIdxs;

        _bool wrapped = false;
        _int rootBoneIdx = -1;

        _vector3 rootEndPos{};
        _vector4 rootEndQuat{_quaternion::Identity};

        _vector3 prevRootPos{};
        _vector4 prevRootQuat{_quaternion::Identity};

        _vector3 rootMoveDelta{};
        _vector4 rootQuatDelta{_quaternion::Identity};

        _int motionBoneIdx = -1;
        AXIS extractMoveAxis = AXIS::NONE;
        AXIS extractRotAxis = AXIS::NONE;

        _vector3 motionEndPos{};
        _vector3 prevMotionPos{};

        _int clipIdx = -1;

        _float prevTrackPos{};
        _float curTrackPos{};

        _float animSpeed = 1.f;
        _bool loop = false;
        _bool finished = true;

        EaseType playEase = EaseType::None;
        _float targetSpeed = 1.f;
        _float easeDur{};
        _float easeElapsed{};

        vector<_float4x4> localMats{};
        _bool blending = false;
        _bool keepTrackPos = false;
        _bool ignoreRotation = false;

        _int nextClipIdx = -1;
        _float blendTrackPos{};
        _float blendElapsed{};
        _float blendDur{};
        EaseType blendEase = EaseType::None;

        vector<_float4x4> blendMats{};
        vector<_float4x4> finalLocalMats{};
    };

    struct PLAYLIST_ENTRY
    {
        _int clipIdx = -1;
        _bool loop = false;
        _float speed = 1.f;
        _float blendDur = 0.2f;
    };

protected:
    CAnimator3D() {}
    CAnimator3D(const CAnimator3D& rhs);
    ~CAnimator3D() DEFAULT;

public:
    HRESULT Initialize_Prototype() override { return S_OK; }
    HRESULT Initialize(COMPONENT_DESC* pArg) override { return S_OK; }

public:
    void    LinkAnimate_Model(const string& levelKey, const string& modelKey);
    HRESULT Link_MetaData(const string& levelKey, const string& metaClipKey);
    HRESULT Resize_Layer(_uint layerCount);

public:
    virtual void Update_Animation(_float dt);
    void         Scrub_Timeline(_uint layerIdx);

public:
    class SetAnimBuild    Set_Animation(AnimArg clipArg);
    class SetAnimBuild    Set_Animation(_uint layerIdx, AnimArg clipArg);
    class ChangeAnimBuild Change_Animation(AnimArg clipArg);
    class ChangeAnimBuild Change_Animation(_uint layerIdx, AnimArg clipArg);

public:
    virtual void   Reset_Layer(_uint layerIdx);
    virtual HRESULT Stop_Animation(_uint layerIdx);
    virtual HRESULT StopAll_Animation();

public:
    _bool   isCurrentAnimEnd(_uint layerIdx = 0);
    _bool   isOverClipTiming(_float percent, _uint layerIdx = 0);
    _bool   isBlending(_uint layerIdx = 0);

    _float  Get_CurAnimDuration(_uint layerIdx = 0);
    string  Get_CurAnimName(_uint layerIdx = 0);
    _int    Get_CurAnimIndex(_uint layerIdx = 0);
    _int    Get_NumLayer();

    _float3 Get_RootBoneMoveDelta() const;
    _float4 Get_RootBoneQuatDelta() const;
    _float4 Get_RootBoneEndQuat() const;

    _vector Get_MotionBoneDelta(_uint layerIdx = 0);

    _float  Get_EaseDuration(_uint layerIdx = 0);
    _float  Get_AnimSpeed(_uint layerIdx = 0);
    _bool   Get_isPause(_uint layerIdx = 0);

public:
    void Set_MotionBone(_int motionBoneIdx);
    void Set_ExtractMotionboneMovement(AXIS axis);
    void Reset_ExtractBoneMovement();

    void Set_Pause(_bool paused, _uint layerIdx = 0);

    void Set_StartBone(_int startBoneIdx, _uint layerIdx = 0);
    void Reset_StartBone(_uint layerIdx = 0);

    void Set_TPose();
    void Set_LayerType(ANIM_LAYER_STATE layerType, _uint layerIdx = 0);

    void Chagne_Speed(_float speed, _uint layerIdx = 0);
    void Change_TransitionSpeed(_float targetSpeed, _float dur, EaseType ease = EaseType::Linear, _uint layerIdx = 0);

public:
    const vector<EVENT_INST>& Get_EventBus() const;
    _quaternion Calc_TransformFromEndAnim(const _vector4& transformQuat);

public:
    void Control_Bone(const string& boneName, _fmatrix boneMat);
    void Control_BoneByIndex(_uint idx, _fmatrix boneMat);
    void Dettach_BoneRelation(_uint idx) { dettachedBone.insert(idx); }

    void Add_Event(CLIP_EVENT_TYPE type, string tag) { eventBus.push_back(EVENT_INST{type, tag}); }
    void Clear_Events() { eventBus.clear(); }

public:
    _float4x4  Get_BoneMatrix(const string& boneName);
    _float4x4  Get_BoneMatrix(_uint idx);

    _float4x4* Get_BoneMatrixPtr(const string& boneName);
    _float4x4* Get_BoneTransformMatrixPtr(const string& boneName);

    const vector<_float4x4>& Get_BoneMatrices() { return combinedMats; }
    vector<_float4x4>        Get_BoneMatrices(_uint meshIdx);

    const vector<_float4x4>& Get_CombinedBoneMatrices() { return combinedMats; }
    vector<_float4x4>* Get_CombinedBoneMatrices_Ptr() { return &combinedMats; }

public:
    _bool Playlist_isPlaying() const { return playlistPlaying; }
    _int  Playlist_GetCurrentIndex() const { return playlistIdx; }
    const vector<PLAYLIST_ENTRY>& Playlist_GetEntries() const { return playlist; }

protected:
    _int  Resolve_ClipIndex(AnimArg clipArg);
    _int  Find_Clip(const string& clipTag);

    _bool isExistLayer(_int layerIdx);
    _bool isExistClip(_int clipIdx);

    _bool hasAxis(AXIS mask, AXIS axis);

    Matrix Calc_MatrixBlend(const _float4x4& base, const _float4x4& target, _float t);
    Matrix Calc_MatrixAdditive(const _float4x4& base, const _float4x4& target, const _float4x4& tPose, _float weight);

protected:
    void Animation_Run(ANIM_LAYER& layer, _float dt);
    void Animation_Convert(ANIM_LAYER& layer, _float dt);

    void Layer_Base(const ANIM_LAYER& layer);
    void Layer_Override(const ANIM_LAYER& layer);
    void Layer_Blend(const ANIM_LAYER& layer);
    void Layer_Additive(const ANIM_LAYER& layer);

    void BuildBone(_float dt);

public:
    virtual void Render_GUI();

protected:
    void GUI_ShowLayerInfo(_float height);
    void GUI_SelectAnim(_float height);

private:
    void Reset_Anim();

protected:
    class CModelData* data = {};
    int guiLayerIdx = 0;

    Matrix preTransform = {Matrix::Identity};

    vector<ANIM_LAYER> animLayers;
    vector<class CAnimationClip*> animClips;

    vector<EVENT_INST> eventBus;

    vector<_float4x4> tPose{};
    vector<_float4x4> basePose{};

    vector<_float4x4> transMats{};
    vector<_float4x4> manipulateMats{};
    vector<_float4x4> combinedMats{};

    unordered_set<_uint> dettachedBone{};

    _int curClipIdx = -1;

    vector<_bool> animLoops;
    unordered_map<string, _uint> animNames;

    vector<PLAYLIST_ENTRY> playlist;
    _int  playlistIdx = -1;

    _bool playlistPlaying = false;
    _bool playlistLoop = false;

    _bool forceTPose = false;

public:
    static CAnimator3D* Create();
    virtual CComponent* Clone() { return new CAnimator3D(*this); }
    virtual void Free() override;
};

template <typename T>
class AnimBuild
{
public:
    T& LayerBlend(_float w, _float targetW, _float dur, EaseType ease)
    {
        layerWeight = w;
        layerTargetWeight = targetW;
        layerWeightDur = dur;
        layerEase = ease;
        return static_cast<T&>(*this);
    }

    T& Pause(_bool paused)
    {
        layerPause = paused;
        return static_cast<T&>(*this);
    }

    T& Loop(_bool loop)
    {
        layerLoop = loop;
        return static_cast<T&>(*this);
    }

    T& Speed(_float speed)
    {
        layerSpeed = speed;
        return static_cast<T&>(*this);
    }

    T& TransitionSpeed(_float startSpeed, _float targetSpeed, _float dur, EaseType ease = EaseType::Linear)
    {
        layerSpeed = startSpeed;
        layerTargetSpeed = targetSpeed;
        easeDur = dur;
        playEase = ease;
        return static_cast<T&>(*this);
    }

    T& ResetRotation(_bool resetRot)
    {
        resetRotation = resetRot;
        return static_cast<T&>(*this);
    }

protected:
    _float   layerWeight = 0.f;
    _float   layerTargetWeight = 0.f;
    _float   layerWeightDur = 0.f;
    EaseType layerEase = EaseType::None;

    _bool layerLoop = false;
    _bool layerPause = false;

    _bool resetRotation = false;

    _float   layerSpeed = 1.f;
    EaseType playEase = EaseType::None;

    _float layerTargetSpeed = 1.f;
    _float easeDur = 0.f;
};

class ENGINE_DLL SetAnimBuild : public AnimBuild<SetAnimBuild>
{
public:
    SetAnimBuild(_int layerIdx, _int clipIdx, CAnimator3D* owner)
        : owner(owner), layerIdx(layerIdx), clipIdx(clipIdx) {}

    ~SetAnimBuild() DEFAULT;

    SetAnimBuild(const SetAnimBuild&) = delete;
    SetAnimBuild& operator=(const SetAnimBuild&) = delete;

public:
    HRESULT Apply();

protected:
    CAnimator3D* owner = nullptr;
    _int layerIdx = -1;
    _int clipIdx = -1;

    _bool applied = false;
};

class ENGINE_DLL ChangeAnimBuild : public AnimBuild<ChangeAnimBuild>
{
public:
    ChangeAnimBuild(_int layerIdx, _int clipIdx, CAnimator3D* owner)
        : owner(owner), layerIdx(layerIdx), clipIdx(clipIdx) {}

    ~ChangeAnimBuild() DEFAULT;

    ChangeAnimBuild(const ChangeAnimBuild&) = delete;
    ChangeAnimBuild& operator=(const ChangeAnimBuild&) = delete;

public:
    HRESULT Apply();

    ChangeAnimBuild& BlendDur(_float dur) { blendDur = dur; return *this; }
    ChangeAnimBuild& BlendEase(EaseType ease) { blendEase = ease; return *this; }
    ChangeAnimBuild& KeepTrackPos(_bool keep) { keepTrackPos = keep; return *this; }
    ChangeAnimBuild& IgnoreRotation(_bool ignore) { ignoreRotation = ignore; return *this; }

protected:
    CAnimator3D* owner{};
    _int layerIdx = -1;
    _int clipIdx = -1;

    _float blendDur = 0.2f;
    _bool  keepTrackPos = false;
    _bool  ignoreRotation = false;

    EaseType blendEase = EaseType::Linear;
};

NS_END