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

    PivotSample SamplePivots(OBJECT_HANDLE handle, _float offsetY, _float faceYOffsetMul)
    {
        PivotSample s{};

        if (!handle.isValid()) return s;

        auto obj = ObjectManager()->Request_Object(handle);
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
}

void CCamDialogueController::Reset()
{
    holding = false;
    blending = false;
    restoring = false;

    savedFov = 0.f;
    holdFov = 30.f;

    partnerHandle.Reset();

    time = 0.f;
    dur = 0.5f;

    fromFov = 0.f;
    toFov = 0.f;

    fromPivotWorld = Vector3::Zero;
    toPivotWorld = Vector3::Zero;

    blendInit = false;

    ease = EaseType::InOutSine;

    maxPivotOffset = 0.8f;
    faceYOffsetMul = 0.85f;
}

void CCamDialogueController::Begin(_float targetFov, _float blendSec)
{
    holdFov = targetFov;

    holding = true;
    restoring = false;

    blending = true;
    time = 0.f;
    dur = blendSec > 0.f ? blendSec : 0.f;

    fromFov = 0.f;
    toFov = 0.f;

    fromPivotWorld = Vector3::Zero;
    toPivotWorld = Vector3::Zero;

    blendInit = false;

    partnerHandle = FieldSystem()->GetInteractHandle();
}

void CCamDialogueController::End(_float blendSec)
{
    holding = false;
    restoring = true;

    blending = true;
    time = 0.f;
    dur = blendSec > 0.f ? blendSec : 0.f;

    fromFov = 0.f;
    toFov = 0.f;

    fromPivotWorld = Vector3::Zero;
    toPivotWorld = Vector3::Zero;

    blendInit = false;
}

void CCamDialogueController::Update(_float dt, CCamera* cam, COrbitCam* orbit, CTransform* focusTr)
{
    if (!holding && !blending) return;

    const _float curFov = cam->Get_FOV();

    if ((holding || restoring) && savedFov == 0.f) savedFov = curFov;

    if (holding && !partnerHandle.isValid())
        partnerHandle = FieldSystem()->GetInteractHandle();

    const _float offsetY = orbit->GetProfileOffsetY();

    const PivotSample me = SamplePivots(orbit->GetTargetHandle(), offsetY, faceYOffsetMul);
    if (!me.valid) return;

    const PivotSample partner = SamplePivots(partnerHandle, offsetY, faceYOffsetMul);

    Vector3 basePivot = me.basePivot;
    Vector3 desiredPivot = basePivot;

    if (partner.valid)
    {
        const Vector3 midXZ = Vector3(
            (me.facePivot.x + partner.facePivot.x) * 0.5f,
            0.f,
            (me.facePivot.z + partner.facePivot.z) * 0.5f
        );

        const _float midY = (me.facePivot.y + partner.facePivot.y) * 0.5f;

        desiredPivot = Vector3(midXZ.x, midY, midXZ.z);
    }

    {
        const Vector3 rawOffset = desiredPivot - basePivot;
        desiredPivot = basePivot + ClampOffset(rawOffset, maxPivotOffset);
    }

    _float desiredHoldFov = holdFov;

    if (orbit->IsDistConstrained())
    {
        desiredHoldFov = savedFov;
        desiredPivot = basePivot;
    }

    if (blending && !blendInit)
    {
        fromFov = curFov;
        toFov = restoring ? savedFov : desiredHoldFov;

        fromPivotWorld = orbit->GetCurPivotWorld();
        toPivotWorld = restoring ? basePivot : desiredPivot;

        time = 0.f;
        blendInit = true;
    }

    _float outFov = curFov;
    Vector3 outPivotWorld = orbit->GetCurPivotWorld();

    if (blending)
    {
        if (dur <= 0.f)
        {
            outFov = restoring ? savedFov : desiredHoldFov;
            outPivotWorld = restoring ? basePivot : desiredPivot;
            blending = false;
        }
        else
        {
            time += dt;

            _float rawT = time / dur;
            if (rawT >= 1.f)
            {
                rawT = 1.f;
                blending = false;
            }

            const _float t = Math::ApplyEase(ease, clamp(rawT, 0.f, 1.f));

            const _float endFov = restoring ? savedFov : toFov;
            const Vector3 endPivot = restoring ? basePivot : toPivotWorld;

            outFov = fromFov + (endFov - fromFov) * t;
            outPivotWorld = Vector3::Lerp(fromPivotWorld, endPivot, t);
        }
    }
    else
    {
        if (holding)
        {
            outFov = desiredHoldFov;
            outPivotWorld = desiredPivot;
        }
        else
        {
            outFov = savedFov;
            outPivotWorld = basePivot;
        }
    }

    cam->Set_FOV(outFov);

    orbit->SetPivotExternalOffset(outPivotWorld - basePivot);

    if (!blending && restoring)
    {
        orbit->ClearPivotExternalOffset();

        restoring = false;
        holding = false;

        savedFov = 0.f;

        fromFov = 0.f;
        toFov = 0.f;

        fromPivotWorld = Vector3::Zero;
        toPivotWorld = Vector3::Zero;

        blendInit = false;

        partnerHandle.Reset();
    }
}
