#include "pch.h"
#include "OrbitCam.h"
#include "GameInstance.h"
#include "CharacterController.h"
#include "IPhysicsService.h"

namespace
{
    inline void DecomposeMatrix(const Matrix& m, Vector3& outScale, Quaternion& outRot, Vector3& outPos)
    {
        XMVECTOR s, r, t;
        const XMMATRIX xm = m;

        const bool ok = XMMatrixDecompose(&s, &r, &t, xm);
        assert(ok);

        outScale = s;
        outRot = r;
        outPos = t;

        outRot.Normalize();
    }
}

COrbitCam* COrbitCam::Create()
{
    auto inst = new COrbitCam();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Object Create Failed : COrbitCam");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* COrbitCam::Clone(INIT_DESC* pArg)
{
    auto inst = new COrbitCam(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Object Clone Failed : COrbitCam");
        Safe_Release(inst);
    }
    return inst;
}

void COrbitCam::Awake()
{
    auto cc = Get_Component<CCharacterController>();
    cc->Resize(0.1f, 0.1f);
    cc->Set_GravityEnabled(false);
}

HRESULT COrbitCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CCharacterController>();
    SetPreset(m_preset, false, true);

    m_curRotDeg = m_targetRotDeg;
    m_curDist = m_targetDist;

    return S_OK;
}

HRESULT COrbitCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

void COrbitCam::SetPreset(OrbitPreset preset, _bool keepZoomRatio, _bool snap)
{
    m_preset = preset;

    const _float oldMin = m_profile.minDist;
    const _float oldMax = m_profile.maxDist;

    m_profile = OrbitPresets::Get(preset);

    if (keepZoomRatio)
    {
        _float n = (m_targetDist - oldMin) / (oldMax - oldMin);
        n = clamp(n, 0.f, 1.f);

        const _float newDist = m_profile.minDist + (m_profile.maxDist - m_profile.minDist) * n;
        m_targetDist = newDist;
        if (snap) m_curDist = newDist;
    }

    ClampTargets();

    if (snap)
    {
        m_targetRotDeg = m_curRotDeg;
        m_targetDist = m_curDist;
        m_targetPivot = m_curPivot;

        m_targetPitchZoomOffset = 0.f;
        m_curPitchZoomOffset = 0.f;

        m_colState.targetDist = 9999.f;
        m_colState.curDist = 9999.f;
    }
}

void COrbitCam::SetTarget(CGameObject* obj)
{
    m_targetHandle = obj->Get_Handle();

    if (m_firstSnap)
    {
        const OrbitProfile p = OrbitPresets::Get(m_preset);
        SetTargetFrontView(obj, p.startDistance, p.startPitchDeg, p.startHeightOffset);
        m_firstSnap = false;
        return;
    }

    const Vector3 pivotTarget = GetPivotTargetPos();
    m_targetPivot = pivotTarget;
    m_curPivot = pivotTarget;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c.x, (float)c.y, (float)c.z);

    Vector3 toPivot = pivotTarget - curPos;
    float dist = toPivot.Length();
    if (dist == 0.f) dist = m_curDist;

    m_curDist = dist;
    m_targetDist = dist;

    toPivot.Normalize();

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    m_curRotDeg.x = XMConvertToDegrees(yawRad);
    m_curRotDeg.y = XMConvertToDegrees(pitchRad);

    m_targetRotDeg = m_curRotDeg;

    ClampTargets();
}

