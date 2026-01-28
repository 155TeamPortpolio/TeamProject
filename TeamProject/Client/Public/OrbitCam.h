#pragma once

#include "CamObject.h"
#include "OrbitCamTypes.h"

NS_BEGIN(Engine)
class CCharacterController;
NS_END

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
    void    SetTarget(OBJECT_HANDLE handle);
    void    ClearTarget();

    void    SyncFromCurTransform();
    void    SnapFromCamPose(const Vector3& camPos, const Quaternion& camRot);

    void    CaptureSnapshot(OrbitCamSnapshot& out) const;
    void    RestoreSnapshot(const OrbitCamSnapshot& snapshot);

    void    FreezeFor(_float sec) { if (sec > m_freezeRemain) m_freezeRemain = sec; }

    void    SetLockOn(OBJECT_HANDLE handle);
    void    ClearLockOn();

    void    Lock_Input() { m_inputLocked = true; }
    void    Unlock_Input() { m_inputLocked = false; }
    _bool   IsInputLocked() const { return m_inputLocked; }

    void    SetPivotExternalOffset(const Vector3& offset) { pose.pivotExternalOffset = offset; }
    Vector3 GetPivotExternalOffset() const { return pose.pivotExternalOffset; }
    void    ClearPivotExternalOffset() { pose.pivotExternalOffset = Vector3::Zero; }

    Vector3 GetCurPivotWorld() const { return pose.curPivot; }
    Vector3 GetBasePivotWorld() const { return GetBasePivotTargetPos(targetHandle) + pose.pivotInternalOffset; }

    _bool   IsDistConstrained() const { return m_distConstrained; }
    _float  GetProfileOffsetY() const { return profile.offsetY; }

    OBJECT_HANDLE GetTargetHandle() const { return targetHandle; }

private:
    void    ClampTargets();

    Vector3 GetPivotPos()       const { return pose.curPivot; }
    Vector3 GetPivotTargetPos() const;
    float   GetEffectiveDist()  const;
    void    ApplyOrbitPose(_float dt, const OrbitLockOnEvalResult& lockRes);

    Vector3 GetTargetFootPos() const;
    Vector3 GetBasePivotTargetPos(OBJECT_HANDLE handle) const;

private:
    void    AutoYaw_OnTargetChanged();
    void    AutoYaw_OnManualInput();
    _float  AutoYaw_EvaluateYawAddDeg(_float dt, const Vector3& footWorld, const Vector3& camLookWorld, const Vector3& camRightWorld, _float curTargetYawDeg);

private:
    void    TargetSwitch_Reset();
    _bool   TargetSwitch_IsActive() const { return m_targetSwitch.active; }
    void    TargetSwitch_BeginSwitch(const Vector3& holdPivotWorld);
    Vector3 TargetSwitch_EvaluateInternalOffset(_float dt, const Vector3& basePivotNow);

private:
    OrbitInputEvalResult Input_Evaluate(_float dt, _float lockOnWeight);

private:
    OrbitCollisionDistEvalResult CollisionDist_Evaluate(_float dt, const OrbitCamProfile& profile, class PxScene* scene, CCharacterController* camCC,
        const Vector3& pivotWorld, _float wantDist, const Vector2& curRotDeg, const Vector2& targetRotDeg, _float curGoalDist);
    _float  CollisionDist_ComputeAllowedDist(const OrbitCamProfile& profile, class PxScene* scene, CCharacterController* camCC,
        const Vector3& pivotWorld, _float wantDist, const Vector2& curRotDeg, const Vector2& targetRotDeg);

private:
    void    PoseSmoother_Smooth(_float dt);

private:
    void    LockOn_Reset();
    _bool   LockOn_IsActive() const { return m_lockState.active; }
    _bool   LockOn_IsBlending() const { return m_lockBlend.active; }
    _bool   LockOn_IsActiveOrBlending() const { return m_lockState.active || m_lockBlend.active; }
    OBJECT_HANDLE LockOn_GetHandle() const { return m_lockState.handle; }
    _float  LockOn_GetWeight() const { return m_lockBlend.weight; }

    void    LockOn_Enter(OBJECT_HANDLE handle, _float curTargetDist);
    void    LockOn_Switch(OBJECT_HANDLE handle) { m_lockState.handle = handle; }
    void    LockOn_BeginExit();
    void    LockOn_ForceClear();

    void    LockOn_StartBlend(_bool entering);
    void    LockOn_UpdateBlend(_float dt);

    OrbitLockOnEvalResult LockOn_Evaluate(_float dt, _float curTargetYawDeg, _float curTargetDist);

private:
    OrbitCamPoseState          pose{};
    OrbitCamInputState         input{};
    OrbitCamProfile            profile{};

    OrbitLockOnState           m_lockState{};
    OrbitLockOnBlendState      m_lockBlend{};
    Vector3                    m_lockFocusPos{};
    _bool                      m_lockHasFocusPos = false;

    OrbitAutoYawFollowState    m_autoYaw{};
    OrbitTargetSwitchState     m_targetSwitch{};

    OBJECT_HANDLE              targetHandle{};

    _float                     m_curMaxYawSpeedDeg = 720.f;
    _float                     m_curMaxPitchSpeedDeg = 540.f;
    _float                     m_freezeRemain = 0.f;
    _bool                      m_distConstrained = false;
    _bool                      m_inputLocked = false;

public:
    static  COrbitCam* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END
