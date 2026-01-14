#include "Engine_Defines.h"
#include "CameraMgr.h"
#include "Camera.h"
#include "GameObject.h"
#include "Engine_Math.h"
#include "GameInstance.h"

CGameObject* CCameraMgr::ResolveObj(OBJECT_HANDLE handle) const
{
    return handle.isValid() ? ObjectManager()->Request_Object(handle) : nullptr;
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

void CCameraMgr::Set_MainCam(CCamera* pCamCom, _float blendSec)
{
    SetMainCamObj(pCamCom->Get_OwnerHandle(), blendSec);
}

void CCameraMgr::Set_ShadowCam(CCamera* pCamCom)
{
    SetShadowCamObj(pCamCom->Get_OwnerHandle());
}

_uint CCameraMgr::Push(CCamera* camComp, _float blendTime)
{
    return PushCamObj(camComp->Get_OwnerHandle(), blendTime);
}

void CCameraMgr::SetMainCamObj(OBJECT_HANDLE camObjHandle, _float blendSec)
{
    m_baseCamObj = camObjHandle;

    if (!m_overrides.empty()) return;
    BeginBlendTo(m_baseCamObj, blendSec);
}

void CCameraMgr::SetShadowCamObj(OBJECT_HANDLE camObjHandle)
{
    m_shadowCamObj = camObjHandle;
}

_uint CCameraMgr::PushCamObj(OBJECT_HANDLE camObjHandle, _float blendSec)
{
    const _uint handle = m_nextHandle++;
    m_overrides.push_back({handle, camObjHandle});

    BeginBlendTo(camObjHandle, blendSec);
    return handle;
}

_bool CCameraMgr::Pop(_uint handle, _float blendTime)
{
    auto it = find_if(m_overrides.begin(), m_overrides.end(), [&](const OverrideEntry& e) { return e.handle == handle; });
    m_overrides.erase(it);

    BeginBlendTo(GetActiveCamObj(), blendTime);
    return true;
}

void CCameraMgr::Clear(_float blendTime)
{
    m_overrides.clear();
    BeginBlendTo(m_baseCamObj, blendTime);
}

CCameraMgr::CamPoseFrame CCameraMgr::CapturePose(CCamera* cam) const
{
    CamPoseFrame pose{};

    const Matrix view  = cam->Get_ViewMatrix();
    const Matrix world = view.Invert();

    pose.pos = world.Translation() + cam->Get_ViewOffset();

    pose.rot = Quaternion::CreateFromRotationMatrix(world);
    pose.rot.Normalize();

    pose.lens.projType = cam->Get_ProjType();
    pose.lens.fov      = cam->Get_FOV();
    pose.lens.nearZ    = cam->Get_Near();
    pose.lens.farZ     = cam->Get_Far();
    pose.lens.aspect   = cam->Get_Aspect();

    if (pose.lens.projType == CamProjType::Orthographic)
        pose.lens.orthoHeight = cam->Get_OrthoSize() * 2.f;

    return pose;
}

CCameraMgr::CamPoseFrame CCameraMgr::BlendPose(const CamPoseFrame& a, const CamPoseFrame& b, _float t) const
{
    CamPoseFrame out{};

    out.pos = Vector3::Lerp(a.pos, b.pos, t);
    out.rot = Quaternion::Slerp(a.rot, b.rot, t);
    out.rot.Normalize();

    out.lens             = b.lens;
    out.lens.fov         = a.lens.fov         + (b.lens.fov         - a.lens.fov)         * t;
    out.lens.nearZ       = a.lens.nearZ       + (b.lens.nearZ       - a.lens.nearZ)       * t;
    out.lens.farZ        = a.lens.farZ        + (b.lens.farZ        - a.lens.farZ)        * t;
    out.lens.aspect      = a.lens.aspect      + (b.lens.aspect      - a.lens.aspect)      * t;
    out.lens.orthoHeight = a.lens.orthoHeight + (b.lens.orthoHeight - a.lens.orthoHeight) * t;

    return out;
}

void CCameraMgr::ApplyCache(CamCache& outCache, const CamPoseFrame& pose)
{
    const Matrix rotM  = Matrix::CreateFromQuaternion(pose.rot);
    const Matrix trM   = Matrix::CreateTranslation(pose.pos);
    const Matrix world = rotM * trM;

    outCache.view = world.Invert();

    if (pose.lens.projType == CamProjType::Perspective)
        outCache.proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(pose.lens.fov), pose.lens.aspect, pose.lens.nearZ, pose.lens.farZ);
    else
        outCache.proj = DirectX::XMMatrixOrthographicLH(pose.lens.orthoHeight * pose.lens.aspect, pose.lens.orthoHeight, pose.lens.nearZ, pose.lens.farZ);

    outCache.invView = outCache.view.Invert();
    outCache.invProj = outCache.proj.Invert();

    outCache.pos  = {pose.pos.x, pose.pos.y, pose.pos.z, 1.f};
    outCache.farZ = pose.lens.farZ;
}