void COrbitCam::SyncFromCurTransform()
{
    m_firstSnap = false;

    auto obj = OBJ->Request_Object(m_targetHandle);
    assert(obj);

    const Vector3 pivotTarget = GetPivotTargetPos();
    m_targetPivot = pivotTarget;
    m_curPivot = pivotTarget;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c.x, (float)c.y, (float)c.z);

    Vector3 toPivot = pivotTarget - curPos;
    float rawDist = toPivot.Length();
    assert(rawDist > 0.f);

    toPivot /= rawDist;

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    m_curRotDeg.x = XMConvertToDegrees(yawRad);
    m_curRotDeg.y = XMConvertToDegrees(pitchRad);

    m_targetRotDeg = m_curRotDeg;

    float zoomOffset = 0.f;
    if (m_profile.usePitchAutoZoom)
    {
        const float pitchAbs = fabsf(m_curRotDeg.y);
        const float pitchLimit = max(fabsf(m_profile.pitchMin), fabsf(m_profile.pitchMax));
        float n = clamp(pitchAbs / pitchLimit, 0.f, 1.f);

        float k = 0.f;
        if (n > m_profile.pitchAutoZoomStartN) k = (n - m_profile.pitchAutoZoomStartN) / (1.f - m_profile.pitchAutoZoomStartN);

        k = clamp(k, 0.f, 1.f);
        k = k * k * (3.f - 2.f * k);

        zoomOffset = -m_profile.pitchAutoZoomMax * k;
    }

    m_curPitchZoomOffset = zoomOffset;
    m_targetPitchZoomOffset = zoomOffset;

    m_curDist = rawDist - zoomOffset;
    m_targetDist = m_curDist;

    const float prePitch = m_targetRotDeg.y;
    const float preRawDist = rawDist;

    ClampTargets();

    const float postEffDist = GetEffectiveDist();
    const float distDiff = fabsf(postEffDist - preRawDist);

    assert(fabsf(m_targetRotDeg.y - prePitch) < 0.001f);
    assert(distDiff < 0.001f && "OrbitCam: dist got clamped (min/max or effectiveDist clamp). Increase maxDist or allow out-of-range on return.");

    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivotTarget.x, pivotTarget.y, pivotTarget.z, 1.f));
}



void COrbitCam::SetTargetFrontView(CGameObject* obj, float distance, float pitchDeg, float heightOffset)
{
    m_targetHandle = obj->Get_Handle();

    const Vector3 pivot = GetPivotTargetPos();
    m_targetPivot = pivot;
    m_curPivot = pivot;

    auto targetTf = obj->Get_Component<CTransform>();
    Vector3 forward = targetTf->Dir(STATE::LOOK);
    forward.y = 0.f;
    forward.Normalize();

    Vector3 camPos = pivot + forward * distance;
    camPos.y += heightOffset;

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(XMVectorSet(camPos.x, camPos.y, camPos.z, 1.f));

    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c.x, (float)c.y, (float)c.z);

    Vector3 toPivot = pivot - curPos;
    float dist = toPivot.Length();
    if (dist == 0.f) dist = m_curDist;

    m_curDist = dist;
    m_targetDist = dist;

    toPivot.Normalize();

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    m_curRotDeg.x = XMConvertToDegrees(yawRad);
    m_curRotDeg.y = pitchDeg;

    m_targetRotDeg = m_curRotDeg;

    m_targetPitchZoomOffset = 0.f;
    m_curPitchZoomOffset = 0.f;

    ClampTargets();

    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::SnapFromCamPose(const Vector3& camPos, const Quaternion& camRot)
{
    m_firstSnap = false;

    const Vector3 pivot = GetPivotTargetPos();
    m_targetPivot = pivot;
    m_curPivot = pivot;

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(XMVectorSet(camPos.x, camPos.y, camPos.z, 1.f));

    Vector3 toPivot = pivot - camPos;
    float dist = toPivot.Length();
    if (dist == 0.f) dist = m_curDist;

    toPivot /= dist;

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    m_curRotDeg.x = XMConvertToDegrees(yawRad);
    m_curRotDeg.y = XMConvertToDegrees(pitchRad);

    m_targetRotDeg = m_curRotDeg;

    m_curPitchZoomOffset = 0.f;
    m_targetPitchZoomOffset = 0.f;

    m_curDist = dist;
    m_targetDist = dist;

    ClampTargets();

    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::Priority_Update(_float dt)
{
    m_targetPivot = GetPivotTargetPos();

    UpdateInput(dt);
    ClampTargets();
    SmoothStates(dt);
    ApplyOrbitPose(dt);
}

void COrbitCam::UpdateInput(_float dt)
{
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        m_targetRotDeg.x += KEY->Mouse_DeltaX() * m_sensitivityX;
        m_targetRotDeg.y += KEY->Mouse_DeltaY() * m_sensitivityY;
    }

    const float zoomDelta = m_zoomSpeed * dt;
    if (KEY->Key_Down('Q')) m_targetDist += zoomDelta;
    if (KEY->Key_Down('E')) m_targetDist -= zoomDelta;

    if (!m_profile.usePitchAutoZoom) { m_targetPitchZoomOffset = 0.f; return; }

    const float pitchAbs = fabsf(m_targetRotDeg.y);
    const float pitchLimit = max(fabsf(m_profile.pitchMin), fabsf(m_profile.pitchMax));

    float n = clamp(pitchAbs / pitchLimit, 0.f, 1.f);

    float k = 0.f;
    if (n > m_profile.pitchAutoZoomStartN) k = (n - m_profile.pitchAutoZoomStartN) / (1.f - m_profile.pitchAutoZoomStartN);

    k = clamp(k, 0.f, 1.f);
    k = k * k * (3.f - 2.f * k);

    m_targetPitchZoomOffset = -m_profile.pitchAutoZoomMax * k;
}

