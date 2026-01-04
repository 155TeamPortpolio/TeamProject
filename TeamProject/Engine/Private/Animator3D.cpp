#include "Engine_Defines.h"
#include "Animator3D.h"
#include "AnimationClip.h"
#include "ModelData.h"
#include "GameInstance.h"
#include "IResourceService.h"

namespace
{
    string ExtractCoreAnimName(const string& fullName)
    {
        size_t pos = fullName.find("_Ani_");
        if (pos != string::npos) return fullName.substr(pos + 5);
        return fullName;
    }

    bool IsAttackAnim(const string& coreName)
    {
        return coreName.find("Attack") != string::npos;
    }
}

CAnimator3D::CAnimator3D(const CAnimator3D& rhs) : CComponent(rhs),
    animClips(rhs.animClips),
    data(rhs.data),
    transMats(rhs.transMats),
    combinedMats(rhs.combinedMats)
{
    for (auto& clip : animClips) Safe_AddRef(clip);
    Safe_AddRef(data);
}

void CAnimator3D::LinkAnimate_Model(const string& levelKey, const string& modelKey)
{
    if (data) Reset_Anim();

    data = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(levelKey, modelKey);
    Safe_AddRef(data);

    _float4x4 identity;
    XMStoreFloat4x4(&identity, XMMatrixIdentity());

    transMats.resize(data->Get_BoneCount(), identity);
    manipulateMats.resize(data->Get_BoneCount(), identity);
    combinedMats.resize(data->Get_BoneCount(), identity);

    for (size_t i = 0; i < data->Get_BoneCount(); i++)
        transMats[i] = data->Get_TransformMatrix(i);

    for (size_t i = 0; i < data->Get_BoneCount(); i++)
    {
        int parent = data->Get_BoneParentIndex(i);
        if (parent == -1) combinedMats[i] = transMats[i];
        else
        {
            _matrix parentCombine = XMLoadFloat4x4(&combinedMats[parent]);
            _matrix myTrans = XMLoadFloat4x4(&transMats[i]);
            XMStoreFloat4x4(&combinedMats[i], myTrans * parentCombine);
        }
    }

    tPose = combinedMats;
    basePose.resize(data->Get_BoneCount(), identity);
}

HRESULT CAnimator3D::Link_MetaData(const string& levelKey, const string& metaClipKey)
{
    ANIMATION_META meta = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_MetaClip(levelKey, metaClipKey);

    if (meta.pClips.empty())
    {
        string msg = "Anim Add Failed: " + metaClipKey + "\n";
        OutputDebugStringA(msg.c_str());
        return E_FAIL;
    }

    preTransform = meta.PreTransform;

    animClips = meta.pClips;
    for (auto& clip : meta.pClips) Safe_AddRef(clip);

    Resize_Layer(1);

    animLayers[0].baseLayer = true;
    animLayers[0].layerType = ANIM_LAYER_STATE::BASE;
    animLayers[0].weight = 1.f;
    animLayers[0].rootBoneIdx = data->Find_BoneIndexByName("Root");

    return S_OK;
}

HRESULT CAnimator3D::Resize_Layer(_uint layerCount)
{
    if (layerCount <= animLayers.size()) return S_OK;

    _float4x4 identity;
    XMStoreFloat4x4(&identity, XMMatrixIdentity());

    while (animLayers.size() < layerCount)
    {
        ANIM_LAYER layer{};
        layer.localMats.resize(data->Get_BoneCount(), identity);
        layer.blendMats.resize(data->Get_BoneCount(), identity);
        layer.finalLocalMats.resize(data->Get_BoneCount(), identity);
        animLayers.push_back(layer);
    }

    return S_OK;
}

void CAnimator3D::Update_Animation(_float dt)
{
    if (animClips.empty()) return;

    dt *= timeScale;

    Clear_Events();

    for (auto& layer : animLayers)
    {
        if (layer.paused) continue;
        if (layer.weight <= 0.f) continue;

        if (layer.blending) Animation_Convert(layer, dt);
        else Animation_Run(layer, dt);
    }

    BuildBone(dt);
}

SetAnimBuild CAnimator3D::Set_Animation(AnimArg clipArg)
{
    return SetAnimBuild(0, Resolve_ClipIndex(clipArg), this);
}

SetAnimBuild CAnimator3D::Set_Animation(_uint layerIdx, AnimArg clipArg)
{
    return SetAnimBuild(layerIdx, Resolve_ClipIndex(clipArg), this);
}

ChangeAnimBuild CAnimator3D::Change_Animation(AnimArg clipArg)
{
    return ChangeAnimBuild(0, Resolve_ClipIndex(clipArg), this);
}

ChangeAnimBuild CAnimator3D::Change_Animation(_uint layerIdx, AnimArg clipArg)
{
    return ChangeAnimBuild(layerIdx, Resolve_ClipIndex(clipArg), this);
}

void CAnimator3D::Reset_Layer(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return;

    ANIM_LAYER& layer = animLayers[layerIdx];

    layer.startBoneIdx = -1;
    layer.affectedBoneIdxs.clear();

    layer.clipIdx = -1;
    layer.prevTrackPos = 0.f;
    layer.curTrackPos = 0.f;

    layer.playEase = EaseType::None;
    layer.loop = false;
    layer.finished = true;

    layer.blending = false;
    layer.nextClipIdx = -1;

    layer.blendElapsed = 0.f;
    layer.blendDur = 0.f;
    layer.blendEase = EaseType::None;

    _float4x4 identity;
    XMStoreFloat4x4(&identity, XMMatrixIdentity());

    layer.localMats.resize(data->Get_BoneCount(), identity);
    layer.blendMats.resize(data->Get_BoneCount(), identity);
    layer.finalLocalMats.resize(data->Get_BoneCount(), identity);
}

