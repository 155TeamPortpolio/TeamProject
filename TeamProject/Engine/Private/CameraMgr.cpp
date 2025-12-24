#include "Engine_Defines.h"
#include "CameraMgr.h"
#include "Camera.h"
#include "GameObject.h"

namespace
{
    float QuatDot(const Quaternion& a, const Quaternion& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }
    Quaternion QuatNegate(const Quaternion& q)
    {
        return Quaternion(-q.x, -q.y, -q.z, -q.w);
    }
    _uint LcgStep(_uint& s)
    {
        s = s * 1664525u + 1013904223u;
        return s;
    }
    float Rand01(_uint& s)
    {
        const _uint x = LcgStep(s) & 0x00FFFFFFu;
        return (float)x / (float)0x01000000u;
    }
    float SmoothStep01(float t)
    {
        t = clamp(t, 0.f, 1.f);
        return t * t * (3.f - 2.f * t);
    }
}

void CCameraMgr::Set_MainCam(CCamera* camComp, _float blendSec)
{
    Safe_Release(m_baseCam);
    m_baseCam = camComp;
    Safe_AddRef(m_baseCam);

    if (m_overrides.empty())
        BeginBlendTo(Get_ActiveCam(), blendSec);
}

void CCameraMgr::Set_ShadowCam(CCamera* camComp)
{
    Safe_Release(m_shadowCam);
    m_shadowCam = camComp;
    Safe_AddRef(m_shadowCam);
}

CCamera* CCameraMgr::Get_ActiveCam() const
{
    if (!m_overrides.empty())
        return m_overrides.back().cam;

    return m_baseCam;
}

_uint CCameraMgr::Push(CCamera* camComp, _float blendSec)
{
    OverrideEntry entry{};
    entry.handle = m_nextHandle++;
    entry.cam = camComp;

    Safe_AddRef(entry.cam);
    m_overrides.push_back(entry);

    BeginBlendTo(Get_ActiveCam(), blendSec);
    return entry.handle;
}

_bool CCameraMgr::Pop(_uint handle, _float blendSec)
{
    size_t idx = (size_t)-1;
    for (size_t i = 0; i < m_overrides.size(); ++i)
        if (m_overrides[i].handle == handle)
        {
            idx = i;
            break;
        }

    if (idx == (size_t)-1) return false;

    const bool wasTop = (idx + 1 == m_overrides.size());

    Safe_Release(m_overrides[idx].cam);
    m_overrides.erase(m_overrides.begin() + (ptrdiff_t)idx);

    if (wasTop)
        BeginBlendTo(Get_ActiveCam(), blendSec);

    return true;
}

void CCameraMgr::Clear(_float blendSec)
{
    for (size_t i = 0; i < m_overrides.size(); ++i)
        Safe_Release(m_overrides[i].cam);

    m_overrides.clear();
    BeginBlendTo(Get_ActiveCam(), blendSec);
}

void CCameraMgr::SetShake(_float amplitudeDeg, _float frequency, _float duration, _float fadeOutSec)
{
    ClearShake();
    AddShake(amplitudeDeg, frequency, duration, fadeOutSec);
}

void CCameraMgr::AddShake(_float amplitudeDeg, _float frequency, _float duration, _float fadeOutSec)
{
    ShakeInstance shakeInst{};
    shakeInst.amplitudeDeg = amplitudeDeg;
    shakeInst.frequency    = frequency;
    shakeInst.duration     = duration;
    shakeInst.fadeOutSec   = fadeOutSec;
    shakeInst.elapsed      = 0.f;

    shakeInst.phase.x = Rand01(m_shakeSeed) * XM_2PI;
    shakeInst.phase.y = Rand01(m_shakeSeed) * XM_2PI;
    shakeInst.phase.z = Rand01(m_shakeSeed) * XM_2PI;

    m_shakes.push_back(shakeInst);
}

