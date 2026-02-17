#include "Engine_Defines.h"
#include "CameraMgr.h"
#include "Camera.h"
#include "GameObject.h"
#include "Engine_Math.h"
#include "GameInstance.h"
#include "AudioDevice.h"

CGameObject* CCameraMgr::ResolveObj(OBJECT_HANDLE handle) const
{
    return ObjectManager()->Request_Object(handle);
}

CCamera* CCameraMgr::ResolveCam(OBJECT_HANDLE handle) const
{
    return handle.isValid() ? ResolveObj(handle)->Get_Component<CCamera>() : nullptr;
}

CCamera* CCameraMgr::Get_BaseCam() const
{
    return ResolveCam(m_baseCamObj);
}

CCamera* CCameraMgr::Get_ActiveCam() const
{
    return ResolveCam(GetActiveCamObj());
}

CCamera* CCameraMgr::Get_ShadowCam() const
{
    return ResolveCam(m_shadowCamObj);
}

void CCameraMgr::Set_MainCam(CCamera* cam, _float blendSec)
{
    SetMainCamObj(cam->Get_OwnerHandle(), blendSec);
}

void CCameraMgr::Set_ShadowCam(CCamera* cam)
{
    SetShadowCamObj(cam->Get_OwnerHandle());
}

_uint CCameraMgr::Push(CCamera* camComp, _float blendTime)
{
    return PushCamObj(camComp->Get_OwnerHandle(), blendTime);
}

void CCameraMgr::SetMainCamObj(OBJECT_HANDLE camObjHandle, _float blendSec)
{
    const OBJECT_HANDLE fromObj = GetActiveCamObj();

    m_baseCamObj = camObjHandle;

    if (!m_overrides.empty()) return;
    BeginBlendTo(fromObj, m_baseCamObj, blendSec);
}

_uint CCameraMgr::PushCamObj(OBJECT_HANDLE camObjHandle, _float blendSec)
{
    const OBJECT_HANDLE fromObj = GetActiveCamObj();

    const _uint handle = m_nextHandle++;
    m_overrides.push_back({handle, camObjHandle});

    BeginBlendTo(fromObj, camObjHandle, blendSec);
    return handle;
}

_bool CCameraMgr::Pop(_uint handle, _float blendTime)
{
    const OBJECT_HANDLE fromObj = GetActiveCamObj();

    auto it = find_if(m_overrides.begin(), m_overrides.end(), [&](const OverrideEntry& e) { return e.handle == handle; });
    m_overrides.erase(it);

    BeginBlendTo(fromObj, GetActiveCamObj(), blendTime);
    return true;
}

void CCameraMgr::Clear(_float blendTime)
{
    const OBJECT_HANDLE fromObj = GetActiveCamObj();

    m_overrides.clear();
    BeginBlendTo(fromObj, m_baseCamObj, blendTime);
}

void CCameraMgr::AddShakeAxis(CamShakeAxis axes, _float ampDeg, _float freq, _float dur, _float fadeOutSec)
{
    m_shake.AddAxis(axes, ampDeg, freq, dur, fadeOutSec);
}

void CCameraMgr::SetShakeAxis(CamShakeAxis axes, _float ampDeg, _float freq, _float dur, _float fadeOutSec)
{
    m_shake.SetAxis(axes, ampDeg, freq, dur, fadeOutSec);
}

void CCameraMgr::AddShakeAxisWave(CamShakeAxis axes, _float ampDeg, _float freq, _float dur, _float fadeOutSec, EaseType attackEase, EaseType decayEase)
{
    m_shake.AddAxisWave(axes, ampDeg, freq, dur, fadeOutSec, attackEase, decayEase);
}

void CCameraMgr::SetShakeAxisWave(CamShakeAxis axes, _float ampDeg, _float freq, _float dur, _float fadeOutSec, EaseType attackEase, EaseType decayEase)
{
    m_shake.SetAxisWave(axes, ampDeg, freq, dur, fadeOutSec, attackEase, decayEase);
}