HRESULT CAnimator3D::Stop_Animation(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return E_FAIL;
    animLayers[layerIdx].paused = true;
    return S_OK;
}

HRESULT CAnimator3D::StopAll_Animation()
{
    for (auto& layer : animLayers) layer.paused = true;
    return S_OK;
}

_bool CAnimator3D::isCurrentAnimEnd(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return true;

    ANIM_LAYER& layer = animLayers[layerIdx];

    if (!isExistClip(layer.clipIdx)) return true;
    if (layer.blending) return false;
    if (layer.loop) return false;

    return layer.finished;
}

_bool CAnimator3D::isOverClipTiming(_float percent, _uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return true;

    ANIM_LAYER& layer = animLayers[layerIdx];

    if (layer.blending)
    {
        if (!isExistClip(layer.nextClipIdx)) return true;
        auto& nextClip = animClips[layer.nextClipIdx];
        _float threshold = nextClip->Get_Duration() * percent;
        return layer.blendTrackPos >= threshold;
    }

    if (!isExistClip(layer.clipIdx)) return true;

    auto& nowClip = animClips[layer.clipIdx];
    _float threshold = nowClip->Get_Duration() * percent;
    return layer.curTrackPos >= threshold;
}

_bool CAnimator3D::isBlending(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return false;
    return animLayers[layerIdx].blending;
}

_float CAnimator3D::Get_CurAnimDuration(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return 0.f;

    ANIM_LAYER& layer = animLayers[layerIdx];

    if (layer.blending)
    {
        if (!isExistClip(layer.nextClipIdx)) return 0.f;
        auto& nextClip = animClips[layer.nextClipIdx];
        return layer.blendTrackPos / nextClip->Get_Duration();
    }

    if (!isExistClip(layer.clipIdx)) return 0.f;

    auto& nowClip = animClips[layer.clipIdx];
    return layer.curTrackPos / nowClip->Get_Duration();
}

string CAnimator3D::Get_CurAnimName(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return "";

    ANIM_LAYER& layer = animLayers[layerIdx];

    if (layer.blending)
    {
        if (isExistClip(layer.nextClipIdx)) return animClips[layer.nextClipIdx]->Get_Name();
        return "";
    }

    if (isExistClip(layer.clipIdx)) return animClips[layer.clipIdx]->Get_Name();
    return "";
}

_int CAnimator3D::Get_CurAnimIndex(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return -1;
    return animLayers[layerIdx].blending ? animLayers[layerIdx].nextClipIdx : animLayers[layerIdx].clipIdx;
}

_int CAnimator3D::Get_NumLayer()
{
    return (_int)animLayers.size();
}

const vector<EVENT_INST>& CAnimator3D::Get_EventBus() const
{
    return eventBus;
}

_float3 CAnimator3D::Get_RootBoneMoveDelta() const
{
    for (auto& layer : animLayers)
        if (layer.baseLayer) return layer.rootMoveDelta;
    return _float3();
}

_float4 CAnimator3D::Get_RootBoneQuatDelta() const
{
    for (auto& layer : animLayers)
        if (layer.baseLayer) return layer.rootQuatDelta;
    return Quaternion::Identity;
}

_float4 CAnimator3D::Get_RootBoneEndQuat() const
{
    for (auto& layer : animLayers)
        if (layer.baseLayer) return layer.rootEndQuat;
    return Quaternion::Identity;
}

_vector CAnimator3D::Get_MotionBoneDelta(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return XMVectorZero();
    if (animLayers[layerIdx].motionBoneIdx == -1) return XMVectorZero();

    _float4x4 moveMat = animLayers[layerIdx].localMats[animLayers[layerIdx].motionBoneIdx];
    _float3 curPos = {moveMat._41, moveMat._42, moveMat._43};

    return XMLoadFloat3(&curPos) - XMLoadFloat3(&animLayers[layerIdx].prevMotionPos);
}

_float CAnimator3D::Get_EaseDuration(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return 0.f;

    auto& layer = animLayers[layerIdx];

    if (layer.playEase == EaseType::None) return 0.f;
    return layer.easeElapsed / layer.easeDur;
}

_float CAnimator3D::Get_AnimSpeed(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return 0.f;
    return animLayers[layerIdx].animSpeed;
}

_bool CAnimator3D::Get_isPause(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return true;
    return animLayers[layerIdx].paused;
}

void CAnimator3D::Set_MotionBone(_int motionBoneIdx)
{
    for (auto& layer : animLayers)
        if (layer.baseLayer) layer.motionBoneIdx = motionBoneIdx;
}

void CAnimator3D::Set_ExtractMotionboneMovement(AXIS axis)
{
    for (auto& layer : animLayers)
        if (layer.baseLayer) layer.extractMoveAxis = axis;
}

void CAnimator3D::Reset_ExtractBoneMovement()
{
    for (auto& layer : animLayers)
        if (layer.baseLayer) layer.extractMoveAxis = AXIS::NONE;
}

void CAnimator3D::Set_Pause(_bool paused, _uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return;
    animLayers[layerIdx].paused = paused;
}

void CAnimator3D::Set_StartBone(_int startBoneIdx, _uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return;

    animLayers[layerIdx].startBoneIdx = startBoneIdx;
    data->Get_AffectBoneIndices(animLayers[layerIdx].affectedBoneIdxs, animLayers[layerIdx].startBoneIdx);
}

