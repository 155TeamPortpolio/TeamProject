#include "pch.h"
#include "OrbitCam.h"

#include "GameInstance.h"
#include "Character.h"
#include "Helper_Func.h"
#include "PhysicsSystem.h"
#include "CharacterController.h"
#include "EventListener.h"
// Interface
#include "ICamCollidable.h"

void COrbitCam::Awake()
{
    auto cc = Get_Component<CCharacterController>();
    cc->Resize(0.2f, 0.2f);
    cc->Set_GravityEnabled(false);
    cc->Set_StepOffset(0.f);
    cc->Set_SlopeLimit(89.f);
    cc->Set_MinMoveDist(0.01f);
    cc->Set_ContactOffset(0.001f);
    cc->Set_RestOffset(0.f);
}

HRESULT COrbitCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CCharacterController>();
    Add_Component<CEventListener>();

    m_pose.rotGoalDeg = Vector2(0.f, m_prof.startPitchDeg);
    m_pose.rotCurDeg = m_pose.rotGoalDeg;

    m_pose.distWanted = m_prof.startDist;
    m_pose.distGoal = m_pose.distWanted;
    m_pose.distCur = m_pose.distGoal;

    m_pose.pivotGoalWorld = Vector3::Zero;
    m_pose.pivotCurWorld = m_pose.pivotGoalWorld;

    m_pose.pivotInternalOffset = Vector3::Zero;
    m_pose.pivotExternalOffset = Vector3::Zero;

    Lock_Reset();
    AutoYaw_OnTarget();
    Switch_Reset();

    ClampTargets();
    return S_OK;
}

HRESULT COrbitCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    auto event = Get_Component<CEventListener>();
    event->Add_Listener<TARGET_LOCK_DESC>([&](TARGET_LOCK_DESC desc)
        {
            if (!desc.tHandle.isValid()) return;
            if (desc.bLock) SetLockOn(desc.tHandle);
            else ClearLockOn();
        });

    return S_OK;
}

void COrbitCam::SetTarget(OBJECT_HANDLE h)
{
    if (!h.isValid()) return;

    const float keepWantedDist = m_pose.distWanted;

    if (!m_target.isValid())
    {
        m_target = h;

        Switch_Reset();
        AutoYaw_OnTarget();

        m_pose.pivotInternalOffset = Vector3::Zero;
        m_pose.rotGoalDeg = m_pose.rotCurDeg;

        m_pose.distWanted = clamp(keepWantedDist, m_prof.distMin, m_prof.distMax);
        m_pose.distGoal = clamp(m_pose.distGoal, m_prof.distMin, m_prof.distMax);
        m_pose.distCur = clamp(m_pose.distCur, m_prof.distMin, m_prof.distMax);

        ClampTargets();
        return;
    }

    if (h == m_target) return;

    const Vector3 holdPivotWorld = m_pose.pivotCurWorld;

    m_target = h;
    AutoYaw_OnTarget();

    Switch_Begin(holdPivotWorld);
    m_pose.rotGoalDeg = m_pose.rotCurDeg;

    m_pose.distWanted = clamp(keepWantedDist, m_prof.distMin, m_prof.distMax);
    m_pose.distGoal = clamp(m_pose.distGoal, m_prof.distMin, m_prof.distMax);
    m_pose.distCur = clamp(m_pose.distCur, m_prof.distMin, m_prof.distMax);

    ClampTargets();
}

void COrbitCam::ClearTarget()
{
    m_target.Reset();
    Lock_Clear();
    Switch_Reset();
}

void COrbitCam::SetLockOn(OBJECT_HANDLE h)
{
    if (!h.isValid()) return;

    if (!Lock_Active())
    {
        Lock_Enter(h, m_pose.distGoal);
        return;
    }

    if (h == Lock_Handle()) return;

    Lock_Switch(h);
}

void COrbitCam::ClearLockOn()
{
    if (!Lock_On()) return;

    Lock_Exit();
    AutoYaw_OnInput();
}