void CCameraMgr::SetFov(_float deltaDeg, _float blendSec, EaseType easeType)
{
    if (blendSec <= 0.f)
    {
        m_fovOffsetCur += deltaDeg;

        if (m_fovOffsetBlending)
        {
            m_fovOffsetFrom += deltaDeg;
            m_fovOffsetTo += deltaDeg;
        }
        else
        {
            m_fovOffsetFrom = m_fovOffsetCur;
            m_fovOffsetTo = m_fovOffsetCur;
        }
        return;
    }

    m_fovOffsetFrom = m_fovOffsetCur;
    m_fovOffsetTo = m_fovOffsetCur + deltaDeg;

    m_fovOffsetTime = 0.f;
    m_fovOffsetDuration = max(blendSec, 0.0001f);
    m_fovOffsetEaseType = easeType;

    m_fovOffsetBlending = true;
}

_float CCameraMgr::EvalFovOffset(_float dt)
{
    if (!m_fovOffsetBlending) return m_fovOffsetCur;

    m_fovOffsetTime += dt;

    const _float rawT = m_fovOffsetTime / m_fovOffsetDuration;
    const _float t = Math::ApplyEase(m_fovOffsetEaseType, rawT);

    if (rawT >= 1.f)
    {
        m_fovOffsetBlending = false;
        m_fovOffsetCur = m_fovOffsetTo;
        return m_fovOffsetCur;
    }

    m_fovOffsetCur = m_fovOffsetFrom + (m_fovOffsetTo - m_fovOffsetFrom) * t;
    return m_fovOffsetCur;
}

void CCameraMgr::ApplyFov(_float dt, _float baseFov)
{
    const _float fovOffset = EvalFovOffset(dt);
    m_outputPose.lens.fov = baseFov + fovOffset;
}

void CCameraMgr::SetZNear(_float zNear, _float blendSec, EaseType easeType)
{
    const _float start = m_overrideNear ? m_nearCur : m_outputPose.lens.nearZ;

    m_overrideNear = true;

    if (blendSec <= 0.f)
    {
        m_nearBlending = false;

        m_nearCur = zNear;
        m_nearFrom = m_nearCur;
        m_nearTo = m_nearCur;
        return;
    }

    m_nearFrom = start;
    m_nearTo = zNear;

    m_nearTime = 0.f;
    m_nearDuration = max(blendSec, 0.0001f);
    m_nearEaseType = easeType;

    m_nearBlending = true;
}

void CCameraMgr::SetZFar(_float zFar, _float blendSec, EaseType easeType)
{
    const _float start = m_overrideFar ? m_farCur : m_outputPose.lens.farZ;

    m_overrideFar = true;

    if (blendSec <= 0.f)
    {
        m_farBlending = false;

        m_farCur = zFar;
        m_farFrom = m_farCur;
        m_farTo = m_farCur;
        return;
    }

    m_farFrom = start;
    m_farTo = zFar;

    m_farTime = 0.f;
    m_farDuration = max(blendSec, 0.0001f);
    m_farEaseType = easeType;

    m_farBlending = true;
}


void CCameraMgr::ApplyNearFarOverrides()
{
    if (m_overrideNear) m_outputPose.lens.nearZ = m_nearOverride;
    if (m_overrideFar)  m_outputPose.lens.farZ = m_farOverride;
}

_float CCameraMgr::EvalNearOverride(_float dt, _float baseNear)
{
    if (!m_overrideNear) return baseNear;
    if (!m_nearBlending) return m_nearCur;

    m_nearTime += dt;

    const _float rawT = m_nearTime / m_nearDuration;
    const _float t = Math::ApplyEase(m_nearEaseType, rawT);

    if (rawT >= 1.f)
    {
        m_nearBlending = false;
        m_nearCur = m_nearTo;
        return m_nearCur;
    }

    m_nearCur = m_nearFrom + (m_nearTo - m_nearFrom) * t;
    return m_nearCur;
}

_float CCameraMgr::EvalFarOverride(_float dt, _float baseFar)
{
    if (!m_overrideFar) return baseFar;
    if (!m_farBlending) return m_farCur;

    m_farTime += dt;

    const _float rawT = m_farTime / m_farDuration;
    const _float t = Math::ApplyEase(m_farEaseType, rawT);

    if (rawT >= 1.f)
    {
        m_farBlending = false;
        m_farCur = m_farTo;
        return m_farCur;
    }

    m_farCur = m_farFrom + (m_farTo - m_farFrom) * t;
    return m_farCur;
}