void CCameraMgr::ClearShake(_float fadeOutSec)
{
    if (fadeOutSec <= 0.f)
    {
        m_shakes.clear();
        return;
    }

    for (size_t i = 0; i < m_shakes.size();)
    {
        auto& shake = m_shakes[i];

        const float remain = shake.duration - shake.elapsed;
        if (remain <= 0.f)
        {
            m_shakes.erase(m_shakes.begin() + (ptrdiff_t)i);
            continue;
        }

        const float outSec = min(fadeOutSec, remain);
        shake.fadeOutSec = outSec;
        shake.duration = shake.elapsed + outSec;

        ++i;
    }
}

void CCameraMgr::Update(_float dt)
{
    CamPoseFrame pose{};

    if (m_isBlending)
    {
        m_blendTime += dt;

        float t = clamp(m_blendTime / m_blendDuration, 0.f, 1.f);

        if (m_easeType != EaseType::None)
            t = clamp(Math::ApplyEase(m_easeType, t), 0.f, 1.f);

        pose = BlendPose(m_blendFrom, CapturePose(m_blendTargetCam), t);

        if (t >= 1.f)
        {
            m_isBlending = false;
            Safe_Release(m_blendTargetCam);
            m_blendTargetCam = nullptr;
        }
    }
    else
        pose = CapturePose(Get_ActiveCam());

    ApplyShake(pose, dt);
    ApplyOutputPose(pose);

    UpdateShadowCache();
}

CCameraMgr::CamPoseFrame CCameraMgr::CapturePose(CCamera* cam) const
{
    CamPoseFrame out{};

    CTransform* transform = cam->Get_Owner()->Get_Component<CTransform>();
    Matrix world(transform->Get_WorldMatrix());

    Vector3 scale{}, trans{};
    Quaternion rot = Quaternion::Identity;

    world.Decompose(scale, rot, trans);
    rot.Normalize();

    out.pos           = trans;
    out.rot           = rot;
    out.lens.projType = cam->Get_ProjType();
    out.lens.fov      = cam->Get_FOV();
    out.lens.nearZ    = cam->Get_Near();
    out.lens.farZ     = cam->Get_Far();
    out.lens.aspect   = cam->Get_Aspect();

    if (out.lens.projType == CamProjType::Orthographic)
    {
        const Matrix proj = cam->Get_ProjMatrix();
        out.lens.orthoHeight = 2.f / proj._22;
    }

    return out;
}

CCameraMgr::CamPoseFrame CCameraMgr::BlendPose(const CamPoseFrame& a, const CamPoseFrame& b, _float t) const
{
    CamPoseFrame out{};

    out.pos = a.pos + (b.pos - a.pos) * t;

    Quaternion qb = b.rot;
    if (QuatDot(a.rot, qb) < 0.f)
        qb = QuatNegate(qb);

    out.rot = Quaternion::Slerp(a.rot, qb, t);
    out.rot.Normalize();

    out.lens.nearZ  = a.lens.nearZ  + (b.lens.nearZ  - a.lens.nearZ)  * t;
    out.lens.farZ   = a.lens.farZ   + (b.lens.farZ   - a.lens.farZ)   * t;
    out.lens.aspect = a.lens.aspect + (b.lens.aspect - a.lens.aspect) * t;

    if (a.lens.projType == b.lens.projType)
    {
        out.lens.projType = a.lens.projType;

        if (out.lens.projType == CamProjType::Perspective)
            out.lens.fov = a.lens.fov + (b.lens.fov - a.lens.fov) * t;
        else
            out.lens.orthoHeight = a.lens.orthoHeight + (b.lens.orthoHeight - a.lens.orthoHeight) * t;
    }
    else
    {
        if (t < 1.f) out.lens = a.lens;
        else out.lens = b.lens;
    }

    return out;
}

void CCameraMgr::ApplyOutputPose(const CamPoseFrame& pose)
{
    m_outputPose = pose;

    const Matrix world = Matrix::CreateFromQuaternion(pose.rot) * Matrix::CreateTranslation(pose.pos);

    m_invView = world;
    m_view = world.Invert();

    if (pose.lens.projType == CamProjType::Perspective)
        m_proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(pose.lens.fov), pose.lens.aspect, pose.lens.nearZ, pose.lens.farZ);
    else
    {
        const float height = pose.lens.orthoHeight;
        const float width  = height * pose.lens.aspect;
        m_proj = XMMatrixOrthographicLH(width, height, pose.lens.nearZ, pose.lens.farZ);
    }

    m_invProj = m_proj.Invert();

    m_camPos = Vector4(pose.pos.x, pose.pos.y, pose.pos.z, 1.f);
    m_activeFar = pose.lens.farZ;
}

