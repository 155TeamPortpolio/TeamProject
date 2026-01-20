#include "Engine_Defines.h"
#include "CameraMgr.h"
#include "Camera.h"
#include "GameObject.h"
#include "Engine_Math.h"
#include "GameInstance.h"

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

    pose.pos = world.Translation();

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
    const Matrix rotM = Matrix::CreateFromQuaternion(pose.rot);
    const Matrix trM = Matrix::CreateTranslation(pose.pos);
    const Matrix world = rotM * trM;

    outCache.view = world.Invert();

    const _float fovRad = pose.lens.fov * (XM_PI / 180.f);

    if (pose.lens.projType == CamProjType::Perspective)
        outCache.proj = XMMatrixPerspectiveFovLH(fovRad, pose.lens.aspect, pose.lens.nearZ, pose.lens.farZ);
    else
        outCache.proj = XMMatrixOrthographicLH(pose.lens.orthoHeight * pose.lens.aspect, pose.lens.orthoHeight, pose.lens.nearZ, pose.lens.farZ);

    outCache.invView = outCache.view.Invert();
    outCache.invProj = outCache.proj.Invert();

    outCache.pos = {pose.pos.x, pose.pos.y, pose.pos.z, 1.f};
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
    {
        auto cam = ResolveCam(m_blendTargetObj);
        if (cam)
            m_outputPose = CapturePose(cam);
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
        const _float t = Math::ApplyEase(m_easeType, rawT);

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

    Vector3 posDelta{};
    Quaternion rotDelta = Quaternion::Identity;
    m_shake.Apply(m_outputPose.rot, dt, posDelta, rotDelta);

    m_outputPose.rot = rotDelta * m_outputPose.rot;
    m_outputPose.rot.Normalize();
    m_outputPose.pos += posDelta;

    const _float zoomDeg = m_zoom.Apply(dt);
    if (zoomDeg != 0.f)
    {
        const _float minFov = 8.f;
        const _float maxFov = 120.f;

        m_outputPose.lens.fov -= zoomDeg;

        if (m_outputPose.lens.fov < minFov) m_outputPose.lens.fov = minFov;
        if (m_outputPose.lens.fov > maxFov) m_outputPose.lens.fov = maxFov;
    }

    ApplyCache(main, m_outputPose);

    if (m_shadowCamObj.isValid())
        UpdateShadowCache();
}

void CCameraMgr::AddImpact(CamShakeType shakeType, CamZoomType zoomType, _float strength)
{
    m_shake.Add(shakeType, strength);
    m_zoom.Add(zoomType, strength);
}

void CCameraMgr::AddImpact(_uint shakeType, _uint zoomType, _float strength)
{
    if (shakeType < ENUM(CamShakeType::End))
        m_shake.Add(static_cast<CamShakeType>(shakeType), strength);

    if (zoomType < ENUM(CamShakeType::End))
        m_zoom.Add(static_cast<CamZoomType>(zoomType), strength);
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

    main = {};
    shadow = {};
}