void COrbitCam::ClampTargets()
{
    m_targetRotDeg.y = clamp(m_targetRotDeg.y, m_profile.pitchMin, m_profile.pitchMax);
    m_targetDist = clamp(m_targetDist, m_profile.minDist, m_profile.maxDist);
}

void COrbitCam::SmoothStates(_float dt)
{
    float rot = 1.f - expf(-m_profile.rotSmoothSpeed * dt);
    rot = clamp(rot, 0.f, 1.f);
    m_curRotDeg = m_curRotDeg + (m_targetRotDeg - m_curRotDeg) * rot;

    float dist = 1.f - expf(-m_profile.distSmoothSpeed * dt);
    dist = clamp(dist, 0.f, 1.f);
    m_curDist = m_curDist + (m_targetDist - m_curDist) * dist;

    float zoom = 1.f - expf(-m_profile.pitchAutoZoomSmooth * dt);
    zoom = clamp(zoom, 0.f, 1.f);
    m_curPitchZoomOffset = m_curPitchZoomOffset + (m_targetPitchZoomOffset - m_curPitchZoomOffset) * zoom;

    float pivot = 1.f - expf(-m_profile.pivotSmoothSpeed * dt);
    pivot = clamp(pivot, 0.f, 1.f);
    m_curPivot = m_curPivot + (m_targetPivot - m_curPivot) * pivot;
}

Vector3 COrbitCam::GetPivotTargetPos() const
{
    auto obj = OBJ->Request_Object(m_targetHandle);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    const Vector3 foot{foot4.x, foot4.y, foot4.z};

    return foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + m_profile.offsetY, 0.f);
}
 
float COrbitCam::GetEffectiveDist() const
{
    return clamp(m_curDist + m_curPitchZoomOffset, m_profile.minDist, m_profile.maxDist);
}

void COrbitCam::ApplyOrbitPose(_float dt)
{
    const Vector3 pivot = GetPivotPos();

    const float yawRad = XMConvertToRadians(m_curRotDeg.x);
    const float pitchRad = XMConvertToRadians(m_curRotDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const float dist = GetEffectiveDist();
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 desiredPos = pivot + backDir * dist;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c0 = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c0.x, (float)c0.y, (float)c0.z);

    const Vector3 disp = desiredPos - curPos;
    cc->Move_Displacement(XMVectorSet(disp.x, disp.y, disp.z, 0.f), dt);

    const PxExtendedVec3& c1 = cc->Get_Controller()->getPosition();
    m_pTransform->Set_WorldPos(XMVectorSet((float)c1.x, (float)c1.y, (float)c1.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}