void COrbitCam::CaptureSnapshot(OrbitSnapshot& out) const
{
    out.pose = m_pose;
    out.lock = m_lock;
    out.lockBlend = m_lockBlend;
    out.autoYaw = m_autoYaw;
    out.sw = m_switch;
    out.target = m_target;
}

void COrbitCam::RestoreSnapshot(const OrbitSnapshot& s)
{
    m_target = s.target;
    m_pose = s.pose;

    m_lock = s.lock;
    m_lockBlend = s.lockBlend;
    m_autoYaw = s.autoYaw;
    m_switch = s.sw;

    m_lockFocus = {};
    m_hasLockFocus = false;

    SyncPivot();

    auto cc = Get_Component<CCharacterController>();

    const float yawRad = XMConvertToRadians(m_pose.rotCurDeg.x);
    const float pitchRad = XMConvertToRadians(m_pose.rotCurDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 camPos = m_pose.pivotCurWorld + backDir * m_pose.distCur;

    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    Lock_BlendUpdate(0.f);

    OrbitLockEval lockRes{};
    if (Lock_On())
        lockRes = EvalLock(0.f, m_pose.rotCurDeg.x, m_pose.distCur);

    ApplyPose(0.f, lockRes);
}

void COrbitCam::SyncFromCurTransform()
{
    SyncPivot();

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& p = cc->Get_Controller()->getPosition();
    const Vector3 camPos((float)p.x, (float)p.y, (float)p.z);

    Vector3 toPivot = m_pose.pivotCurWorld - camPos;
    const float dist = toPivot.Length();
    toPivot /= dist;

    Vector2 rotDeg{};
    CalcRotDeg(toPivot, rotDeg);

    m_pose.rotCurDeg = rotDeg;
    m_pose.rotGoalDeg = rotDeg;

    m_pose.distCur = dist;
    m_pose.distGoal = dist;
    m_pose.distWanted = dist;

    ClampTargets();

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(m_pose.pivotCurWorld.x, m_pose.pivotCurWorld.y, m_pose.pivotCurWorld.z, 1.f));
}

void COrbitCam::SnapFromCamPose(const Vector3& pos, const Quaternion& rot)
{
    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(Vector4(pos.x, pos.y, pos.z, 1.f));

    Vector3 footWorld{};
    Vector3 basePivotWorld{};
    GetBasePivot(m_target, footWorld, basePivotWorld);

    SnapPose(pos, rot, basePivotWorld);

    m_yawDeltaCapDeg = m_prof.yawDeltaCapDeg;
    m_pitchDeltaCapDeg = m_prof.pitchDeltaCapDeg;

    ClampTargets();

    m_pTransform->Set_WorldPos(Vector4(pos.x, pos.y, pos.z, 1.f));
    m_pTransform->LookAt(Vector4(m_pose.pivotCurWorld.x, m_pose.pivotCurWorld.y, m_pose.pivotCurWorld.z, 1.f));
}

_bool COrbitCam::SkipUpdate(_float dt)
{
    if (!m_target.isValid()) return true;

    if (m_freeze > 0.f)
    {
        m_freeze -= dt;
        return true;
    }

    if (Lock_On() && !Lock_Handle().isValid())
        ClearLockOn();

    return false;
}

void COrbitCam::UpdateSwitch(_float dt)
{
    if (!Switch_Active()) return;

    const Vector3 basePivotNow = GetBasePivotTargetPos(m_target);
    m_pose.pivotInternalOffset = Switch_EvalOffset(dt, basePivotNow);
}

void COrbitCam::ApplyInput(_float dt)
{
    const float lockW = Lock_Weight();
    const OrbitInputEval inRes = EvalInput(dt, lockW);

    m_pose.rotGoalDeg.x += inRes.yawDeltaDeg;
    m_pose.rotGoalDeg.y += inRes.pitchDeltaDeg;
    m_pose.distWanted -= inRes.zoomDelta;
}

OrbitLockEval COrbitCam::ApplyLock(_float dt)
{
    Lock_BlendUpdate(dt);

    OrbitLockEval lockRes{};
    if (!Lock_On()) return lockRes;

    lockRes = EvalLock(dt, m_pose.rotGoalDeg.x, m_pose.distWanted);

    m_pose.rotGoalDeg.x += lockRes.yawAddDeg;
    if (lockRes.hasDist) m_pose.distWanted = lockRes.dist;

    return lockRes;
}