void CAnimator3D::Reset_StartBone(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return;

    animLayers[layerIdx].startBoneIdx = -1;
    animLayers[layerIdx].affectedBoneIdxs.clear();
}

void CAnimator3D::Set_TPose()
{
    for (_uint i = 0; i < animLayers.size(); i++)
    {
        Reset_Layer(i);
        if (0 < i) animLayers[i].layerType = ANIM_LAYER_STATE::NONE;
    }

    combinedMats = tPose;
}

void CAnimator3D::Set_LayerType(ANIM_LAYER_STATE layerType, _uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return;
    if (animLayers[layerIdx].baseLayer) return;

    animLayers[layerIdx].layerType = layerType;
}

void CAnimator3D::Chagne_Speed(_float speed, _uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return;
    animLayers[layerIdx].animSpeed = speed;
}

void CAnimator3D::Change_TransitionSpeed(_float targetSpeed, _float dur, EaseType ease, _uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return;

    animLayers[layerIdx].targetSpeed = targetSpeed;
    animLayers[layerIdx].easeDur = dur;
    animLayers[layerIdx].playEase = ease;
}

_quaternion CAnimator3D::Calc_TransformFromEndAnim(const _vector4& transformQuat)
{
    _quaternion endQ = Get_RootBoneEndQuat();
    _quaternion transQ = transformQuat;

    endQ.Normalize();
    transQ.Normalize();

    return endQ * transQ;
}

void CAnimator3D::Control_Bone(const string& boneName, _fmatrix boneMat)
{
    _int idx = data->Find_BoneIndexByName(boneName);
    if (idx == -1) return;

    XMStoreFloat4x4(&manipulateMats[idx], boneMat);
}

void CAnimator3D::Control_BoneByIndex(_uint idx, _fmatrix boneMat)
{
    if (idx >= manipulateMats.size()) return;
    XMStoreFloat4x4(&manipulateMats[idx], boneMat);
}

_float4x4 CAnimator3D::Get_BoneMatrix(const string& boneName)
{
    _int idx = data->Find_BoneIndexByName(boneName);
    if (idx == -1) return _float4x4{};
    return combinedMats[idx];
}

vector<_float4x4> CAnimator3D::Get_BoneMatrices(_uint meshIdx)
{
    vector<_float4x4> out;
    out.reserve(combinedMats.size());

    for (size_t i = 0; i < combinedMats.size(); ++i)
    {
        _smatrix finalMat = combinedMats[i];
        _smatrix offset = data->Get_Offset(meshIdx, i);
        out.push_back(offset * finalMat);
    }

    return out;
}

_float4x4 CAnimator3D::Get_BoneMatrix(_uint idx)
{
    if (idx >= combinedMats.size()) return _float4x4{};
    return combinedMats[idx];
}

_float4x4* CAnimator3D::Get_BoneMatrixPtr(const string& boneName)
{
    _int idx = data->Find_BoneIndexByName(boneName);
    if (idx == -1) return nullptr;
    return &combinedMats[idx];
}

_float4x4* CAnimator3D::Get_BoneTransformMatrixPtr(const string& boneName)
{
    _int idx = data->Find_BoneIndexByName(boneName);
    if (idx == -1) return nullptr;
    return &transMats[idx];
}

_int CAnimator3D::Resolve_ClipIndex(AnimArg clipArg)
{
    if (holds_alternative<_int>(clipArg)) return get<_int>(clipArg);
    return Find_Clip(get<string>(clipArg));
}

_int CAnimator3D::Find_Clip(const string& clipTag)
{
    _int idx = 0;
    for (auto& clip : animClips)
    {
        if (clipTag == clip->Get_Name()) return idx;
        ++idx;
    }
    return -1;
}

_bool CAnimator3D::isExistLayer(_int layerIdx)
{
    return 0 <= layerIdx && layerIdx < (_int)animLayers.size();
}

_bool CAnimator3D::isExistClip(_int clipIdx)
{
    return 0 <= clipIdx && clipIdx < (_int)animClips.size();
}

_bool CAnimator3D::hasAxis(AXIS mask, AXIS axis)
{
    return (mask & axis) != AXIS::NONE;
}

Matrix CAnimator3D::Calc_MatrixBlend(const _float4x4& base, const _float4x4& target, _float t)
{
    Matrix baseMat = base;
    Matrix targetMat = target;

    _vector3 baseS, targetS;
    _quaternion baseR, targetR;
    _vector3 baseT, targetT;

    baseMat.Decompose(baseS, baseR, baseT);
    targetMat.Decompose(targetS, targetR, targetT);

    _vector3 s = _vector3::Lerp(baseS, targetS, t);
    _quaternion r = _quaternion::Slerp(baseR, targetR, t);
    r.Normalize();
    _vector3 tr = _vector3::Lerp(baseT, targetT, t);

    return XMMatrixAffineTransformation(s, XMVectorZero(), r, tr);
}

Matrix CAnimator3D::Calc_MatrixAdditive(const _float4x4& base, const _float4x4& target, const _float4x4& tPose, _float weight)
{
    Vector3 baseS, targetS, refS;
    Quaternion baseR, targetR, refR;
    Vector3 baseT, targetT, refT;

    Matrix(base).Decompose(baseS, baseR, baseT);
    Matrix(target).Decompose(targetS, targetR, targetT);
    Matrix(tPose).Decompose(refS, refR, refT);

    Vector3 deltaT = targetT - refT;

    refR.Inverse(refR);
    Quaternion deltaR = refR * targetR;

    Vector3 deltaS = targetS - refS;

    Vector3 outS = baseS;
    Quaternion outR = baseR * Quaternion::Slerp(Quaternion::Identity, deltaR, weight);
    outR.Normalize();
    Vector3 outT = baseT;

    return XMMatrixAffineTransformation(outS, XMVectorZero(), outR, outT);
}

