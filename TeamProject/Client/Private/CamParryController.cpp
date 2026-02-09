#include "pch.h"
#include "CamParryController.h"
// Engine
#include "Helper_Func.h"
#include "CamDirector.h"

void CCamParryController::Reset()
{
    hold = false;
    blendIn = false;
    blendOut = false;

    fovSaved = 0.f;
    fovHold = 30.f;

    extSaved = {};
    extFrom = {};
    extTo = {};

    blendTime = 0.f;
    blendInDur = 0.08f;
    blendOutDur = 0.65f;

    holdTime = 0.f;
    holdDur = 0.12f;

    easeIn = EaseType::OutCubic;
    easeOut = EaseType::InOutSine;
}

void CCamParryController::Begin(_float fovHold_, _float blendInSec, _float holdSec)
{
    Reset();

    fovHold = fovHold_;
    blendInDur = blendInSec;
    holdDur = holdSec;

    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = orbit->Get_Component<CCamera>();

    fovSaved = cam->Get_FOV();
    extSaved = orbit->GetPivotExt();

    extFrom = extSaved;
    extTo = extSaved + Vector3(0.f, 0.28f, 0.f);

    orbit->ParryMode_Begin();
   // orbit->Lock_Input();

   // orbit->SetLockOn(CamDirector()->GetCurTarget());

    blendIn = true;
}

void CCamParryController::End(_float blendOutSec)
{
    if (!IsBusy()) return;

    blendOutDur = blendOutSec;

   // auto orbit = CamDirector()->GetOrbitCam();
   // orbit->ClearLockOn();

    blendTime = 0.f;
    hold = false;
    blendIn = false;
    blendOut = true;
}

void CCamParryController::Update(_float dt)
{
    if (!IsBusy()) return;

    auto orbit = CamDirector()->GetOrbitCam();
    auto cam = orbit->Get_Component<CCamera>();

    if (blendIn)
    {
        blendTime += dt;

        _float t = blendTime / blendInDur;
        if (t >= 1.f)
        {
            t = 1.f;
            blendIn = false;
            hold = true;
            blendTime = 0.f;
        }

        t = clamp(t, 0.f, 1.f);
        t = Math::ApplyEase(easeIn, t);

        cam->Set_FOV(fovSaved + (fovHold - fovSaved) * t);
        orbit->SetPivotExt(extFrom + (extTo - extFrom) * t);
        return;
    }

    if (hold)
    {
        holdTime += dt;
        cam->Set_FOV(fovHold);
        orbit->SetPivotExt(extTo);

        if (holdTime >= holdDur) End(blendOutDur);
        return;
    }

    if (blendOut)
    {
        blendTime += dt;

        _float t = blendTime / blendOutDur;
        if (t >= 1.f)
        {
            cam->Set_FOV(fovSaved);
            orbit->SetPivotExt(extSaved);

            orbit->Unlock_Input();
            orbit->ParryMode_End();

            Reset();
            return;
        }

        t = clamp(t, 0.f, 1.f);
        t = Math::ApplyEase(easeOut, t);

        cam->Set_FOV(fovHold + (fovSaved - fovHold) * t);
        orbit->SetPivotExt(extTo + (extSaved - extTo) * t);
    }
}