void COrbitCam::ApplyAutoYaw(_float dt, const OrbitLockEval& lockRes)
{
    if (lockRes.weight > 0.f) return;

    const Vector3 foot = GetFoot();
    const Vector3 camLook = m_pTransform->Dir(STATE::LOOK);
    const Vector3 camRight = m_pTransform->Dir(STATE::RIGHT);

    m_pose.rotGoalDeg.x += EvalAutoYaw(dt, foot, camLook, camRight, m_pose.rotGoalDeg.x);
}

void COrbitCam::ApplyCollide(_float dt)
{
    auto cc = Get_Component<CCharacterController>();
    auto scene = PhysicsSystem()->Get_Scene();

    const OrbitCollideEval res = EvalCollideDist(dt, m_prof, scene, cc, m_pose.pivotGoalWorld, m_pose.distWanted, m_pose.rotCurDeg, m_pose.rotGoalDeg, m_pose.distGoal);

    m_hitDist = res.hit;

    m_yawDeltaCapDeg = res.yawDeltaCapDeg;
    m_pitchDeltaCapDeg = res.pitchDeltaCapDeg;

    m_pose.distGoal = res.goalDist;
}

void COrbitCam::Priority_Update(_float dt)
{
    if (SkipUpdate(dt)) return;

    UpdateSwitch(dt);

    m_pose.pivotGoalWorld = GetPivotTargetPos();

    ApplyInput(dt);

    const OrbitLockEval lockRes = ApplyLock(dt);

    ApplyAutoYaw(dt, lockRes);

    ClampTargets();

    ApplyCollide(dt);

    SmoothPose(dt);

    ApplyPose(dt, lockRes);
}

void COrbitCam::OnTriggerEnter(CGameObject* obj)
{
    auto cam = dynamic_cast<ICamCollidable*>(obj);
    if (!cam) return;

    auto& count = m_camOcclusionRefCount[cam];
    count++;

    if (count == 1)
        cam->OnCameraCollision(true);
}

void COrbitCam::OnTriggerStay(CGameObject* obj)
{
    auto cam = dynamic_cast<ICamCollidable*>(obj);
    if (!cam) return;

    auto it = m_camOcclusionRefCount.find(cam);
    if (it != m_camOcclusionRefCount.end()) return;

    m_camOcclusionRefCount[cam] = 1;
    cam->OnCameraCollision(true);
}

void COrbitCam::OnTriggerExit(CGameObject* obj)
{
    auto cam = dynamic_cast<ICamCollidable*>(obj);
    if (!cam) return;

    auto it = m_camOcclusionRefCount.find(cam);
    if (it == m_camOcclusionRefCount.end()) return;

    it->second--;

    if (it->second > 0) return;

    m_camOcclusionRefCount.erase(it);
    cam->OnCameraCollision(false);
}


void COrbitCam::ClampTargets()
{
    m_pose.rotGoalDeg.y = clamp(m_pose.rotGoalDeg.y, m_prof.pitchLimitMinDeg, m_prof.pitchLimitMaxDeg);
    m_pose.distWanted = clamp(m_pose.distWanted, m_prof.distMin, m_prof.distMax);
}

Vector3 COrbitCam::GetPivotTargetPos() const
{
    const Vector3 basePivot = GetBasePivotTargetPos(m_target);
    return basePivot + m_pose.pivotInternalOffset + m_pose.pivotExternalOffset;
}

float COrbitCam::GetDist() const
{
    if (m_pose.distCur > m_prof.distMax) return m_prof.distMax;
    return m_pose.distCur;
}