void CAnimator3D::Animation_Run(ANIM_LAYER& layer, _float dt)
{
    if (layer.clipIdx == -1) return;

    auto& nowClip = animClips[layer.clipIdx];

    _float animSpeed = layer.animSpeed;

    if (layer.playEase != EaseType::None)
    {
        layer.easeElapsed += dt;

        _float t = min(layer.easeElapsed / layer.easeDur, 1.f);
        _float eased = Math::ApplyEase(layer.playEase, t);

        animSpeed = Math::Lerp(layer.animSpeed, layer.targetSpeed, eased);

        if (1.f <= t)
        {
            layer.animSpeed = animSpeed;
            layer.playEase = EaseType::None;
        }
    }

    _float playSpeed = dt * animSpeed;

    layer.curTrackPos = nowClip->TranslateAnimateMatrix(
        layer.localMats,
        layer.curTrackPos,
        playSpeed,
        layer.loop,
        &layer.wrapped,
        &layer.finished,
        eventBus
    );

    if (!layer.baseLayer) return;

    if (layer.rootBoneIdx != -1 && layer.motionBoneIdx != -1)
    {
        Matrix rootMat = layer.localMats[layer.rootBoneIdx];
        Matrix motionMat = layer.localMats[layer.motionBoneIdx];

        _vector s, r, tr;
        XMMatrixDecompose(&s, &r, &tr, rootMat);

        _vector3 curRootPos = tr;
        _vector4 curRootQuat = r;

        if (layer.wrapped)
        {
            _vector3 startPos = animClips[layer.clipIdx]->Get_StartKeyFrameByBoneIndex(layer.rootBoneIdx).vTranslation;
            layer.rootMoveDelta = (layer.rootEndPos - layer.prevRootPos) + (curRootPos - startPos);
            layer.wrapped = false;
        }
        else
        {
            layer.rootMoveDelta = curRootPos - layer.prevRootPos;

            motionMat.Translation(motionMat.Translation() - curRootPos);

            _vector invCurRootQuat = XMQuaternionInverse(curRootQuat);
            _matrix invCurRootRot = XMMatrixRotationQuaternion(invCurRootQuat);

            motionMat = motionMat * invCurRootRot;
            layer.localMats[layer.motionBoneIdx] = motionMat;
        }

        _vector quatDeltaLocal = XMQuaternionNormalize(XMQuaternionMultiply(curRootQuat, XMQuaternionInverse(layer.prevRootQuat)));

        _vector pos = XMVectorSet(layer.rootMoveDelta.x, layer.rootMoveDelta.y, layer.rootMoveDelta.z, 1.0f);
        _vector posT = XMVector4Transform(pos, preTransform);

        Vector3 posOut;
        posOut.x = XMVectorGetX(posT);
        posOut.y = XMVectorGetY(posT);
        posOut.z = XMVectorGetZ(posT);

        layer.rootMoveDelta = posOut;

        _matrix pRot = preTransform;
        pRot.r[3] = XMVectorSet(0, 0, 0, 1);

        _vector qP = XMQuaternionRotationMatrix(pRot);

        _vector quatDeltaOut = XMQuaternionNormalize(
            XMQuaternionMultiply(qP, XMQuaternionMultiply(quatDeltaLocal, XMQuaternionInverse(qP)))
        );

        XMStoreFloat4(&layer.rootQuatDelta, quatDeltaOut);

        layer.prevRootPos = curRootPos;
        layer.prevRootQuat = curRootQuat;
    }

    if (layer.motionBoneIdx != -1)
    {
        _float4x4& mat = layer.localMats[layer.motionBoneIdx];

        layer.prevMotionPos = _vector3(mat._41, mat._42, mat._43);

        if (hasAxis(layer.extractMoveAxis, AXIS::X)) mat._41 = 0.f;
        if (hasAxis(layer.extractMoveAxis, AXIS::Y)) mat._42 = 0.f;
        if (hasAxis(layer.extractMoveAxis, AXIS::Z)) mat._43 = 0.f;
    }
}