void CCameraMgr::BeginBlendTo(OBJECT_HANDLE targetObj, _float blendSec)
{
    m_isBlending     = (blendSec > 0.f);
    m_blendTime      = 0.f;
    m_blendDuration  = blendSec;
    m_blendFrom      = m_outputPose;
    m_blendTargetObj = targetObj;

    if (!m_isBlending)
        m_outputPose = CapturePose(ResolveCam(m_blendTargetObj));
}

void CCameraMgr::ApplyShake(CamPoseFrame& ioPose, _float dt)
{
    if (m_shakes.empty()) return;

    _float yaw   = 0.f;
    _float pitch = 0.f;
    _float roll  = 0.f;

    for (auto& s : m_shakes)
    {
        s.elapsed += dt;

        _float w = 1.f;
        if (s.elapsed > s.duration)
        {
            if (s.fadeOutSec <= 0.f) w = 0.f;
            else
            {
                const _float k = (s.elapsed - s.duration) / s.fadeOutSec;
                w = 1.f - Math::Clamp01(k);
            }
        }

        const _float rad    = DirectX::XMConvertToRadians(s.amplitudeDeg) * w;
        const _float phaseT = s.elapsed * s.frequency * 6.28318530718f;

        pitch += sinf(phaseT + s.phase.x) * rad;
        yaw   += sinf(phaseT + s.phase.y) * rad;
        roll  += sinf(phaseT + s.phase.z) * rad;
    }

    Quaternion dq = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
    dq.Normalize();

    ioPose.rot = dq * ioPose.rot;
    ioPose.rot.Normalize();

    m_shakes.erase(remove_if(m_shakes.begin(), m_shakes.end(), [&](const ShakeInstance& s)
        {
            if (s.elapsed <= s.duration) return false;
            if (s.fadeOutSec <= 0.f)     return true;
            return (s.elapsed - s.duration) >= s.fadeOutSec;
        }), m_shakes.end());
}

void CCameraMgr::UpdateShadowCache()
{
    CamPoseFrame pose = CapturePose(ResolveCam(m_shadowCamObj));
    ApplyCache(shadow, pose);
}

void CCameraMgr::SetShake(_float ampDeg, _float freq, _float dur, _float fadeOutSec)
{
    m_shakes.clear();
    AddShake(ampDeg, freq, dur, fadeOutSec);
}

void CCameraMgr::AddShake(_float ampDeg, _float freq, _float dur, _float fadeOutSec)
{
    ShakeInstance s{};
    s.amplitudeDeg = ampDeg;
    s.frequency    = freq;
    s.duration     = dur;
    s.fadeOutSec   = fadeOutSec;
    s.elapsed      = 0.f;
    s.phase        = Math::SeedPhase(m_shakeSeed);

    m_shakes.push_back(s);
}

void CCameraMgr::ClearShake(_float fadeOutSec)
{
    if (fadeOutSec <= 0.f)
    {
        m_shakes.clear();
        return;
    }

    for (auto& s : m_shakes)
    {
        s.duration   = min(s.duration, s.elapsed);
        s.fadeOutSec = fadeOutSec;
    }
}

void CCameraMgr::Update(_float dt)
{
    if (!m_baseCamObj.isValid()) return;

    auto targetCam          = ResolveCam(GetActiveCamObj());
    CamPoseFrame targetPose = CapturePose(targetCam);

    if (m_isBlending)
    {
        m_blendTime += dt;

        const _float rawT = m_blendTime / m_blendDuration;
        const _float t    = Math::ApplyEase(m_easeType, rawT);

        if (rawT >= 1.f)
        {
            m_isBlending = false;
            m_outputPose = targetPose;
        }
        else
            m_outputPose = BlendPose(m_blendFrom, targetPose, t);
    }
    else
        m_outputPose = targetPose;

    ApplyShake(m_outputPose, dt);
    ApplyCache(main, m_outputPose);
    UpdateShadowCache();
}

Lens CCameraMgr::Get_Lens() const
{
    Lens out{};
    out.fov    = m_outputPose.lens.fov;
    out.zNear  = m_outputPose.lens.nearZ;
    out.zFar   = m_outputPose.lens.farZ;
    out.aspect = m_outputPose.lens.aspect;
    return out;
}

Lens CCameraMgr::Get_ShadowLens() const
{
    auto cam = ResolveCam(m_shadowCamObj);

    Lens out{};
    out.fov    = cam->Get_FOV();
    out.zNear  = cam->Get_Near();
    out.zFar   = cam->Get_Far();
    out.aspect = cam->Get_Aspect();
    return out;
}

void CCameraMgr::Free()
{
    m_overrides.clear();
    m_shakes.clear();

    m_baseCamObj.Reset();
    m_shadowCamObj.Reset();

    m_isBlending    = false;
    m_blendTime     = 0.f;
    m_blendDuration = 0.f;
    m_blendTargetObj.Reset();
    m_outputPose    = {};

    main   = {};
    shadow = {};
}