void COrbitCam::ApplyPose(_float dt, const OrbitLockEval& lockRes)
{
    const Vector3 pivot = GetPivotPos();

    const float yawRad = XMConvertToRadians(m_pose.rotCurDeg.x);
    const float pitchRad = XMConvertToRadians(m_pose.rotCurDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const float dist = GetDist();
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 camPos = pivot + backDir * dist;

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(Vector4(camPos.x, camPos.y, camPos.z, 1.f));

    Vector3 lookAt = pivot;
    if (lockRes.weight > 0.f) lookAt = Vector3::Lerp(pivot, lockRes.focusPos, lockRes.weight);

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(lookAt.x, lookAt.y, lookAt.z, 1.f));
}

Vector3 COrbitCam::GetFoot() const
{
    auto obj = ObjectManager()->Request_Object(m_target);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    return Vector3(foot4.x, foot4.y, foot4.z);
}

Vector3 COrbitCam::GetBasePivotTargetPos(OBJECT_HANDLE h) const
{
    auto obj = ObjectManager()->Request_Object(h);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    const Vector3 foot{foot4.x, foot4.y, foot4.z};

    return foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + m_prof.offsetY, 0.f);
}

void COrbitCam::AutoYaw_OnTarget()
{
    m_autoYaw.holdTimer = m_prof.autoYawDelay;
    m_autoYaw.hasPrevFoot = false;
    m_autoYaw.prevFootWorld = {};
}

void COrbitCam::AutoYaw_OnInput()
{
    m_autoYaw.holdTimer = m_prof.autoYawDelay;
}

_float COrbitCam::EvalAutoYaw(_float dt, const Vector3& foot, const Vector3& camLookWorld, const Vector3& camRightWorld, _float curYawDeg)
{
    if (!m_prof.autoYaw) return 0.f;

    if (m_autoYaw.holdTimer > 0.f)
    {
        m_autoYaw.holdTimer -= dt;
        return 0.f;
    }

    if (!m_autoYaw.hasPrevFoot)
    {
        m_autoYaw.prevFootWorld = foot;
        m_autoYaw.hasPrevFoot = true;
        return 0.f;
    }

    Vector3 move = foot - m_autoYaw.prevFootWorld;
    m_autoYaw.prevFootWorld = foot;

    move.y = 0.f;

    const float len = move.Length();
    if (len == 0.f) return 0.f;

    move /= len;

    Vector3 camLook = camLookWorld;
    Vector3 camRight = camRightWorld;

    camLook.y = 0.f;
    camRight.y = 0.f;

    camLook.Normalize();
    camRight.Normalize();

    const float localZ = move.Dot(camLook);
    const float localX = move.Dot(camRight);

    if (localZ < 0.f && fabsf(localZ) > fabsf(localX)) return 0.f;

    const float desiredYawDeg = XMConvertToDegrees(atan2f(move.x, move.z));
    const float deltaYawDeg = Math::WrapDeg(desiredYawDeg - curYawDeg);

    float a = 1.f - expf(-m_prof.autoYawSpeed * dt);
    a = clamp(a, 0.f, 1.f);

    return deltaYawDeg * a;
}

void COrbitCam::Switch_Reset()
{
    m_switch = {};
}

void COrbitCam::Switch_Begin(const Vector3& holdPivotWorld)
{
    m_switch.active = true;
    m_switch.elapsed = 0.f;
    m_switch.holdPivotWorld = holdPivotWorld;
}

Vector3 COrbitCam::Switch_EvalOffset(_float dt, const Vector3& basePivotNow)
{
    if (!m_switch.active) return Vector3::Zero;

    m_switch.elapsed += dt;

    _float ratio = m_switch.elapsed / m_prof.switchBlendSec;
    if (ratio >= 1.f)
    {
        m_switch.active = false;
        return Vector3::Zero;
    }

    ratio = clamp(ratio, 0.f, 1.f);
    ratio = Math::ApplyEase(m_prof.switchEase, ratio);

    const Vector3 keepOff = m_switch.holdPivotWorld - basePivotNow;
    return Vector3::Lerp(keepOff, Vector3::Zero, ratio);
}

