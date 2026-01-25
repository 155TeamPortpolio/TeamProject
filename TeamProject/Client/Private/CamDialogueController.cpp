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
    Vector3 GetLookDir(CTransform* tr)
    {
        const Vector4 look4 = tr->Dir(STATE::LOOK);
        Vector3 look(look4.x, look4.y, look4.z);
        if (look.LengthSquared() <= 0.f) look = Vector3(0.f, 0.f, 1.f);
        look.Normalize();
        return look;
    }

    Vector3 GetPivotWorldFromHandle(OBJECT_HANDLE handle, _float offsetY)
    {
        if (!handle.isValid()) return Vector3::Zero;

        auto obj = ObjectManager()->Request_Object(handle);
        if (!obj) return Vector3::Zero;

        auto cc = obj->Get_Component<CCharacterController>();
        if (!cc) return Vector3::Zero;

        const Vector4 foot4 = cc->Get_FootPosition();
        const Vector3 foot(foot4.x, foot4.y, foot4.z);

        return foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + offsetY, 0.f);
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
    holdFov = 30.f;
    dur = 0.5f;
    ease = EaseType::InOutSine;
    assumedPartnerFrontDist = 1.f;
    blendInit = false;
    fromPivotWorld = Vector3::Zero;
    toPivotWorld = Vector3::Zero;
    maxPivotOffset = 0.8f;
}

void CCamDialogueController::Begin(_float targetFov, _float blendSec, _float assumedPartnerFrontDist, OBJECT_HANDLE partnerHandle)
{
    assumedPartnerFrontDist = assumedPartnerFrontDist;
    partnerHandle = partnerHandle;

    holdFov = targetFov;

    holding = true;
    restoring = false;

    blending = (blendSec > 0.f);
    time = 0.f;
    dur = blendSec > 0.f ? blendSec : 0.f;

    fromFov = 0.f;
    toFov = 0.f;

    blendInit = false;
}

void CCamDialogueController::End(_float blendSec)
{
    holding = false;
    restoring = true;

    blending = (blendSec > 0.f);
    time = 0.f;
    dur = blendSec > 0.f ? blendSec : 0.f;

    fromFov = 0.f;
    toFov = 0.f;

    blendInit = false;
}

void CCamDialogueController::Update(_float dt, CCamera* cam, COrbitCam* orbit, CTransform* focusTr)
{
    if (!holding && !blending) return;

    const _float curFov = cam->Get_FOV();

    if ((holding || restoring) && savedFov == 0.f)
        savedFov = curFov;

    const Vector3 basePivot = orbit->GetBasePivotWorld();

    Vector3 desiredPivot = basePivot;

    {
        const _float offsetY = orbit->GetProfileOffsetY();
        const Vector3 partnerPivot = GetPivotWorldFromHandle(partnerHandle, offsetY);

        if (partnerPivot != Vector3::Zero)
        {
            desiredPivot = (basePivot + partnerPivot) * 0.5f;
        }
        else
        {
            const Vector3 look = GetLookDir(focusTr);
            const _float front = assumedPartnerFrontDist * 0.5f;
            desiredPivot = basePivot + look * front;
        }

        const Vector3 rawOffset = desiredPivot - basePivot;
        desiredPivot = basePivot + ClampOffset(rawOffset, maxPivotOffset);
    }

    _float desiredHoldFov = holdFov;
    if (orbit->IsDistConstrained()) desiredHoldFov = savedFov;

    if (blending && !blendInit)
    {
        fromFov = curFov;
        toFov = restoring ? savedFov : desiredHoldFov;

        fromPivotWorld = orbit->GetCurPivotWorld();
        toPivotWorld = restoring ? basePivot : desiredPivot;

        time = 0.f;
        blendInit = true;
    }

    if (blending)
    {
        if (dur <= 0.f)
        {
            cam->Set_FOV(toFov);
            orbit->SetPivotExternalOffset(toPivotWorld - orbit->GetBasePivotWorld());
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

            const _float toFov = restoring ? savedFov : desiredHoldFov;
            const Vector3 toPivot = restoring ? basePivot : desiredPivot;

            const _float outFov = fromFov + (toFov - fromFov) * t;
            const Vector3 outPivotWorld = Vector3::Lerp(fromPivotWorld, toPivot, t);

            cam->Set_FOV(outFov);
            orbit->SetPivotExternalOffset(outPivotWorld - orbit->GetBasePivotWorld());

            this->toFov = toFov;
            this->toPivotWorld = toPivot;
        }
    }
    else
    {
        if (holding)
        {
            cam->Set_FOV(desiredHoldFov);
            orbit->SetPivotExternalOffset(desiredPivot - orbit->GetBasePivotWorld());
        }
        else
        {
            cam->Set_FOV(savedFov);
            orbit->SetPivotExternalOffset(basePivot - orbit->GetBasePivotWorld());
        }
    }

    if (!blending && restoring)
    {
        orbit->ClearPivotExternalOffset();

        restoring = false;
        holding = false;
        savedFov = 0.f;

        fromFov = 0.f;
        toFov = 0.f;

        blendInit = false;
        fromPivotWorld = Vector3::Zero;
        toPivotWorld = Vector3::Zero;

        partnerHandle.Reset();
    }
}