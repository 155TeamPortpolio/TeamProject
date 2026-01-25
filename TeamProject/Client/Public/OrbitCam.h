#pragma once

#include "CamObject.h"
#include "OrbitLockOnController.h"
#include "OrbitAutoYawFollowController.h"
#include "OrbitCollisionDistController.h"
#include "OrbitTargetSwitchController.h"
#include "OrbitInputController.h"
#include "OrbitPoseSmootherController.h"

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
    //void    Render_GUI()                override;

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

private:
    void    ClampTargets();

    Vector3 GetPivotPos()       const { return pose.curPivot; }
    Vector3 GetPivotTargetPos() const;
    float   GetEffectiveDist()  const;
    void    ApplyOrbitPose(_float dt, const OrbitLockOnEvalResult& lockRes);

    Vector3 GetTargetFootPos() const;
    Vector3 GetBasePivotTargetPos(OBJECT_HANDLE handle) const;

private:
    OrbitCamPoseState               pose{};
    OrbitCamInputState              input{};
    OrbitCamProfile                 profile{};
    COrbitLockOnController          lockOnCtrl{};
    COrbitAutoYawFollowController   autoYawCtrl{};
    COrbitCollisionDistController   collisionDistCtrl{};
    COrbitTargetSwitchController    targetSwitchCtrl{};
    COrbitInputController           inputCtrl{};
    COrbitPoseSmootherController    poseSmootherCtrl{};
    OBJECT_HANDLE                   targetHandle{};

    _float                          m_curMaxYawSpeedDeg = 720.f;
    _float                          m_curMaxPitchSpeedDeg = 540.f;
    _float                          m_freezeRemain = 0.f;
    _bool                           m_distConstrained = false;
    _bool                           m_inputLocked = false;

public:
    static  COrbitCam* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END