void CCameraMgr::BeginBlendTo(CCamera* targetCam, _float blendSec)
{
    Safe_Release(m_blendTargetCam);
    m_blendTargetCam = nullptr;

    if (blendSec <= 0.f)
    {
        m_isBlending = false;
        m_blendTime = 0.f;
        m_blendDuration = 0.f;
        ApplyOutputPose(CapturePose(targetCam));
        return;
    }

    m_blendTargetCam = targetCam;
    Safe_AddRef(m_blendTargetCam);

    m_blendFrom = GetCurOutputPose();
    m_isBlending = true;
    m_blendTime = 0.f;
    m_blendDuration = blendSec;
}

void CCameraMgr::UpdateShadowCache()
{
    if (!m_shadowCam) return;

    m_shadowView    = m_shadowCam->Get_ViewMatrix();
    m_shadowProj    = m_shadowCam->Get_ProjMatrix();
    m_shadowInvView = m_shadowView.Invert();
    m_shadowInvProj = m_shadowProj.Invert();
    m_shadowCamPos  = m_shadowCam->Get_Pos();
}

void CCameraMgr::ApplyShake(CamPoseFrame& ioPose, _float dt)
{
    if (m_shakes.empty()) return;

    float sumYawDeg = 0.f;
    float sumPitchDeg = 0.f;
    float sumRollDeg = 0.f;

    for (size_t i = 0; i < m_shakes.size();)
    {
        auto& shake = m_shakes[i];

        shake.elapsed += dt;

        if (shake.elapsed >= shake.duration)
        {
            m_shakes.erase(m_shakes.begin() + (ptrdiff_t)i);
            continue;
        }

        const float fadeInSec = min(0.05f, shake.duration * 0.2f);

        float wIn = 1.f;
        if (fadeInSec > 0.f) wIn = SmoothStep01(shake.elapsed / fadeInSec);

        float fadeOutSec = shake.fadeOutSec;
        if (fadeOutSec < 0.f) fadeOutSec = 0.f;
        if (fadeOutSec > shake.duration) fadeOutSec = shake.duration;

        float wOut = 1.f;
        if (fadeOutSec > 0.f)
        {
            const float startFadeOut = shake.duration - fadeOutSec;
            if (shake.elapsed >= startFadeOut)
            {
                const float u = (shake.elapsed - startFadeOut) / fadeOutSec;
                wOut = 1.f - SmoothStep01(u);
            }
        }

        const float w = wIn * wOut;

        const float omega = shake.elapsed * shake.frequency * XM_2PI;
        const float a = shake.amplitudeDeg * w;

        sumPitchDeg += sinf(omega + shake.phase.x) * a;
        sumYawDeg += sinf(omega + shake.phase.y) * a;
        sumRollDeg += sinf(omega + shake.phase.z) * a;

        ++i;
    }

    if (sumYawDeg == 0.f && sumPitchDeg == 0.f && sumRollDeg == 0.f) return;

    const float yawRad = XMConvertToRadians(sumYawDeg);
    const float pitchRad = XMConvertToRadians(sumPitchDeg);
    const float rollRad = XMConvertToRadians(sumRollDeg);

    const Quaternion qShake = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, rollRad);

    ioPose.rot = ioPose.rot * qShake;
    ioPose.rot.Normalize();
}

void CCameraMgr::Free()
{
    m_isBlending = false;

    for (size_t i = 0; i < m_overrides.size(); ++i)
        Safe_Release(m_overrides[i].cam);
    m_overrides.clear();

    Safe_Release(m_blendTargetCam);
    Safe_Release(m_baseCam);
    Safe_Release(m_shadowCam);
    m_shakes.clear();
    __super::Free();
}