void CCameraMgr::ApplyNearFarOverrides(_float dt)
{
    const _float baseNear = m_outputPose.lens.nearZ;
    const _float baseFar = m_outputPose.lens.farZ;

    m_outputPose.lens.nearZ = EvalNearOverride(dt, baseNear);
    m_outputPose.lens.farZ = EvalFarOverride(dt, baseFar);
}


CCameraMgr::CamPoseFrame CCameraMgr::CapturePose(CCamera* cam) const
{
    CamPoseFrame pose{};

    const Matrix view = cam->Get_ViewMatrix();
    const Matrix world = view.Invert();

    pose.pos = world.Translation();

    pose.rot = Quaternion::CreateFromRotationMatrix(world);
    pose.rot.Normalize();

    pose.lens.projType = cam->Get_ProjType();
    pose.lens.fov = cam->Get_FOV();
    pose.lens.nearZ = cam->Get_Near();
    pose.lens.farZ = cam->Get_Far();
    pose.lens.aspect = cam->Get_Aspect();

    if (pose.lens.projType == CamProjType::Orthographic)
        pose.lens.orthoHeight = cam->Get_OrthoSize() * 2.f;

    return pose;
}

CCameraMgr::CamPoseFrame CCameraMgr::BlendPose(const CamPoseFrame& a, const CamPoseFrame& b, _float t) const
{
    CamPoseFrame out{};

    out.pos = Vector3::Lerp(a.pos, b.pos, t);

    Quaternion bRot = b.rot;
    if (a.rot.Dot(bRot) < 0.f) bRot = -bRot;

    out.rot = Quaternion::Slerp(a.rot, bRot, t);
    out.rot.Normalize();

    out.lens = b.lens;
    out.lens.fov = a.lens.fov + (b.lens.fov - a.lens.fov) * t;
    out.lens.nearZ = a.lens.nearZ + (b.lens.nearZ - a.lens.nearZ) * t;
    out.lens.farZ = a.lens.farZ + (b.lens.farZ - a.lens.farZ) * t;
    out.lens.aspect = a.lens.aspect + (b.lens.aspect - a.lens.aspect) * t;
    out.lens.orthoHeight = a.lens.orthoHeight + (b.lens.orthoHeight - a.lens.orthoHeight) * t;

    return out;
}

void CCameraMgr::ApplyCache(CamCache& outCache, const CamPoseFrame& pose)
{
    const Matrix rotM = Matrix::CreateFromQuaternion(pose.rot);
    const Matrix trM = Matrix::CreateTranslation(pose.pos);
    const Matrix world = rotM * trM;

    outCache.view = world.Invert();

    const _float fovRad = XMConvertToRadians(pose.lens.fov);

    if (pose.lens.projType == CamProjType::Perspective)
        outCache.proj = XMMatrixPerspectiveFovLH(fovRad, pose.lens.aspect, pose.lens.nearZ, pose.lens.farZ);
    else
        outCache.proj = XMMatrixOrthographicLH(pose.lens.orthoHeight * pose.lens.aspect, pose.lens.orthoHeight, pose.lens.nearZ, pose.lens.farZ);

    outCache.invView = outCache.view.Invert();
    outCache.invProj = outCache.proj.Invert();

    outCache.pos = {pose.pos.x, pose.pos.y, pose.pos.z, 1.f};
    outCache.farZ = pose.lens.farZ;
}

void CCameraMgr::BeginBlendTo(OBJECT_HANDLE fromObj, OBJECT_HANDLE targetObj, _float blendSec)
{
    m_isBlending = (blendSec > 0.f);
    m_blendTime = 0.f;
    m_blendDuration = blendSec;
    m_blendTargetObj = targetObj;
    m_blendFromObj = fromObj;
    m_blendEaseType = m_easeType;

    auto fromCam = ResolveCam(m_blendFromObj);
    if (fromCam) m_blendFrom = CapturePose(fromCam);
    else m_blendFrom = m_outputPose;

    if (!m_isBlending)
    {
        auto cam = ResolveCam(m_blendTargetObj);
        if (cam) m_outputPose = CapturePose(cam);
    }
}

