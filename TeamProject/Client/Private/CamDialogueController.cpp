#include "pch.h"
#include "CamDialogueController.h"
#include "OrbitCam.h"
#include "FieldSystem.h"
// Engine
#include "GameInstance.h"
#include "Camera.h"
#include "Helper_Func.h"
#include "CharacterController.h"

namespace
{
    struct PivotSample
    {
        Vector3 basePivot{};
        Vector3 facePivot{};
        _bool   valid = false;
    };

    PivotSample SamplePivots(OBJECT_HANDLE h, _float offsetY, _float faceYOffsetMul)
    {
        PivotSample s{};

        if (!h.isValid()) return s;

        auto obj = ObjectManager()->Request_Object(h);
        if (!obj) return s;

        auto cc = obj->Get_Component<CCharacterController>();
        if (!cc) return s;

        const Vector4 foot4 = cc->Get_FootPosition();
        const Vector3 foot(foot4.x, foot4.y, foot4.z);

        const _float half = cc->Get_HalfSize();
        const Vector3 base = foot + Vector3(0.f, half * 1.5f + offsetY, 0.f);

        const _float faceY = half * 2.f * faceYOffsetMul;
        const Vector3 face = foot + Vector3(0.f, faceY + offsetY, 0.f);

        s.basePivot = base;
        s.facePivot = face;
        s.valid = true;
        return s;
    }

    Vector3 ClampOffset(const Vector3& offset, _float maxLen)
    {
        if (maxLen <= 0.f) return offset;

        const _float len = offset.Length();
        if (len <= maxLen) return offset;
        if (len <= 0.f) return Vector3::Zero;

        return offset * (maxLen / len);
    }

    float ExpAlpha(float tau, float dt)
    {
        float t = max(tau, 0.0001f);
        float a = 1.f - expf(-dt / t);
        return clamp(a, 0.f, 1.f);
    }

    float SmoothAngleDeg(float cur, float target, float a)
    {
        float d = Math::WrapDeg(target - cur);
        return cur + d * a;
    }
}

void CCamDialogueController::Reset()
{
    hold = false;
    blend = false;
    restore = false;

    fovSaved = 0.f;
    fovHold = 30.f;

    partner.Reset();

    blendTime = 0.f;
    blendDur = 0.5f;

    fovFrom = 0.f;
    fovTo = 0.f;

    pivotFrom = Vector3::Zero;
    pivotTo = Vector3::Zero;

    hasBlendInit = false;

    ease = EaseType::InOutCubic;

    maxPivotOff = 0.8f;
    faceYOffsetMul = 0.85f;
}

void CCamDialogueController::Begin(_float fovHold, _float blendSec)
{
    this->fovHold = fovHold;

    hold = true;
    restore = false;

    blend = true;
    blendTime = 0.f;
    blendDur = blendSec > 0.f ? blendSec : 0.f;

    fovSaved = 0.f;
    fovFrom = 0.f;
    fovTo = 0.f;

    pivotFrom = Vector3::Zero;
    pivotTo = Vector3::Zero;

    hasBlendInit = false;

    sideDir = Vector3::Zero;
    sideInit = false;
    sideSign = 0;

    yawInit = false;
    yawSm = 0.f;
    wSm = 0.f;

    partner = FieldSystem()->GetInteractHandle();
}

void CCamDialogueController::End(_float blendSec)
{
    hold = false;
    restore = true;

    blend = true;
    blendTime = 0.f;
    blendDur = blendSec > 0.f ? blendSec : 0.f;

    fovFrom = 0.f;
    fovTo = 0.f;

    pivotFrom = Vector3::Zero;
    pivotTo = Vector3::Zero;

    hasBlendInit = false;
}