void CAnimator3D::Animation_Convert(ANIM_LAYER& layer, _float dt)
{
    if (layer.clipIdx == -1) return;

    auto& nowClip = animClips[layer.clipIdx];
    auto& nextClip = animClips[layer.nextClipIdx];

    _float animSpeed = layer.animSpeed;

    if (layer.playEase != EaseType::None)
    {
        layer.easeElapsed += dt;

        _float t = min(layer.easeElapsed / layer.easeDur, 1.f);
        _float eased = Math::ApplyEase(layer.playEase, t);

        animSpeed = Math::Lerp(layer.animSpeed, layer.targetSpeed, eased);

        if (1.f <= t)
        {
            layer.animSpeed = animSpeed;
            layer.playEase = EaseType::None;
        }
    }

    _float playSpeed = dt * animSpeed;

    if (layer.keepTrackPos)
    {
        layer.curTrackPos = nowClip->TranslateAnimateMatrix(
            layer.localMats,
            layer.curTrackPos,
            playSpeed,
            layer.loop,
            &layer.wrapped,
            &layer.finished,
            eventBus
        );
    }

    layer.blendTrackPos = nextClip->TranslateAnimateMatrix(
        layer.blendMats,
        layer.blendTrackPos,
        playSpeed,
        layer.loop,
        &layer.wrapped,
        &layer.finished,
        eventBus
    );

    if (layer.baseLayer)
    {
        if (layer.rootBoneIdx != -1 && layer.motionBoneIdx != -1)
        {
            Matrix rootMat = layer.blendMats[layer.rootBoneIdx];
            Matrix motionMat = layer.blendMats[layer.motionBoneIdx];

            _vector s, r, tr;
            XMMatrixDecompose(&s, &r, &tr, rootMat);

            Vector3 curRootPos = tr;
            Vector4 curRootQuat = r;

            if (layer.wrapped)
            {
                _vector3 startPos = animClips[layer.clipIdx]->Get_StartKeyFrameByBoneIndex(layer.rootBoneIdx).vTranslation;
                layer.rootMoveDelta = (layer.rootEndPos - layer.prevRootPos) + (curRootPos - startPos);
                layer.wrapped = false;
            }
            else
            {
                layer.rootMoveDelta = curRootPos - layer.prevRootPos;

                motionMat.Translation(motionMat.Translation() - curRootPos);

                _vector invCurRootQuat = XMQuaternionInverse(curRootQuat);
                _matrix invCurRootRot = XMMatrixRotationQuaternion(invCurRootQuat);

                motionMat = motionMat * invCurRootRot;
                layer.blendMats[layer.motionBoneIdx] = motionMat;
            }

            _vector quatDeltaLocal = XMQuaternionNormalize(XMQuaternionMultiply(curRootQuat, XMQuaternionInverse(layer.prevRootQuat)));

            _vector pos = XMVectorSet(layer.rootMoveDelta.x, layer.rootMoveDelta.y, layer.rootMoveDelta.z, 1.0f);
            _vector posT = XMVector4Transform(pos, preTransform);

            Vector3 posOut;
            posOut.x = XMVectorGetX(posT);
            posOut.y = XMVectorGetY(posT);
            posOut.z = XMVectorGetZ(posT);

            layer.rootMoveDelta = posOut;

            _matrix pRot = preTransform;
            pRot.r[3] = XMVectorSet(0, 0, 0, 1);

            _vector qP = XMQuaternionRotationMatrix(pRot);

            _vector quatDeltaOut = XMQuaternionNormalize(
                XMQuaternionMultiply(qP, XMQuaternionMultiply(quatDeltaLocal, XMQuaternionInverse(qP)))
            );

            XMStoreFloat4(&layer.rootQuatDelta, quatDeltaOut);

            layer.prevRootPos = curRootPos;
            layer.prevRootQuat = curRootQuat;
        }

        if (layer.motionBoneIdx != -1)
        {
            _float4x4& mat = layer.blendMats[layer.motionBoneIdx];

            layer.prevMotionPos = _float3(mat._41, mat._42, mat._43);

            if (hasAxis(layer.extractMoveAxis, AXIS::X)) mat._41 = 0.f;
            if (hasAxis(layer.extractMoveAxis, AXIS::Y)) mat._42 = 0.f;
            if (hasAxis(layer.extractMoveAxis, AXIS::Z)) mat._43 = 0.f;
        }
    }

    layer.blendElapsed += dt;

    _float t = layer.blendElapsed / layer.blendDur;
    _float blendRate = Math::ApplyEase(layer.blendEase, t);

    for (_uint i = 0; i < data->Get_BoneCount(); ++i)
        layer.finalLocalMats[i] = Calc_MatrixBlend(layer.localMats[i], layer.blendMats[i], blendRate);

    if (layer.blendDur < layer.blendElapsed)
    {
        layer.blending = false;
        layer.keepTrackPos = false;
        layer.ignoreRotation = false;

        layer.clipIdx = layer.nextClipIdx;
        layer.nextClipIdx = -1;

        layer.curTrackPos = layer.blendTrackPos;

        layer.blendElapsed = 0.f;
        layer.blendDur = 0.f;

        layer.localMats = layer.finalLocalMats;
    }
}

void CAnimator3D::Layer_Base(const ANIM_LAYER& layer)
{
    if (layer.blending) transMats = layer.finalLocalMats;
    else transMats = layer.localMats;
}

void CAnimator3D::Layer_Override(const ANIM_LAYER& layer)
{
    if (layer.startBoneIdx == -1)
    {
        if (layer.blending) transMats = layer.finalLocalMats;
        else transMats = layer.localMats;
        return;
    }

    for (_int boneIdx : layer.affectedBoneIdxs)
    {
        if (boneIdx == layer.motionBoneIdx || boneIdx == layer.rootBoneIdx) continue;

        if (layer.blending) transMats[boneIdx] = layer.finalLocalMats[boneIdx];
        else transMats[boneIdx] = layer.localMats[boneIdx];
    }
}

void CAnimator3D::Layer_Blend(const ANIM_LAYER& layer)
{
    auto ApplyBlend = [&](int boneIdx, const vector<_float4x4>& src)
        {
            transMats[boneIdx] = Calc_MatrixBlend(transMats[boneIdx], src[boneIdx], layer.weight);
        };

    if (layer.startBoneIdx == -1)
    {
        for (int i = 0; i < data->Get_BoneCount(); i++)
        {
            if (i == layer.motionBoneIdx || i == layer.rootBoneIdx) continue;
            if (layer.blending) ApplyBlend(i, layer.finalLocalMats);
            else ApplyBlend(i, layer.localMats);
        }
        return;
    }

    for (_int boneIdx : layer.affectedBoneIdxs)
    {
        if (boneIdx == layer.motionBoneIdx || boneIdx == layer.rootBoneIdx) continue;
        if (layer.blending) ApplyBlend(boneIdx, layer.finalLocalMats);
        else ApplyBlend(boneIdx, layer.localMats);
    }
}

