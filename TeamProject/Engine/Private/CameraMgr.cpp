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
}

void CCameraMgr::Set_MainCam(CCamera* camComp)
{
    if (m_baseCam)
        Safe_Release(m_baseCam);

    m_baseCam = camComp;

    if (m_baseCam)
        Safe_AddRef(m_baseCam);

    if (m_overrides.empty())
        BeginBlendTo(GetDesiredActiveCam(), 0.f);
}

void CCameraMgr::Set_ShadowCam(CCamera* camComp)
{
    if (m_shadowCam)
        Safe_Release(m_shadowCam);

    m_shadowCam = camComp;

    if (m_shadowCam)
        Safe_AddRef(m_shadowCam);
}

_uint CCameraMgr::Push(CCamera* camComp, _float blendSec)
{
    if (!camComp) return 0u;

    OverrideEntry entry{};
    entry.handle = m_nextHandle++;
    entry.cam = camComp;

    Safe_AddRef(entry.cam);
    m_overrides.push_back(entry);

    BeginBlendTo(GetDesiredActiveCam(), blendSec);
    return entry.handle;
}

_bool CCameraMgr::Pop(_uint handle, _float blendSec)
{
    if (handle == 0u) return false;

    size_t idx = (size_t)-1;
    for (size_t i = 0; i < m_overrides.size(); ++i)
    {
        if (m_overrides[i].handle == handle)
        {
            idx = i;
            break;
        }
    }

    if (idx == (size_t)-1) return false;

    const bool wasTop = (idx + 1 == m_overrides.size());

    if (m_overrides[idx].cam)
        Safe_Release(m_overrides[idx].cam);

    m_overrides.erase(m_overrides.begin() + (ptrdiff_t)idx);

    if (wasTop)
        BeginBlendTo(GetDesiredActiveCam(), blendSec);

    return true;
}

void CCameraMgr::Clear(_float blendSec)
{
    for (size_t i = 0; i < m_overrides.size(); ++i)
        if (m_overrides[i].cam)
            Safe_Release(m_overrides[i].cam);

    m_overrides.clear();
    BeginBlendTo(GetDesiredActiveCam(), blendSec);
}

void CCameraMgr::Update(_float dt)
{
    if (m_isBlending)
    {
        m_blendTime += dt;

        float t = 1.f;
        if (m_blendDuration > 1e-6f)
            t = m_blendTime / m_blendDuration;

        t = clamp(t, 0.f, 1.f);

        if (m_easeType != EaseType::None && t > 0.f && t < 1.f)
        {
            t = Math::ApplyEase(m_easeType, t);
            t = clamp(t, 0.f, 1.f);
        }

        const CamPoseFrame toPose = CapturePose(m_blendTargetCam);
        ApplyOutputPose(BlendPose(m_blendFrom, toPose, t));

        if (t >= 1.f)
        {
            m_isBlending = false;

            if (m_blendTargetCam)
                Safe_Release(m_blendTargetCam);

            m_blendTargetCam = nullptr;
        }
    }
    else
        ApplyOutputPose(CapturePose(GetDesiredActiveCam()));

    UpdateShadowCache();
}

CCamera* CCameraMgr::GetDesiredActiveCam() const
{
    if (!m_overrides.empty())
        return m_overrides.back().cam;

    return m_baseCam;
}

CCameraMgr::CamPoseFrame CCameraMgr::CapturePose(CCamera* cam) const
{
    CamPoseFrame out{};
    if (!cam) return out;

    CTransform* tr = cam->Get_Owner()->Get_Component<CTransform>();
    assert(tr);

    Matrix world = Matrix(tr->Get_WorldMatrix());

    Vector3 scale{};
    Vector3 translation{};
    Quaternion rotation = Quaternion::Identity;

    const bool ok = world.Decompose(scale, rotation, translation);
    assert(ok);

    rotation.Normalize();

    out.pos = translation;
    out.rot = rotation;

    out.lens.projType = cam->Get_ProjType();
    out.lens.fov = cam->Get_FOV();
    out.lens.nearZ = cam->Get_Near();
    out.lens.farZ = cam->Get_Far();
    out.lens.aspect = cam->Get_Aspect();

    if (out.lens.projType == CamProjType::Orthographic)
    {
        const Matrix proj = cam->Get_ProjMatrix();
        const float m22 = proj._22;
        if (fabsf(m22) > 1e-6f)
            out.lens.orthoHeight = 2.f / m22;
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

    out.lens.nearZ = a.lens.nearZ + (b.lens.nearZ - a.lens.nearZ) * t;
    out.lens.farZ = a.lens.farZ + (b.lens.farZ - a.lens.farZ) * t;
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
        const float width = height * pose.lens.aspect;
        m_proj = XMMatrixOrthographicLH( width,  height, pose.lens.nearZ, pose.lens.farZ );
    }

    m_invProj = m_proj.Invert();

    m_camPos = Vector4(pose.pos.x, pose.pos.y, pose.pos.z, 1.f);
    m_activeFar = pose.lens.farZ;
}

void CCameraMgr::BeginBlendTo(CCamera* targetCam, _float blendSec)
{
    if (m_blendTargetCam)
        Safe_Release(m_blendTargetCam);

    m_blendTargetCam = nullptr;

    if (!targetCam || blendSec <= 1e-6f)
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

void CCameraMgr::Free()
{
    m_isBlending = false;

    if (m_blendTargetCam)
        Safe_Release(m_blendTargetCam);

    m_blendTargetCam = nullptr;

    if (m_baseCam)
        Safe_Release(m_baseCam);

    if (m_shadowCam)
        Safe_Release(m_shadowCam);
}