void CCamDialogueController::Update(_float dt, CCamera* cam, COrbitCam* orbit, CTransform* focusTr)
{
    if (!hold && !blend) return;

    const _float curFov = cam->Get_FOV();

    if ((hold || restore) && fovSaved == 0.f) fovSaved = curFov;

    if (hold && !partner.isValid())
        partner = FieldSystem()->GetInteractHandle();

    const _float offsetY = orbit->GetOffsetY();

    const OBJECT_HANDLE ih = FieldSystem()->GetInteractHandle();
    const OBJECT_HANDLE iph = FieldSystem()->GetInteractPartnerHandle();

    OBJECT_HANDLE aHandle = orbit->GetTarget();
    OBJECT_HANDLE bHandle = partner;

    if (hold)
    {
        if (ih.isValid())
        {
            if (iph.isValid())
            {
                aHandle = ih;
                bHandle = iph;
                partner = ih;
            }
            else
            {
                aHandle = orbit->GetTarget();
                bHandle = ih;
                partner = ih;
            }
        }
    }

    const PivotSample me = SamplePivots(aHandle, offsetY, faceYOffsetMul);
    if (!me.valid) return;

    const PivotSample other = SamplePivots(bHandle, offsetY, faceYOffsetMul);

    const Vector3 basePivot = orbit->GetBasePivot();

    Vector3 desiredPivot = basePivot;

    if (other.valid)
    {
        const Vector3 midXZ(
            (me.facePivot.x + other.facePivot.x) * 0.5f,
            0.f,
            (me.facePivot.z + other.facePivot.z) * 0.5f
        );

        const _float midY = (me.facePivot.y + other.facePivot.y) * 0.5f;
        desiredPivot = Vector3(midXZ.x, midY, midXZ.z);
    }

    const Vector3 rawOff = desiredPivot - basePivot;
    desiredPivot = basePivot + ClampOffset(rawOff, maxPivotOff);

    Vector3 desiredExt = desiredPivot - basePivot;

    _float desiredHoldFov = this->fovHold;

    if (orbit->IsDistHit())
    {
        desiredHoldFov = fovSaved;
        desiredExt = Vector3::Zero;
    }

    const _float blendDurSafe = max(blendDur, 0.f);

    _float blendTimeNext = blendTime;
    if (blend && blendDurSafe > 0.f) blendTimeNext = min(blendTime + dt, blendDurSafe);

    _float rawTNext = 1.f;
    if (blend && blendDurSafe > 0.f) rawTNext = clamp(blendTimeNext / blendDurSafe, 0.f, 1.f);

    const _float tBlendNext = (blend && blendDurSafe > 0.f) ? Math::ApplyEase(ease, rawTNext) : 1.f;

    if (restore)
    {
        const _float wOut = wSm * (blend ? (1.f - tBlendNext) : 1.f);

        if (wOut > 0.001f) orbit->DialogueYaw_Set(yawSm, wOut);
        else orbit->DialogueYaw_Clear();
    }
    else if (!hold || !other.valid || orbit->IsDistHit())
    {
        orbit->DialogueYaw_Clear();
        sideInit = false;
        sideSign = 0;
        yawInit = false;
        wSm = 0.f;
    }
    else
    {
        Vector3 ab = other.facePivot - me.facePivot;
        ab.y = 0.f;

        const _float abLen = ab.Length();
        if (abLen > 0.f)
        {
            ab /= abLen;

            const Vector3 sideA(-ab.z, 0.f, ab.x);

            if (!sideInit)
            {
                Vector3 camLook = cam->Get_Owner()->Get_Component<CTransform>()->Dir(STATE::LOOK);
                camLook.y = 0.f;
                camLook.Normalize();

                sideSign = (sideA.Dot(camLook) >= 0.f) ? 1 : -1;
                sideInit = true;
            }

            const Vector3 side = sideA * (_float)sideSign;

            const Vector3 delta = other.facePivot - me.facePivot;
            const _float sep = fabsf(delta.Dot(side));

            const _float sepMin = 0.35f;
            _float wRaw = (sepMin - sep) / sepMin;
            wRaw = clamp(wRaw, 0.f, 1.f);

            const _float yawGoalRaw = Math::WrapDeg(XMConvertToDegrees(atan2f(side.x, side.z)) + (_float)sideSign * yawBiasDeg);

            const _float aYaw = ExpAlpha(yawTau, dt);
            const _float aW = ExpAlpha(wTau, dt);

            if (!yawInit)
            {
                yawSm = yawGoalRaw;
                wSm = wRaw;
                yawInit = true;
            }

            yawSm = SmoothAngleDeg(yawSm, yawGoalRaw, aYaw);
            wSm = wSm + (wRaw - wSm) * aW;

            const _float wOut = wSm * (blend ? tBlendNext : 1.f);

            if (wOut > 0.001f) orbit->DialogueYaw_Set(yawSm, wOut);
            else orbit->DialogueYaw_Clear();
        }
        else
        {
            orbit->DialogueYaw_Clear();
            sideInit = false;
            sideSign = 0;
            yawInit = false;
            wSm = 0.f;
        }
    }

    const Vector3 curExt = orbit->GetPivotExt();

    if (blend && !hasBlendInit)
    {
        fovFrom = curFov;
        fovTo = restore ? fovSaved : desiredHoldFov;

        pivotFrom = curExt;
        pivotTo = restore ? Vector3::Zero : desiredExt;

        blendTime = 0.f;
        hasBlendInit = true;
    }

    _float outFov = curFov;
    Vector3 outExt = curExt;

    if (blend)
    {
        if (blendDurSafe <= 0.f)
        {
            outFov = restore ? fovSaved : desiredHoldFov;
            outExt = restore ? Vector3::Zero : pivotTo;
            blend = false;
        }
        else
        {
            blendTime = blendTimeNext;

            if (rawTNext >= 1.f) blend = false;

            const _float endFov = restore ? fovSaved : fovTo;
            const Vector3 endExt = restore ? Vector3::Zero : pivotTo;

            outFov = fovFrom + (endFov - fovFrom) * tBlendNext;
            outExt = Vector3::Lerp(pivotFrom, endExt, tBlendNext);
        }
    }
    else
    {
        if (hold)
        {
            outFov = desiredHoldFov;
            outExt = desiredExt;
        }
        else
        {
            outFov = fovSaved;
            outExt = Vector3::Zero;
        }
    }

    cam->Set_FOV(outFov);
    orbit->SetPivotExt(outExt);

    if (!blend && restore)
    {
        orbit->ClearPivotExt();

        restore = false;
        hold = false;

        fovSaved = 0.f;

        fovFrom = 0.f;
        fovTo = 0.f;

        pivotFrom = Vector3::Zero;
        pivotTo = Vector3::Zero;

        hasBlendInit = false;

        partner.Reset();

        orbit->DialogueYaw_Clear();
        sideInit = false;
        sideSign = 0;
        yawInit = false;
        wSm = 0.f;
    }
}