void CCameraMgr::UpdateShadowCache()
{
    if (m_shadowCamObj.isValid())
    {
        CamPoseFrame pose = CapturePose(ResolveCam(m_shadowCamObj));
        ApplyCache(shadow, pose);
    }
}

void CCameraMgr::Update(_float dt)
{
    if (!m_baseCamObj.isValid()) return;

    auto targetCam = ResolveCam(GetActiveCamObj());
    CamPoseFrame targetPose = CapturePose(targetCam);

    if (m_isBlending)
    {
        m_blendTime += dt;

        const _float rawT = m_blendTime / m_blendDuration;
        const _float t = Math::ApplyEase(m_blendEaseType, rawT);

        if (rawT >= 1.f)
        {
            m_isBlending = false;
            m_outputPose = targetPose;
        }
        else
        {
            CamPoseFrame fromPose = m_blendFrom;

            auto fromCam = ResolveCam(m_blendFromObj);
            if (fromCam) fromPose = CapturePose(fromCam);

            m_outputPose = BlendPose(fromPose, targetPose, t);
        }
    }
    else
        m_outputPose = targetPose;

    Vector3    posDelta{};
    Quaternion rotDelta = Quaternion::Identity;
    m_shake.Apply(m_outputPose.rot, dt, posDelta, rotDelta);

    m_outputPose.rot = rotDelta * m_outputPose.rot;
    m_outputPose.rot.Normalize();
    m_outputPose.pos += posDelta;

    const _float baseFov = m_outputPose.lens.fov;

    ApplyFov(dt, baseFov);
    ApplyNearFarOverrides(dt);

    ApplyCache(main, m_outputPose);

    if (m_shadowCamObj.isValid()) UpdateShadowCache();
}


void CCameraMgr::AddImpact(_uint shakeType, _uint zoomType, _float strength)
{
    m_shake.Add(shakeType, strength);
    m_zoom.Add(zoomType, strength);
}

Lens CCameraMgr::Get_Lens() const
{
    Lens out{};
    out.fov = m_outputPose.lens.fov;
    out.zNear = m_outputPose.lens.nearZ;
    out.zFar = m_outputPose.lens.farZ;
    out.aspect = m_outputPose.lens.aspect;
    return out;
}

Lens CCameraMgr::Get_ShadowLens() const
{
    auto cam = ResolveCam(m_shadowCamObj);

    Lens out{};
    out.fov = cam->Get_FOV();
    out.zNear = cam->Get_Near();
    out.zFar = cam->Get_Far();
    out.aspect = cam->Get_Aspect();
    return out;
}

Vector4 CCameraMgr::GetForward() const
{
    return GetActiveCamObj().Get()->Get_Component<CTransform>()->Dir(STATE::LOOK);
}

Vector4 CCameraMgr::GetRight() const
{
    return GetActiveCamObj().Get()->Get_Component<CTransform>()->Dir(STATE::RIGHT);
}

void CCameraMgr::Free()
{
    m_overrides.clear();
    m_shake.Reset();
    m_zoom.Reset();

    m_baseCamObj.Reset();
    m_shadowCamObj.Reset();

    m_isBlending = false;
    m_blendTime = 0.f;
    m_blendDuration = 0.f;
    m_blendTargetObj.Reset();
    m_outputPose = {};

    m_fovOffsetBlending = false;
    m_fovOffsetTime = 0.f;
    m_fovOffsetDuration = 0.f;
    m_fovOffsetCur = 0.f;
    m_fovOffsetFrom = 0.f;
    m_fovOffsetTo = 0.f;
    m_fovOffsetEaseType = EaseType::Linear;

    m_overrideNear = false;
    m_overrideFar = false;
    m_nearOverride = 0.f;
    m_farOverride = 0.f;

    m_nearBlending = false;
    m_nearTime = 0.f;
    m_nearDuration = 0.f;
    m_nearCur = 0.f;
    m_nearFrom = 0.f;
    m_nearTo = 0.f;
    m_nearEaseType = EaseType::Linear;

    m_farBlending = false;
    m_farTime = 0.f;
    m_farDuration = 0.f;
    m_farCur = 0.f;
    m_farFrom = 0.f;
    m_farTo = 0.f;
    m_farEaseType = EaseType::Linear;

    main = {};
    shadow = {};
}