OrbitInputEval COrbitCam::EvalInput(_float dt, _float lockW)
{
    OrbitInputEval out{};

    if (m_lockInput) return out;

#ifdef _USING_GUI
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered()) return out;
#endif

    const float mouseDx = InputDevice()->Mouse_DeltaX();
    const float mouseDy = InputDevice()->Mouse_DeltaY();
    const float wheel = InputDevice()->Mouse_DeltaW() * 0.5f;

    if (lockW <= 0.f) out.yawDeltaDeg = mouseDx * m_input.sensX;
    out.pitchDeltaDeg = mouseDy * m_input.sensY;

    if (wheel != 0.f) out.zoomDelta = wheel * m_input.zoomSpeed;

    if (mouseDx != 0.f || mouseDy != 0.f || wheel != 0.f) AutoYaw_OnInput();

    const float yawCapThisFrame = m_yawDeltaCapDeg * dt;
    const float pitchCapThisFrame = m_pitchDeltaCapDeg * dt;

    out.yawDeltaDeg = clamp(out.yawDeltaDeg, -yawCapThisFrame, yawCapThisFrame);
    out.pitchDeltaDeg = clamp(out.pitchDeltaDeg, -pitchCapThisFrame, pitchCapThisFrame);

    return out;
}

OrbitCollideEval COrbitCam::EvalCollideDist(_float dt, const OrbitProfile& prof, PxScene* scene, Engine::CCharacterController* camCC,
    const Vector3& pivotWorld, _float distWanted, const Vector2& rotCurDeg, const Vector2& rotGoalDeg, _float distGoal)
{
    OrbitCollideEval out{};

    const _float allowed = CalcAllowDist(prof, scene, camCC, pivotWorld, distWanted, rotCurDeg, rotGoalDeg);

    out.allowedDist = allowed;
    out.hit = (allowed < distWanted - 0.001f);

    out.yawDeltaCapDeg = out.hit ? prof.yawHitDeltaCapDeg : prof.yawDeltaCapDeg;
    out.pitchDeltaCapDeg = out.hit ? prof.pitchHitDeltaCapDeg : prof.pitchDeltaCapDeg;

    const _float targetDist = min(distWanted, allowed);
    const _float zoomSpeed = (targetDist < distGoal) ? prof.zoomInCollide : prof.zoomOutCollide;

    _float nextGoal = Math::MoveTowards(distGoal, targetDist, zoomSpeed * dt);
    nextGoal = clamp(nextGoal, prof.distMin, prof.distMax);

    out.goalDist = nextGoal;
    return out;
}

_float COrbitCam::CalcAllowDist(const OrbitProfile& prof, PxScene* scene, Engine::CCharacterController* camCC,
    const Vector3& pivotWorld, _float distWanted, const Vector2& rotCurDeg, const Vector2& rotGoalDeg)
{
    const _float camRadius = camCC->Get_Radius();
    const _float padding = 0.1f;
    const _float stepDeg = 4.f;

    if (!scene) return distWanted;

    const _float startYaw = rotCurDeg.x;
    const _float startPitch = rotCurDeg.y;

    const _float endYaw = rotGoalDeg.x;
    const _float endPitch = rotGoalDeg.y;

    const _float deltaYaw = Math::WrapDeg(endYaw - startYaw);
    const _float deltaPitch = endPitch - startPitch;

    const _float maxAbs = max(fabsf(deltaYaw), fabsf(deltaPitch));
    _int steps = (_int)ceilf(maxAbs / stepDeg);
    if (steps < 1) steps = 1;
    if (steps > 12) steps = 12;

    PxSphereGeometry geom(camRadius);

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER;

    CRaycastFilterCallback filterCallback(ENUM(COLLISION_GROUP::COMMON), false);

    _float minAllowed = distWanted;

    for (_int i = 1; i <= steps; ++i)
    {
        const _float ratio = (_float)i / (_float)steps;

        const _float yawRad = XMConvertToRadians(startYaw + deltaYaw * ratio);
        const _float pitchRad = XMConvertToRadians(startPitch + deltaPitch * ratio);

        const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

        Vector3 dir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
        dir.Normalize();

        PxTransform posePx(PxVec3(pivotWorld.x, pivotWorld.y, pivotWorld.z));

        PxSweepBuffer hit;
        PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

        const _float sweepDist = distWanted + padding;

        const _bool ok = scene->sweep(geom, posePx, PxVec3(dir.x, dir.y, dir.z), sweepDist, hit, hitFlags, filterData, &filterCallback);
        if (!ok || !hit.hasBlock) continue;

        _float allowed = hit.block.distance - padding;
        allowed = clamp(allowed, prof.distMin, distWanted);

        if (allowed < minAllowed) minAllowed = allowed;
    }

    return minAllowed;
}