void CAnimator3D::Layer_Additive(const ANIM_LAYER& layer)
{
    auto ApplyAdd = [&](int boneIdx, const vector<_float4x4>& src)
        {
            transMats[boneIdx] = Calc_MatrixAdditive(transMats[boneIdx], src[boneIdx], basePose[boneIdx], layer.weight);
        };

    if (layer.startBoneIdx == -1)
    {
        for (int i = 0; i < data->Get_BoneCount(); i++)
        {
            if (i == layer.motionBoneIdx || i == layer.rootBoneIdx) continue;
            if (layer.blending) ApplyAdd(i, layer.finalLocalMats);
            else ApplyAdd(i, layer.localMats);
        }
        return;
    }

    for (_int boneIdx : layer.affectedBoneIdxs)
    {
        if (boneIdx == layer.motionBoneIdx || boneIdx == layer.rootBoneIdx) continue;
        if (layer.blending) ApplyAdd(boneIdx, layer.finalLocalMats);
        else ApplyAdd(boneIdx, layer.localMats);
    }
}

void CAnimator3D::BuildBone(_float dt)
{
    for (auto& layer : animLayers)
    {
        if (layer.weight <= 0.f) continue;

        if (layer.weightEase != EaseType::None)
        {
            layer.weightElapsed += dt;

            _float t = min(layer.weightElapsed / layer.weightDur, 1.f);
            _float eased = Math::ApplyEase(layer.weightEase, t);

            layer.weight = Math::Lerp(layer.weight, layer.targetWeight, eased);
        }

        switch (layer.layerType)
        {
        case ANIM_LAYER_STATE::NONE: break;
        case ANIM_LAYER_STATE::BASE: Layer_Base(layer); break;
        case ANIM_LAYER_STATE::OVERRIDE: Layer_Override(layer); break;
        case ANIM_LAYER_STATE::BLEND: Layer_Blend(layer); break;
        case ANIM_LAYER_STATE::ADDITIVE: Layer_Additive(layer); break;
        default: break;
        }
    }

    for (size_t i = 0; i < data->Get_BoneCount(); i++)
    {
        int parent = data->Get_BoneParentIndex(i);

        if (parent == -1)
        {
            _matrix my = XMLoadFloat4x4(&manipulateMats[i]) * XMLoadFloat4x4(&transMats[i]) * XMLoadFloat4x4(&preTransform);

            XMStoreFloat4x4(&combinedMats[i], my);
        }
        else if (dettachedBone.count(i))
        {
            _matrix my = XMLoadFloat4x4(&manipulateMats[i]) * XMLoadFloat4x4(&transMats[i]);
            XMStoreFloat4x4(&combinedMats[i], my);
        }
        else
        {
            _matrix parentCombine = XMLoadFloat4x4(&combinedMats[parent]);
            _matrix my = XMLoadFloat4x4(&manipulateMats[i]) * XMLoadFloat4x4(&transMats[i]);
            XMStoreFloat4x4(&combinedMats[i], my * parentCombine);
        }
    }
}

void CAnimator3D::Render_GUI()
{
    ImGui::SeparatorText("Animator 3D");

    GUI_ShowLayerInfo(160.f);
    GUI_SelectAnim(800.f);
}

