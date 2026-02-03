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

    ease = EaseType::InOutSine;

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

    fovFrom = 0.f;
    fovTo = 0.f;

    pivotFrom = Vector3::Zero;
    pivotTo = Vector3::Zero;

    hasBlendInit = false;

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

    const PivotSample me = SamplePivots(orbit->GetTarget(), offsetY, faceYOffsetMul);
    if (!me.valid) return;

    const PivotSample other = SamplePivots(partner, offsetY, faceYOffsetMul);

    Vector3 basePivot = me.basePivot;
    Vector3 desiredPivot = basePivot;

    if (other.valid)
    {
        const Vector3 midXZ = Vector3(
            (me.facePivot.x + other.facePivot.x) * 0.5f,
            0.f,
            (me.facePivot.z + other.facePivot.z) * 0.5f
        );

        const _float midY = (me.facePivot.y + other.facePivot.y) * 0.5f;

        desiredPivot = Vector3(midXZ.x, midY, midXZ.z);
    }

    const Vector3 rawOff = desiredPivot - basePivot;
    desiredPivot = basePivot + ClampOffset(rawOff, maxPivotOff);

    _float desiredHoldFov = this->fovHold;

    if (orbit->IsDistHit())
    {
        desiredHoldFov = fovSaved;
        desiredPivot = basePivot;
    }

    if (blend && !hasBlendInit)
    {
        fovFrom = curFov;
        fovTo = restore ? fovSaved : desiredHoldFov;

        pivotFrom = orbit->GetPivot();
        pivotTo = restore ? basePivot : desiredPivot;

        blendTime = 0.f;
        hasBlendInit = true;
    }

    _float outFov = curFov;
    Vector3 outPivot = orbit->GetPivot();

    if (blend)
    {
        if (blendDur <= 0.f)
        {
            outFov = restore ? fovSaved : desiredHoldFov;
            outPivot = restore ? basePivot : desiredPivot;
            blend = false;
        }
        else
        {
            blendTime += dt;

            _float rawT = blendTime / blendDur;
            if (rawT >= 1.f)
            {
                rawT = 1.f;
                blend = false;
            }

            const _float t = Math::ApplyEase(ease, clamp(rawT, 0.f, 1.f));

            const _float endFov = restore ? fovSaved : fovTo;
            const Vector3 endPivot = restore ? basePivot : pivotTo;

            outFov = fovFrom + (endFov - fovFrom) * t;
            outPivot = Vector3::Lerp(pivotFrom, endPivot, t);
        }
    }
    else
    {
        if (hold)
        {
            outFov = desiredHoldFov;
            outPivot = desiredPivot;
        }
        else
        {
            outFov = fovSaved;
            outPivot = basePivot;
        }
    }

    cam->Set_FOV(outFov);

    orbit->SetPivotExt(outPivot - basePivot);

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
    }
}