void COrbitCam::SmoothPose(_float dt)
{
    float rotA = 1.f - expf(-m_prof.rotSmooth * dt);
    rotA = clamp(rotA, 0.f, 1.f);
    m_pose.rotCurDeg = m_pose.rotCurDeg + (m_pose.rotGoalDeg - m_pose.rotCurDeg) * rotA;

    float distA = 1.f - expf(-m_prof.distSmooth * dt);
    distA = clamp(distA, 0.f, 1.f);
    m_pose.distCur = m_pose.distCur + (m_pose.distGoal - m_pose.distCur) * distA;

    float pivotA = 1.f - expf(-m_prof.pivotSmooth * dt);
    pivotA = clamp(pivotA, 0.f, 1.f);
    m_pose.pivotCurWorld = m_pose.pivotCurWorld + (m_pose.pivotGoalWorld - m_pose.pivotCurWorld) * pivotA;
}

void COrbitCam::Lock_Reset()
{
    m_lock = {};
    m_lockBlend = {};
    m_lockFocus = {};
    m_hasLockFocus = false;
}

void COrbitCam::Lock_Enter(OBJECT_HANDLE h, _float curDist)
{
    m_lock.active = true;
    m_lock.handle = h;
    m_lock.savedDist = curDist;

    Lock_BlendStart(true);
}

void COrbitCam::Lock_Exit()
{
    if (!Lock_On()) return;

    if (!m_lockBlend.active)
    {
        Lock_BlendStart(false);
        return;
    }

    if (m_lockBlend.entering) Lock_BlendStart(false);
}

void COrbitCam::Lock_Clear()
{
    Lock_Reset();
}

void COrbitCam::Lock_BlendUpdate(_float dt)
{
    if (!m_lockBlend.active)
    {
        m_lockBlend.weight = m_lock.active ? 1.f : 0.f;
        return;
    }

    m_lockBlend.elapsed += dt;

    _float ratio = m_lockBlend.elapsed / m_lockBlend.duration;
    if (ratio >= 1.f)
    {
        m_lockBlend.active = false;

        if (m_lockBlend.entering) m_lockBlend.weight = 1.f;
        else
        {
            m_lockBlend.weight = 0.f;
            m_lock = {};
            m_lockFocus = {};
            m_hasLockFocus = false;
        }
        return;
    }

    ratio = clamp(ratio, 0.f, 1.f);
    const _float e = Math::ApplyEase(m_lockBlend.ease, ratio);

    m_lockBlend.weight = m_lockBlend.entering ? e : (1.f - e);
}

OrbitLockEval COrbitCam::EvalLock(_float dt, _float curYawDeg, _float curDist)
{
    OrbitLockEval out{};

    const _float w = Lock_Weight();
    out.weight = w;

    if (!m_target.isValid()) return out;

    const Vector3 playerPivot = GetBasePivotTargetPos(m_target);

    out.focusPos = m_hasLockFocus ? m_lockFocus : playerPivot;

    if (m_lockBlend.active && !m_lockBlend.entering)
    {
        out.hasDist = true;
        const _float k = 1.f - w;
        out.dist = curDist + (m_lock.savedDist - curDist) * k;
    }

    if (w <= 0.f) return out;
    if (!m_lock.handle.isValid()) return out;

    const Vector3 targetPivot = GetBasePivotTargetPos(m_lock.handle);

    Vector3 flat = targetPivot - playerPivot;
    flat.y = 0.f;

    const _float len = flat.Length();
    if (len == 0.f) return out;

    flat /= len;

    const _float desiredYawDeg = XMConvertToDegrees(atan2f(flat.x, flat.z));
    const _float deltaYawDeg = Math::WrapDeg(desiredYawDeg - curYawDeg);

    _float a = 1.f - expf(-m_prof.lockYawSpeed * dt);
    a = clamp(a, 0.f, 1.f);

    out.yawAddDeg = deltaYawDeg * a * w;

    _float distT = len / (len + m_prof.lockFocusDist);
    distT = clamp(distT, 0.f, 1.f);

    const _float focusT = m_prof.lockFocusNear + (m_prof.lockFocusFar - m_prof.lockFocusNear) * distT;
    out.focusPos = Vector3::Lerp(playerPivot, targetPivot, focusT);

    m_lockFocus = out.focusPos;
    m_hasLockFocus = true;

    if (m_prof.lockAutoZoom && !out.hasDist)
    {
        const _float wanted = len * m_prof.lockAutoZoomFactor;
        const _float clampedDist = clamp(wanted, m_prof.distMin, m_prof.distMax);

        if (curDist < clampedDist)
        {
            out.hasDist = true;
            out.dist = curDist + (clampedDist - curDist) * w;
        }
    }

    return out;
}