void CAnimator3D::GUI_ShowLayerInfo(_float height)
{
    ImGui::BeginChild("##Animator Layer", ImVec2(0.f, height), true);

    ImGui::Text("Layer");
    ImGui::SameLine();

    int layerCount = (int)animLayers.size();

    ImGui::SetNextItemWidth(80);

    if (ImGui::BeginCombo("##Layer", to_string(guiLayerIdx).c_str()))
    {
        for (int i = 0; i < layerCount; ++i)
        {
            bool selected = (guiLayerIdx == i);

            if (ImGui::Selectable(to_string(i).c_str(), selected))
                guiLayerIdx = i;

            if (selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    auto& layer = animLayers[guiLayerIdx];

    ImGui::SameLine();
    ImGui::Checkbox("Loop", &layer.loop);

    ImGui::Separator();

    string animName{};
    if (isExistClip(layer.clipIdx))
        animName = animClips[layer.clipIdx]->Get_Name();

    string animInfo = "Clip : " + to_string(layer.clipIdx) + " | Name : " + animName;
    ImGui::Text(animInfo.c_str());

    ImGui::Separator();

    if (ImGui::Button(layer.paused ? "Play" : "Pause", ImVec2(60.f, 0.f)))
        layer.paused = !layer.paused;

    ImGui::SameLine();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    float dur = 0.f;
    if (isExistClip(layer.clipIdx)) dur = animClips[layer.clipIdx]->Get_Duration();

    float prevPos = layer.curTrackPos;

    if (ImGui::SliderFloat("##PlayBar", &layer.curTrackPos, 0.f, dur))
        if (prevPos != layer.curTrackPos) Scrub_Timeline(guiLayerIdx);

    ImGui::Separator();

    ImGui::Text("TimeScale");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderFloat("##TimeScale", &timeScale, 0.f, 3.f, "%.2f");

    ImGui::Separator();

    if (ImGui::Button("x0.10", ImVec2(60.f, 0.f))) timeScale = 0.10f;
    ImGui::SameLine();
    if (ImGui::Button("x0.25", ImVec2(60.f, 0.f))) timeScale = 0.25f;
    ImGui::SameLine();
    if (ImGui::Button("x0.50", ImVec2(60.f, 0.f))) timeScale = 0.50f;
    ImGui::SameLine();
    if (ImGui::Button("x0.75", ImVec2(60.f, 0.f))) timeScale = 0.75f;
    ImGui::SameLine();
    if (ImGui::Button("x1.00", ImVec2(60.f, 0.f))) timeScale = 1.00f;

    ImGui::EndChild();
}

void CAnimator3D::GUI_SelectAnim(_float height)
{
    ImGui::BeginChild("##Animator Animation", ImVec2(0.f, height), true);

    ImVec2 padding = ImGui::GetStyle().FramePadding;
    float lineH = ImGui::GetTextLineHeightWithSpacing();

    string selectedName;
    if (0 <= curClipIdx && curClipIdx < (int)animClips.size())
        selectedName = ExtractCoreAnimName(animClips[curClipIdx]->Get_Name());

    string playingName;
    {
        int playingIdx = animLayers[guiLayerIdx].clipIdx;
        if (0 <= playingIdx && playingIdx < (int)animClips.size())
            playingName = ExtractCoreAnimName(animClips[playingIdx]->Get_Name());
    }

    struct Entry { int idx; string display; string full; bool attack; };

    unordered_set<string> seen;
    seen.reserve(animClips.size());

    vector<Entry> normal;
    vector<Entry> attack;

    normal.reserve(animClips.size());
    attack.reserve(animClips.size());

    for (int i = 0; i < (int)animClips.size(); i++)
    {
        string full = animClips[i]->Get_Name();
        string display = ExtractCoreAnimName(full);

        if (!seen.insert(display).second) continue;

        bool isAtk = IsAttackAnim(display);
        Entry e{i, display, full, isAtk};

        if (isAtk) attack.push_back(e);
        else normal.push_back(e);
    }

    static int lastFocusedIdx = -1;

    auto DrawEntry = [&](const Entry& e)
        {
            bool selected = (!selectedName.empty() && selectedName == e.display);
            bool playing = (!playingName.empty() && playingName == e.display);

            ImGui::PushID(e.idx);

            if (ImGui::Selectable(("##" + e.display).c_str(), selected, 0, ImVec2(0.f, lineH)))
            {
                curClipIdx = e.idx;
                lastFocusedIdx = -1;
                Change_Animation(e.idx).Loop(true).Apply();
            }

            ImVec2 itemMin = ImGui::GetItemRectMin();
            ImVec2 itemMax = ImGui::GetItemRectMax();
            ImVec2 textSize = ImGui::CalcTextSize(e.display.c_str());

            ImVec2 textPos(itemMax.x - textSize.x - padding.x, itemMin.y + padding.y);

            ImU32 col = ImGui::GetColorU32(ImGuiCol_Text);
            if (playing) col = IM_COL32(255, 80, 80, 255);

            ImGui::GetWindowDrawList()->AddText(textPos, col, e.display.c_str());

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", e.full.c_str());

            if (selected && lastFocusedIdx != e.idx)
            {
                ImGui::SetItemDefaultFocus();
                lastFocusedIdx = e.idx;
            }

            ImGui::PopID();
        };

    ImGui::SeparatorText("");

    for (auto& e : normal) DrawEntry(e);

    ImGui::SeparatorText("");

    for (auto& e : attack) DrawEntry(e);

    ImGui::EndChild();
}

void CAnimator3D::Scrub_Timeline(_uint layerIdx)
{
    if (!isExistLayer(layerIdx)) return;

    auto& layer = animLayers[layerIdx];

    if (!isExistClip(layer.clipIdx)) return;

    auto& nowClip = animClips[layer.clipIdx];

    if (layer.blending)
    {
        if (isExistClip(layer.nextClipIdx))
        {
            auto& nextClip = animClips[layer.nextClipIdx];

            nowClip->TranslateAnimateMatrixFromDurationNoEvent(layer.localMats, layer.curTrackPos);
            nextClip->TranslateAnimateMatrixFromDurationNoEvent(layer.blendMats, layer.blendTrackPos);

            float t = 0.f;
            if (0.f < layer.blendDur) t = layer.blendElapsed / layer.blendDur;

            float blendRate = Math::ApplyEase(layer.blendEase, t);

            for (_uint i = 0; i < data->Get_BoneCount(); ++i)
                layer.finalLocalMats[i] = Calc_MatrixBlend(layer.localMats[i], layer.blendMats[i], blendRate);
        }
    }
    else
        nowClip->TranslateAnimateMatrixFromDurationNoEvent(layer.localMats, layer.curTrackPos);

    if (layer.baseLayer)
    {
        layer.wrapped = false;

        if (layer.rootBoneIdx != -1)
        {
            Matrix rootMat = layer.blending ? layer.finalLocalMats[layer.rootBoneIdx] : layer.localMats[layer.rootBoneIdx];

            _vector s, r, tr;
            XMMatrixDecompose(&s, &r, &tr, rootMat);

            layer.prevRootPos = Vector3(tr);
            layer.prevRootQuat = Vector4(r);

            layer.rootMoveDelta = Vector3::Zero;
            XMStoreFloat4(&layer.rootQuatDelta, Quaternion::Identity);
        }

        if (layer.motionBoneIdx != -1)
        {
            _float4x4& mat = layer.blending ? layer.finalLocalMats[layer.motionBoneIdx] : layer.localMats[layer.motionBoneIdx];

            layer.prevMotionPos = _vector3(mat._41, mat._42, mat._43);

            if (hasAxis(layer.extractMoveAxis, AXIS::X)) mat._41 = 0.f;
            if (hasAxis(layer.extractMoveAxis, AXIS::Y)) mat._42 = 0.f;
            if (hasAxis(layer.extractMoveAxis, AXIS::Z)) mat._43 = 0.f;
        }
    }

    BuildBone(0.f);
}

void CAnimator3D::Reset_Anim()
{
    animNames.clear();

    for (auto& clip : animClips) Safe_Release(clip);
    animClips.clear();

    Safe_Release(data);
}

CAnimator3D* CAnimator3D::Create()
{
    CAnimator3D* inst = new CAnimator3D();
    if (FAILED(inst->Initialize_Prototype())) Safe_Release(inst);
    return inst;
}

void CAnimator3D::Free()
{
    __super::Free();

    Safe_Release(data);

    for (auto& clip : animClips) Safe_Release(clip);
    animClips.clear();

    animLayers.clear();
}

HRESULT SetAnimBuild::Apply()
{
    if (!owner) return E_FAIL;
    if (!owner->isExistLayer(layerIdx)) return E_FAIL;
    if (!owner->isExistClip(clipIdx)) return E_FAIL;

    CAnimator3D::ANIM_LAYER& layer = owner->animLayers[layerIdx];

    layer.clipIdx = clipIdx;

    if (layer.baseLayer)
    {
        layer.prevRootPos = owner->animClips[clipIdx]->Get_StartKeyFrameByBoneIndex(layer.rootBoneIdx).vTranslation;
        layer.prevRootQuat = owner->animClips[clipIdx]->Get_StartKeyFrameByBoneIndex(layer.rootBoneIdx).vRotation;

        layer.rootEndPos = owner->animClips[clipIdx]->Get_EndKeyFrameByBoneIndex(layer.rootBoneIdx).vTranslation;
        layer.rootEndQuat = owner->animClips[clipIdx]->Get_EndKeyFrameByBoneIndex(layer.rootBoneIdx).vRotation;

        layer.motionEndPos = owner->animClips[clipIdx]->Get_EndKeyFrameByBoneIndex(layer.motionBoneIdx).vTranslation;
    }
    else
    {
        layer.weight = layerWeight;
        layer.targetWeight = layerTargetWeight;

        layer.weightElapsed = 0.f;
        layer.weightDur = layerWeightDur;

        layer.weightEase = layerEase;

        if (layer.layerType == ANIM_LAYER_STATE::ADDITIVE)
            owner->animClips[clipIdx]->TranslateAnimateMatrixFromDurationNoEvent(owner->basePose, 0.f);
    }

    layer.loop = layerLoop;
    layer.curTrackPos = 0.f;

    layer.animSpeed = layerSpeed;
    layer.paused = layerPause;

    layer.playEase = playEase;
    layer.targetSpeed = layerTargetSpeed;

    layer.easeElapsed = 0.f;
    layer.easeDur = easeDur;

    layer.finished = false;

    return S_OK;
}

HRESULT ChangeAnimBuild::Apply()
{
    if (!owner) return E_FAIL;
    if (!owner->isExistLayer(layerIdx)) return E_FAIL;
    if (!owner->isExistClip(clipIdx)) return E_FAIL;

    auto& layer = owner->animLayers[layerIdx];

    if (layer.baseLayer)
    {
        layer.prevRootPos = owner->animClips[clipIdx]->Get_StartKeyFrameByBoneIndex(layer.rootBoneIdx).vTranslation;
        layer.prevRootQuat = owner->animClips[clipIdx]->Get_StartKeyFrameByBoneIndex(layer.rootBoneIdx).vRotation;

        layer.rootEndPos = owner->animClips[clipIdx]->Get_EndKeyFrameByBoneIndex(layer.rootBoneIdx).vTranslation;
        layer.rootEndQuat = owner->animClips[clipIdx]->Get_EndKeyFrameByBoneIndex(layer.rootBoneIdx).vRotation;

        layer.motionEndPos = owner->animClips[clipIdx]->Get_EndKeyFrameByBoneIndex(layer.motionBoneIdx).vTranslation;
    }
    else
    {
        layer.weight = layerWeight;
        layer.targetWeight = layerTargetWeight;

        layer.weightElapsed = 0.f;
        layer.weightDur = layerWeightDur;

        layer.weightEase = layerEase;

        if (layer.layerType == ANIM_LAYER_STATE::ADDITIVE)
            owner->animClips[clipIdx]->TranslateAnimateMatrixFromDurationNoEvent(owner->basePose, 0.f);
    }

    layer.loop = layerLoop;

    layer.animSpeed = layerSpeed;
    layer.paused = layerPause;

    layer.playEase = playEase;
    layer.targetSpeed = layerTargetSpeed;

    layer.easeElapsed = 0.f;
    layer.easeDur = easeDur;

    if (layer.clipIdx == -1)
    {
        layer.clipIdx = clipIdx;
        layer.curTrackPos = 0.f;
        layer.finished = false;
        return S_OK;
    }
    else
    {
        if (layer.blending)
        {
            layer.clipIdx = layer.nextClipIdx;
            layer.curTrackPos = layer.blendTrackPos;
            layer.localMats = layer.blendMats;
        }
    }

    layer.blending = true;

    layer.keepTrackPos = keepTrackPos;
    layer.ignoreRotation = ignoreRotation;

    layer.nextClipIdx = clipIdx;

    layer.blendTrackPos = 0.f;

    layer.blendElapsed = 0.f;
    layer.blendDur = blendDur;
    layer.blendEase = blendEase;

    layer.finished = false;

    return S_OK;
}