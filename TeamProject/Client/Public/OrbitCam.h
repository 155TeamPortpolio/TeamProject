#pragma once

#include "CamObject.h"
#include "OrbitCamTypes.h"

NS_BEGIN(Client)

class COrbitCam final : public CCamObject
{
private:
    COrbitCam() {}
    COrbitCam(const COrbitCam& rhs) : CCamObject(rhs) {}
    virtual ~COrbitCam() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake()                     override;
    void    Priority_Update(_float dt)  override;

public:
    void    SetTarget(OBJECT_HANDLE h);
    void    ClearTarget();

    void    SyncFromCurTransform();
    void    SnapFromCamPose(const Vector3& pos, const Quaternion& rot);

    void    CaptureSnapshot(OrbitSnapshot& out) const;
    void    RestoreSnapshot(const OrbitSnapshot& s);

    void    FreezeFor(_float sec) { if (sec > m_freeze) m_freeze = sec; }

    void    SetLockOn(OBJECT_HANDLE h);
    void    ClearLockOn();

    void    Lock_Input() { m_lockInput = true; }
    void    Unlock_Input() { m_lockInput = false; }
    _bool   IsInputLocked() const { return m_lockInput; }

    void    SetPivotExt(const Vector3& off) { m_pose.pivotExternalOffset = off; }
    Vector3 GetPivotExt() const { return m_pose.pivotExternalOffset; }
    void    ClearPivotExt() { m_pose.pivotExternalOffset = Vector3::Zero; }

    Vector3 GetPivot() const { return m_pose.pivotCurWorld; }
    Vector3 GetBasePivot() const { return GetBasePivotTargetPos(m_target) + m_pose.pivotInternalOffset; }

    _bool   IsDistHit() const { return m_hitDist; }
    _float  GetOffsetY() const { return m_prof.offsetY; }

    OBJECT_HANDLE GetTarget() const { return m_target; }

private:
    void    ClampTargets();

    Vector3 GetPivotPos() const { return m_pose.pivotCurWorld; }
    Vector3 GetPivotTargetPos() const;
    float   GetDist() const;
    void    ApplyPose(_float dt, const OrbitLockEval& lockRes);

    Vector3 GetFoot() const;
    Vector3 GetBasePivotTargetPos(OBJECT_HANDLE h) const;

private:
    void    AutoYaw_OnTarget();
    void    AutoYaw_OnInput();
    _float  EvalAutoYaw(_float dt, const Vector3& foot, const Vector3& camLook, const Vector3& camRight, _float curYawDeg);

private:
    void    Switch_Reset();
    _bool   Switch_Active() const { return m_switch.active; }
    void    Switch_Begin(const Vector3& holdPivot);
    Vector3 Switch_EvalOffset(_float dt, const Vector3& basePivotNow);

private:
    OrbitInputEval   EvalInput(_float dt, _float lockW);
    OrbitCollideEval EvalCollideDist(_float dt, const OrbitProfile& prof, class PxScene* scene, CCharacterController* camCC,
        const Vector3& pivotWorld, _float distWanted, const Vector2& rotCurDeg, const Vector2& rotGoalDeg, _float distGoal);
    _float           CalcAllowDist(const OrbitProfile& prof, class PxScene* scene, CCharacterController* camCC,
        const Vector3& pivotWorld, _float distWanted, const Vector2& rotCurDeg, const Vector2& rotGoalDeg);
    void             SmoothPose(_float dt);

private:
    void          Lock_Reset();
    _bool         Lock_Active() const { return m_lock.active; }
    _bool         Lock_On()     const { return m_lock.active || m_lockBlend.active; }
    OBJECT_HANDLE Lock_Handle() const { return m_lock.handle; }
    _float        Lock_Weight() const { return m_lockBlend.weight; }
    void          Lock_Enter(OBJECT_HANDLE h, _float curDist);
    void          Lock_Switch(OBJECT_HANDLE h) { m_lock.handle = h; }
    void          Lock_Exit();
    void          Lock_Clear();         
    void          Lock_BlendStart(_bool entering);
    void          Lock_BlendUpdate(_float dt);

    OrbitLockEval EvalLock(_float dt, _float curYawDeg, _float curDist);

private:
    _bool         SkipUpdate(_float dt);
    void          UpdateSwitch(_float dt);
    void          ApplyInput(_float dt);
    OrbitLockEval ApplyLock(_float dt);
    void          ApplyAutoYaw(_float dt, const OrbitLockEval& lockRes);
    void          ApplyCollide(_float dt);

private:
    void    SyncPivot();
    void    CalcRotDeg(const Vector3& lookDir, Vector2& outRotDeg) const;
    void    GetBasePivot(const OBJECT_HANDLE& h, Vector3& outFootWorld, Vector3& outBasePivotWorld) const;
    void    SnapPose(const Vector3& camPos, const Quaternion& camRot, const Vector3& basePivotWorld);

private:
    OrbitPose     m_pose{};
    OrbitInput    m_input{};
    OrbitProfile  m_prof{};

    OrbitLockState   m_lock{};
    OrbitBlendState  m_lockBlend{};
    Vector3          m_lockFocus{};
    _bool            m_hasLockFocus = false;

    OrbitAutoYaw  m_autoYaw{};
    OrbitSwitch   m_switch{};
    OBJECT_HANDLE m_target{};

    _float  m_yawDeltaCapDeg   = 720.f;
    _float  m_pitchDeltaCapDeg = 540.f;
    _float  m_freeze           = 0.f;
    _bool   m_hitDist          = false;
    _bool   m_lockInput        = false;

public:
    static  COrbitCam* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END