void COrbitCam::Lock_BlendStart(_bool entering)
{
    m_lockBlend.active = true;
    m_lockBlend.entering = entering;
    m_lockBlend.elapsed = 0.f;

    if (entering)
    {
        m_lockBlend.duration = m_prof.lockBlendIn;
        m_lockBlend.ease = m_prof.lockBlendInEase;
        m_lockBlend.weight = 0.f;
    }
    else
    {
        m_lockBlend.duration = m_prof.lockBlendOut;
        m_lockBlend.ease = m_prof.lockBlendOutEase;
        m_lockBlend.weight = 1.f;
    }

    if (m_lockBlend.duration <= 0.f) m_lockBlend.duration = 0.0001f;
}

void COrbitCam::SyncPivot()
{
    const Vector3 pivot = GetPivotTargetPos();
    m_pose.pivotGoalWorld = pivot;
    m_pose.pivotCurWorld = pivot;
}

void COrbitCam::CalcRotDeg(const Vector3& lookDir, Vector2& outRotDeg) const
{
    const float yawRad = atan2f(lookDir.x, lookDir.z);
    const float pitchRad = asinf(clamp(-lookDir.y, -1.f, 1.f));

    outRotDeg.x = XMConvertToDegrees(yawRad);
    outRotDeg.y = XMConvertToDegrees(pitchRad);
}

void COrbitCam::GetBasePivot(const OBJECT_HANDLE& h, Vector3& outFootWorld, Vector3& outBasePivotWorld) const
{
    auto obj = ObjectManager()->Request_Object(h);
    auto cc = obj->Get_Component<Engine::CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    outFootWorld = Vector3(foot4.x, foot4.y, foot4.z);

    outBasePivotWorld = outFootWorld + Vector3(0.f, cc->Get_HalfSize() * 1.5f + m_prof.offsetY, 0.f);
}

void COrbitCam::SnapPose(const Vector3& camPos, const Quaternion& camRot, const Vector3& basePivotWorld)
{
    Vector3 forward = Vector3::Transform(Vector3(0.f, 0.f, 1.f), camRot);
    forward.Normalize();

    float dist = (basePivotWorld - camPos).Length();
    dist = clamp(dist, m_prof.distMin, m_prof.distMax);

    const Vector3 holdPivotWorld = camPos + forward * dist;

    Switch_Reset();

    m_pose.pivotExternalOffset = Vector3::Zero;
    m_pose.pivotInternalOffset = holdPivotWorld - basePivotWorld;

    m_pose.pivotCurWorld = holdPivotWorld;
    m_pose.pivotGoalWorld = holdPivotWorld;

    Vector3 toPivot = holdPivotWorld - camPos;
    const float rawDist = toPivot.Length();
    toPivot /= rawDist;

    Vector2 rotDeg{};
    CalcRotDeg(toPivot, rotDeg);

    m_pose.rotCurDeg = rotDeg;
    m_pose.rotGoalDeg = rotDeg;

    m_pose.distCur = dist;
    m_pose.distGoal = dist;
    m_pose.distWanted = dist;
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